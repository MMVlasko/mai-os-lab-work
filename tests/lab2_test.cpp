#include <gtest/gtest.h>
#include <cstdlib>

extern "C" {
    #include <utils.h>
}

TEST(Merge, test) 
{ 
    int input[] = {2, 4, 6, 8, 10, 1, 3, 5, 7, 9};
    int *data = (int*)malloc(sizeof(int) * 10);
    
    for (int i = 0; i < 10; ++i) {
        data[i] = input[i];
    }

    int expected[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    Merge(data, 0, 4, 9);

    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(data[i], expected[i]);
    }

    free(data);
} 

TEST(Sort, test) 
{ 
    int input[] = {2, 1, 9, 7, 5, 6, 4, 10, 3, 8};
    int *data = (int*)malloc(sizeof(int) * 10);

    for (int i = 0; i < 10; ++i) {
        data[i] = input[i];
    }

    int expected[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    MergeData mergeData = {data, 0, 9};

    Sort(&mergeData);

    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(data[i], expected[i]);
    }

    free(data);
}

TEST(ParallelSort, test) 
{ 
    int threadsCount = 4;
    sem_init(&semaphore, 0, threadsCount);

    int input[] = {2, 1, 9, 7, 5, 6, 4, 10, 3, 8};
    int *data = (int*)malloc(sizeof(int) * 10);

    for (int i = 0; i < 10; ++i) {
        data[i] = input[i];
    }

    int expected[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    MergeData mergeData = {data, 0, 9};

    ParallelSort(&mergeData);
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(data[i], expected[i]);
    }

    free(data);
    sem_destroy(&semaphore);
} 
 
int main(int argc, char **argv) { 
    testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS(); 
}