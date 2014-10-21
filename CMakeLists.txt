cmake_minimum_required(VERSION 2.8.4)
project(db-lab)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")

set(CODEGEN_SOURCE_FILES
    codegen/codegen.cpp
    codegen/Parser.hpp
    codegen/Schema.hpp
    codegen/Types.hpp
    codegen/Parser.cpp
    codegen/Schema.cpp)

add_executable(db-lab-codegen ${CODEGEN_SOURCE_FILES})

set(SOURCE_FILES
    src/neworderrandom.cpp
    src/types.hpp
    src/table.hpp
    src/tables_gen.hpp
    src/types.cpp)

add_executable(db-lab ${SOURCE_FILES})

add_custom_target(codegen
  COMMAND db-lab-codegen ../data/schema.sql
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "genereate code in ${CMAKE_CURRENT_SOURCE_DIR}"
  SOURCES ${CODEGEN_SOURCE_FILES}
)

add_dependencies(db-lab codegen)
