set(CMAKE_C_STANDARD 11)
project(mai-os)

include(FetchContent)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.2
)

FetchContent_MakeAvailable(nlohmann_json)

add_library(cp_tools src/tools.cpp)
target_include_directories(cp_tools PRIVATE include)
target_link_libraries(cp_tools PRIVATE nlohmann_json::nlohmann_json)

add_executable(cp main.cpp)
target_include_directories(cp PRIVATE include)
target_link_libraries(cp PRIVATE cp_tools nlohmann_json::nlohmann_json)