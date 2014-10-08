cmake_minimum_required(VERSION 2.8.4)
project(db-lab)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/neworderrandom.cpp
    src/tables.hpp
    src/Types.cpp
    src/Types.hpp)

add_executable(db-lab ${SOURCE_FILES})
