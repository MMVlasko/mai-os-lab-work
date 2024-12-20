#include <gtest/gtest.h>
#include <cstdlib>
#include <cmath>

extern "C" {
    #include <headers.h>
    #include <dlfcn.h>
}

TEST(Pi_first, test) {
    constexpr int K = 100000;
    const float pi = Pi(K);
    ASSERT_TRUE(abs(pi - M_PI) < 0.0001);
}

TEST(E_first, test) {
    constexpr int x = 100000;
    const float e = E(x);
    ASSERT_TRUE(abs(e - M_E) < 0.0001);
}

TEST(Pi_second, test) {
    static void *library = dlopen("../LW4/libsecond.so", RTLD_LAZY);

    if (library == NULL) {
        fprintf(stderr, "Error with loading library2: %s\n", dlerror());
        exit(-1);
    }

    float (*DynamicPi)(int) = (float(*)(int))dlsym(library, "Pi");

    constexpr int K = 10000;
    const float pi = DynamicPi(K);
    ASSERT_TRUE(abs(pi - M_PI) < 0.01);
}

TEST(E_second, test) {
    static void *library = dlopen("../LW4/libsecond.so", RTLD_LAZY);

    if (library == NULL) {
        fprintf(stderr, "Error with loading library2: %s\n", dlerror());
        exit(-1);
    }

    float (*DynamicE)(int) = (float(*)(int))dlsym(library, "E");

    constexpr int x = 100000;
    const float e = DynamicE(x);
    ASSERT_TRUE(abs(e - M_E) < 0.0001);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}