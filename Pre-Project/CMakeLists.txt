cmake_minimum_required(VERSION 3.5)
project(NetworkHomework)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(test6 W1002-test6-UDPConnection.cpp W1002-test6-util.h)

add_executable(test6-out ${test6})

add_executable(test6-test W1002-test6-test.cpp)

add_executable(test6-test-in W1002-test6-testin.cpp)
add_executable(test6-test-out W1002-test6-testout.cpp)
