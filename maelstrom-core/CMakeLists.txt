cmake_minimum_required(VERSION 3.14)
project(echo)

# Enforce out-of-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds are not allowed. Please use a separate build directory.")
endif()

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

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

# Add the executable
add_executable(program src/main.cpp src/node.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)


# Include RapidJSON headers
target_include_directories(program PRIVATE ${rapidjson_SOURCE_DIR}/include)

# Add compiler flags
target_compile_options(program PRIVATE -Wall -O2)
