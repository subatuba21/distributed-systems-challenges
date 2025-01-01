cmake_minimum_required(VERSION 3.27)

# Optionally set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent
include(FetchContent)

# Fetch RapidJSON
FetchContent_Declare(
  rapidjson
  GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
  GIT_TAG v1.1.0 # Use a stable release tag
)

set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(RAPIDJSON_BUILD_TESTS OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(rapidjson)

# Add subdirectories
add_subdirectory(maelstrom-core)
add_subdirectory(echo)

target_include_directories(shared PUBLIC
    ${rapidjson_SOURCE_DIR}/include
)

target_include_directories(echo PUBLIC
    ${rapidjson_SOURCE_DIR}/include
)