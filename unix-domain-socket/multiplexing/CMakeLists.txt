cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 11)
project(multiplexing)

add_executable(server server.cpp)
add_executable(client client.cpp)