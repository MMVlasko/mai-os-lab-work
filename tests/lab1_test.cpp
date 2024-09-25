#include <gtest/gtest.h> 

extern "C" {
    #include <string.h>
    #include <utils.h>
}

TEST(test_utils, basic_tests) 
{ 
    char str[] = "abcdef";
    char expected_str[] = "fedcba";
    ReverseString(str);
    ASSERT_TRUE(strcmp(str, expected_str) == 0);
    
} 
 
int main(int argc, char **argv) { 
    testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS(); 
}