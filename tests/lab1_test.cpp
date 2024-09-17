#include <gtest/gtest.h> 

#include "utils.h"
 
TEST(test_utils, basic_tests) 
{ 
    char str[] = "abcdef";
    reverse_string(str);
    ASSERT_TRUE(str == "fedcba");
} 
 
int main(int argc, char **argv) { 
    testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS(); 
}