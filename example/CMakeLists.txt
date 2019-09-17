cmake_minimum_required(VERSION 3.14)
project(example)

set(CMAKE_CXX_STANDARD 11)

add_executable(example main.cpp)

target_link_libraries(example jshttpserver_uvw)
