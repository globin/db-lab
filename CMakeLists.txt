cmake_minimum_required(VERSION 2.8.4)
project(db-lab)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")
set(CMAKE_CXX_COMPILER clang++)

set(SOURCE_FILES
    src/neworderrandom.cpp
    src/tables.hpp
    src/types.cpp
    src/types.hpp)

add_executable(db-lab ${SOURCE_FILES})
