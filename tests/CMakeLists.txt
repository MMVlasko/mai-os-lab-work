project(mai-os)
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

add_executable(LW1_test lab1_test.cpp)
target_include_directories(LW1_test PUBLIC ../LW1/include)
target_link_libraries(LW1_test PRIVATE utils parent GTest::GTest)

enable_testing()
add_test(NAME LW1_test COMMAND LW1_test)