#include <gtest/gtest.h>
#include <cstdlib>

extern "C" {
    #include <headers.h>
    #include <dlfcn.h>
}

TEST(some_first, test) {
    int result = some(1);
    ASSERT_EQ(result, 3);
}

TEST(home_first, test) {
    int result = home(1);
    ASSERT_EQ(result, -1);
}

TEST(some_second, test) {
    static void *library = dlopen("../LW4/libsecond.so", RTLD_LAZY);

    if (library == NULL) {
        fprintf(stderr, "Error with loading library\n");
        exit(-1);
    }

    int (*_some)(int) = (int(*)(int))dlsym(library, "some");

    int result = _some(1);
    ASSERT_EQ(result, 3);
}

TEST(home_second, test) {
    static void *library = dlopen("../LW4/libsecond.so", RTLD_LAZY);

    if (library == NULL) {
        fprintf(stderr, "Error with loading library\n");
        exit(-1);
    }

    int (*_home)(int) = (int(*)(int))dlsym(library, "home");

    int result = _home(1);
    ASSERT_EQ(result, -1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}