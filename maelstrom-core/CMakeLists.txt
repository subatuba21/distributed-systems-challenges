cmake_minimum_required(VERSION 3.14)
project(echo)

# Enforce out-of-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds are not allowed. Please use a separate build directory.")
endif()

set(SHARED_SOURCES src/node/node.cpp src/node/handlers.cpp src/node/parse.cpp)

add_library(shared SHARED ${SHARED_SOURCES})
# Add the executable

target_include_directories(shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
