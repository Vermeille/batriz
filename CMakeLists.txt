cmake_minimum_required(VERSION 2.6)

project(BATRIZ CXX)

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wextra -ggdb3")

add_subdirectory(src)

include_directories(src)
add_subdirectory(tests)

