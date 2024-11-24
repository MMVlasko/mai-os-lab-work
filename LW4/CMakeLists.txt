set(CMAKE_C_STANDARD 11)
project(mai-os)

add_library(first SHARED src/first_implementation.c)
target_include_directories(first PRIVATE include)
target_link_libraries(first PUBLIC m)

add_library(second SHARED src/second_implementation.c)

add_executable(lab4l linking_main.c)
target_include_directories(lab4l PRIVATE include)
target_link_libraries(lab4l first second)

add_executable(lab4d dynamic_main.c)
target_include_directories(lab4d PRIVATE include)