project(pgen)

cmake_minimum_required(VERSION 3.9)

find_package(fmt REQUIRED)
find_package(gflags REQUIRED)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(pgen SOURCES)
add_executable(pgen ${SOURCES})

target_include_directories(pgen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(pgen fmt::fmt gflags_shared)
