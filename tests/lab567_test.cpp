#include <controller.h>
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

#include <tools.h>
#include <worker.h>

TEST(insertNode, test)
{
    auto id = 1;
    auto pid = 234;
    std::shared_ptr<Node> root = nullptr;

    auto ok = InsertNode(root, id, pid);

    ASSERT_TRUE(ok);
    ASSERT_EQ(root->id, id);
    ASSERT_EQ(root->pid, pid);

    auto ok1 = InsertNode(root, id + 1, pid + 1);

    ASSERT_TRUE(ok1);
    ASSERT_FALSE(root->right == nullptr);
    ASSERT_EQ(root->right->id, id + 1);
    ASSERT_EQ(root->right->pid, pid + 1);

    ASSERT_TRUE(root->left == nullptr);

    auto bad = InsertNode(root, id, pid);
    ASSERT_FALSE(bad);
}

TEST(findNode, test)
{
    auto id = 1;
    auto pid = 234;
    std::shared_ptr<Node> root = nullptr;

    InsertNode(root, id, pid);
    InsertNode(root, id + 1, pid + 1);

    auto first = FindNode(root, id);
    auto second = FindNode(root, id + 1);
    auto bad = FindNode(root, id - 1);

    ASSERT_EQ(first->id, id);
    ASSERT_EQ(second->id, id + 1);
    ASSERT_TRUE(bad == nullptr);
}

TEST(pinAndDestroyNodes, test)
{
    auto expected_size = 1;
    auto id1 = 1111;
    auto id2 = 2222;
    std::shared_ptr<Node> root = nullptr;

    pid_t pid1 = fork();
    if (pid1 == 0) {
        Worker(id1);
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        Worker(id2);
        exit(0);
    }

    InsertNode(root, id1, pid1);
    InsertNode(root, id2, pid2);
    root->right->socket.close();
    kill(pid2, SIGKILL);

    std::unordered_set<int> unavailable_nodes;

    PingNodes(root, unavailable_nodes);

    int size = 0;
    for (auto _ : unavailable_nodes) {
        ++size;
    }
    EXPECT_EQ(size, expected_size);
    EXPECT_EQ(*unavailable_nodes.begin(), root->right->id);

    TerminateNodes(root);
}

TEST(worker, test)
{
    const std::string msg = "exec 3 1 2 3";
    const std::string resp = "Ok:1: 6";
    auto id = 1;
    std::shared_ptr<Node> root = nullptr;

    pid_t pid = fork();
    if (pid == 0) {
        Worker(id);
        exit(0);
    }

    InsertNode(root, id, pid);

    zmq::message_t message(msg);
    root->socket.send(message, zmq::send_flags::none);

    zmq::message_t reply;
    const auto ok = root->socket.recv(reply, zmq::recv_flags::none);

    EXPECT_TRUE(ok);
    EXPECT_EQ(reply.to_string(), resp);

    TerminateNodes(root);
}

TEST(controller, test)
{
    std::string expected_tracing = "\nOk:1: 7\nOk:2: 3\nOk: -1\n";
    auto fileWithInput = "input.txt";

    constexpr int inputSize = 6;

    std::array<const char*, inputSize> input = {
        "create 1111",
        "create 2222",
        "exec 1111 2 3 4",
        "exec 2222 3 1 1 1",
        "pingall",
        "exit"
    };

    {
        auto inFile = std::ofstream(fileWithInput);

        for(const auto& line : input) {
            inFile << line << '\n';
        }
    }

    auto inFile = std::ifstream(fileWithInput);

    testing::internal::CaptureStdout();
    Controller(inFile, true);
    auto output = testing::internal::GetCapturedStdout();

    ASSERT_FALSE(output.empty());

    inFile.close();
    if(std::filesystem::exists(fileWithInput)) {
        std::filesystem::remove(fileWithInput);
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}