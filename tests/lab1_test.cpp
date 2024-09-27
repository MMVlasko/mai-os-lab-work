#include <gtest/gtest.h> 
#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <cstring>

extern "C" {
    #include <utils.h>
    #include <parent.h>
}

TEST(test_reverse_string, test_simple_string) 
{ 
    char str[] = "abcdef";
    char expected_str[] = "fedcba";
    ReverseString(str);
    ASSERT_TRUE(strcmp(str, expected_str) == 0);
    
} 

TEST(test_reverse_string, test_empty_string) 
{ 
    char str[] = "";
    char expected_str[] = "";
    ReverseString(str);
    ASSERT_TRUE(strcmp(str, expected_str) == 0);
    
} 

TEST(test_parent, test) 
{
    testing::internal::CaptureStdout();
    const char* fileWithInput = "input.txt";
    const char* fileWithOutput = "output.txt";

    constexpr int inputSize = 5;

    std::array<const char*, inputSize> input = {
            "first",
            "second",
            "abcd",
            "efghi",
            "q"
    };

    std::string expectedOutput = "Введите имя файла для первого дочернего процесса: \
Введите имя файла для второго дочернего \
процесса: Введите строку: Второй дочерний процесс: dcba\n\
Введите строку: Первый дочерний процесс: ihgfe\n\
Введите строку: ";

    {
        auto inFile = std::ofstream(fileWithInput);

        for(const auto& line : input) {
            inFile << line << '\n';
        }
    }

    auto deleter = [](FILE* file) {
        fclose(file);
    };

    std::unique_ptr<FILE, decltype(deleter)> inFile(fopen(fileWithInput, "r"), deleter);

    Parent("../LW1/child1", "../LW1/child2", inFile.get());

    std::string resultOutput = testing::internal::GetCapturedStdout();

    ASSERT_EQ(resultOutput, expectedOutput);

    std::ifstream file1(input[0]);
    std::ifstream file2(input[1]);

    ASSERT_TRUE(file1.good());
    ASSERT_TRUE(file2.good());

    std::string firstOutput;
    std::string secondOutput;

    std::getline(file1, firstOutput);
    std::getline(file2, secondOutput);

    std::string firstExpectedOtput = "ihgfe";
    std::string secondExpectedOtput = "dcba";

    ASSERT_EQ(firstOutput, firstExpectedOtput);
    ASSERT_EQ(secondOutput, secondExpectedOtput);

    auto removeIfExists = [](const char* path) {
        if(std::filesystem::exists(path)) {
            std::filesystem::remove(path);
        }
    };

    removeIfExists(fileWithInput);
    removeIfExists(fileWithOutput);
    removeIfExists(input[0]);
    removeIfExists(input[1]);
}
 
int main(int argc, char **argv) { 
    testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS(); 
}