#include <controller.h>
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <thread>

#include <tools.h>
#include <worker.h>

TEST(insertNode, test)
{
    auto id = 1;
    std::shared_ptr<Node> root = nullptr;

    auto ok = InsertNode(root, id);

    ASSERT_TRUE(ok);
    ASSERT_EQ(root->id, id);

    auto ok1 = InsertNode(root, id + 1);

    ASSERT_TRUE(ok1);
    ASSERT_FALSE(root->right == nullptr);
    ASSERT_EQ(root->right->id, id + 1);

    ASSERT_TRUE(root->left == nullptr);

    auto bad = InsertNode(root, id);
    ASSERT_FALSE(bad);
}

TEST(findNode, test)
{
    auto id = 1;
    auto pid = 234;
    std::shared_ptr<Node> root = nullptr;

    InsertNode(root, id);
    InsertNode(root, id + 1);

    auto first = FindNode(root, id);
    auto second = FindNode(root, id + 1);
    auto bad = FindNode(root, id - 1);

    ASSERT_EQ(first->id, id);
    ASSERT_EQ(second->id, id + 1);
    ASSERT_TRUE(bad == nullptr);
}

TEST(pinAndDestroyNodes, test)
{
    auto expected_size = 0;
    auto id1 = 1111;
    auto id2 = 2222;
    std::shared_ptr<Node> root = nullptr;

    InsertNode(root, id1);
    InsertNode(root, id2);

    std::unordered_set<int> unavailable_nodes;

    PingNodes(root, unavailable_nodes);

    EXPECT_EQ(unavailable_nodes.size(), expected_size);
    TerminateNodes(root);
}

TEST(worker, test)
{
    const std::string msg = "exec 3 1 2 3";
    const std::string resp = "Ok:1: 6";
    auto id = 1;
    std::shared_ptr<Node> root = nullptr;

    InsertNode(root, id);

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