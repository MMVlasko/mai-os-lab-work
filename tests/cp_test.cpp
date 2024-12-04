#include <gtest/gtest.h>
#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <cstring>

#include <tools.h>

TEST(isCyclicUtil, test)
{
    auto id = 3;
    std::unordered_map<int, bool> visited1, recStack1;
    std::unordered_map<int, std::vector<int>> goodGraph = {
        {1, {}},
        {2, {1}},
        {3, {1, 2}}
    };

    std::unordered_map<int, bool> visited2, recStack2;
    std::unordered_map<int, std::vector<int>> badGraph = {
        {1, {2}},
        {2, {1}},
        {3, {1, 2}}
    };

    auto good = IsCyclicUtil(id, goodGraph, visited1, recStack1);
    auto bad = IsCyclicUtil(id, badGraph, visited2, recStack2);

    ASSERT_FALSE(good);
    ASSERT_TRUE(bad);
}

TEST(isCyclic, test)
{
    std::unordered_map<int, std::vector<int>> goodGraph = {
        {1, {}},
        {2, {1}},
        {3, {1, 2}}
    };

    std::unordered_map<int, std::vector<int>> badGraph = {
        {1, {2}},
        {2, {1}},
        {3, {1, 2}}
    };

    auto good = IsCyclic(goodGraph);
    auto bad = IsCyclic(badGraph);

    ASSERT_FALSE(good);
    ASSERT_TRUE(bad);
}

TEST(validateDAG, test)
{
    auto expectedResult = "Error: DAG contains cycles\nError: DAG is not a single connected component\n";

    std::unordered_map<int, std::vector<int>> goodGraph = {
        {1, {}},
        {2, {1}},
        {3, {1, 2}}
    };

    std::unordered_map<int, std::vector<int>> badGraph1 = {
        {1, {2}},
        {2, {1}},
        {3, {1, 2}}
    };

    std::unordered_map<int, std::vector<int>> badGraph2 = {
        {1, {}},
        {2, {1}},
        {3, {}},
        {3, {3}}
    };

    testing::internal::CaptureStderr();

    auto good = ValidateDAG(goodGraph);
    auto bad1 = ValidateDAG(badGraph1);
    auto bad2 = ValidateDAG(badGraph2);

    auto output = testing::internal::GetCapturedStderr();

    ASSERT_TRUE(good);
    ASSERT_FALSE(bad1);
    ASSERT_FALSE(bad2);
    ASSERT_EQ(output, expectedResult);
}

TEST(executeJob, test)
{
    auto expectedResult = "Starting job: some\nJob completed: some\n";

    auto name = "some";
    auto semaphore = nullptr;
    std::atomic<bool> errorFlag = false;
    const int execTime = 2;

    testing::internal::CaptureStdout();

    ExecuteJob(name, semaphore, errorFlag, execTime);

    auto output = testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, expectedResult);
}

TEST(threadProcess, test)
{
    auto expectedResult = "Starting job: a\nJob completed: a\nStarting job: b\nJob completed: "
                          "b\nStarting job: c\nJob completed: c\nStarting job: d\nJob completed: d\n";

    readyJobs.push(1);
    readyJobs.push(2);

    jobs = {
        {1, Job{"a", {}, "s1", 2, 1}},
        {2, Job{"b", {}, "s1", 2, 1}},
        {3, Job{"c", {1, 2}, "s1", 2, 1}},
        {4, Job{"d", {3}, "s1", 2, 1}},

    };

    jobNames = {
        {1, "a"},
        {2, "b"},
        {3, "c"},
        {4, "d"},

    };

    graph = {
        {1, {}},
        {2, {}},
        {3, {1, 2}},
        {4, {3}}
    };

    inDegree = {
        {1, 0},
        {2, 0},
        {3, 2},
        {4, 1}
    };

    sem_t semaphore;
    sem_init(&semaphore, 0, 2);

    semaphores = {
        {"s1", semaphore}
    };

    testing::internal::CaptureStdout();

    ThreadProcess(nullptr);

    auto output = testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, expectedResult);

    sem_destroy(&semaphore);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}