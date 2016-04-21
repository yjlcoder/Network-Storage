cmake_minimum_required(VERSION 3.5)
project(NetworkHomework)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_TEST1_SERVER W0802-test1-server.cpp)
set(SOURCE_TEST1_CLIENT W0802-test1-client.cpp)

add_executable(test1-server ${SOURCE_TEST1_SERVER})
add_executable(test1-client ${SOURCE_TEST1_CLIENT})
