# Copyright (c) 2020 Roberto Raggi <roberto.raggi@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

cmake_minimum_required(VERSION 3.16)

project(pgen)

include(FetchContent)

find_package(fmt 6.1.2 CONFIG QUIET)

if(NOT fmt_FOUND)
    message(STATUS "fetching fmt")

    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG        7.0.2
    )

    FetchContent_MakeAvailable(fmt)
endif()

find_package(gflags CONFIG)

if(NOT gflags_FOUND)
    message(STATUS "fetching gflags")

    FetchContent_Declare(
        gflags
        GIT_REPOSITORY https://github.com/gflags/gflags
        GIT_TAG        v2.2.2
    )

    FetchContent_MakeAvailable(gflags)
endif()

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(pgen SOURCES)
add_executable(pgen ${SOURCES})

install(TARGETS pgen EXPORT pgen-targets DESTINATION bin)
install(EXPORT pgen-targets DESTINATION lib/pgen)

target_include_directories(pgen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
    target_compile_definitions(pgen
        PRIVATE GFLAGS_DLL_DECLARE_FLAG=
                GFLAGS_DLL_DEFINE_FLAG=
    )
endif()

target_link_libraries(pgen fmt::fmt gflags)

#
# explicit targets and comments to regenerate the parser.
#

add_custom_command(
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT pgen/parser-pgen.h
    DEPENDS pgen/parser.pgen
    COMMAND pgen pgen/parser.pgen -o pgen/parser-pgen.h
)

add_custom_target(gen-parser
    DEPENDS pgen/parser-pgen.h)
