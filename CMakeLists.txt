cmake_minimum_required(VERSION 2.8.4)
project(db-lab)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")

set(CODEGEN_SOURCE_FILES
    codegen/codegen.cpp)

add_executable(db-lab-codegen ${CODEGEN_SOURCE_FILES})

set(SOURCE_FILES
    src/neworderrandom.cpp
    src/types.cpp)

add_executable(db-lab ${SOURCE_FILES})
