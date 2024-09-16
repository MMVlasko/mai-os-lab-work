include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)


set(CMAKE_CXX_STANDARD 17)

add_executable(lab1_test lab1_test.cpp ../lab2/src/parent.c ../lab2/include/parent.h
        ../lab2/src/utils.c ../lab2/include/utils.h)

target_link_libraries(lab1_test PRIVATE GTest::GTest)
target_include_directories(lab1_test PRIVATE ../lab2/include)

add_test(lab1_test lab1_test)