cmake_minimum_required(VERSION 2.8.4)
project(db-lab)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -Wno-unknown-pragmas -stdlib=libc++")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set(CMAKE_CXX_COMPILER clang++)

#set(CODEGEN_RELATIONAL_ALGEBRA_SOURCE_FILES
#    codegen-relational-algebra/codegen.cpp
#    codegen-relational-algebra/operator.hpp)
#
#add_executable(db-lab-codegen-relational-algebra ${CODEGEN_RELATIONAL_ALGEBRA_SOURCE_FILES})

set(CODEGEN_SOURCE_FILES
    codegen/codegen.cpp
    codegen/Parser.hpp
    codegen/Schema.hpp
    codegen/Types.hpp
    codegen/Parser.cpp
    codegen/Schema.cpp)

add_executable(db-lab-codegen ${CODEGEN_SOURCE_FILES})

set(SOURCE_FILES
    src/main.cpp
    src/parser.hpp
    src/parser.cpp
    src/operator.hpp
    src/types.hpp
    src/table.hpp
    src/types.cpp)

add_executable(db-lab ${SOURCE_FILES})
target_link_libraries(db-lab readline dl)

add_custom_target(codegen
  COMMAND db-lab-codegen ../data/schema.sql
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "genereate code in ${CMAKE_CURRENT_SOURCE_DIR}"
  SOURCES ${CODEGEN_SOURCE_FILES}
)

add_dependencies(db-lab codegen)
