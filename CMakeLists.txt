project(pgen)

cmake_minimum_required(VERSION 3.10)

find_package(fmt REQUIRED)
find_package(gflags REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_library(pgen-code
    pgen/ast.cc
    pgen/classify-symbols.cc
    pgen/code-generator.cc
    pgen/flags.cc
    pgen/gen-parser.cc
    pgen/ir.cc
    pgen/lexer.cc
    pgen/parser.cc
    pgen/token.cc
)

target_include_directories(pgen-code PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(pgen-code fmt gflags)

add_executable(pgen pgen/pgen.cc)
target_link_libraries(pgen pgen-code)
