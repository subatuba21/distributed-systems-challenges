cmake_minimum_required(VERSION 3.14)
project(broadcast)

# Enforce out-of-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds are not allowed. Please use a separate build directory.")
endif()

# Add the executable
add_executable(broadcast main.cpp)
target_link_libraries(broadcast PRIVATE shared)

