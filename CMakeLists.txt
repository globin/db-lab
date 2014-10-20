cmake_minimum_required(VERSION 2.8.4)
project(db-lab)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")

set(CODEGEN_SOURCE_FILES
    codegen/parser.cpp
    codegen/Parser.cpp
    codegen/Schema.cpp)

add_executable(db-lab-codegen ${CODEGEN_SOURCE_FILES})

set(SOURCE_FILES
    src/neworderrandom.cpp
    src/types.cpp)

add_executable(db-lab ${SOURCE_FILES})

add_custom_target(codegen
  COMMAND db-lab-codegen
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "genereate code in ${CMAKE_CURRENT_SOURCE_DIR}"
  SOURCES ${CODEGEN_SOURCE_FILES}
)

add_dependencies(db-lab codegen)
