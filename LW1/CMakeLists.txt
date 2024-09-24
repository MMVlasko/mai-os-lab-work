set(CMAKE_C_STANDARD 11)
project(mai-os)

add_executable(lab1 main.c src/parent.c src/utils.c)

target_include_directories(lab1 PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_executable(child1 src/child1.c src/utils.c)

target_include_directories(child1 PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_executable(child2 src/child2.c src/utils.c)

target_include_directories(child2 PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_dependencies(lab1 child1 child2)