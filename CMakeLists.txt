project(pgen)

cmake_minimum_required(VERSION 3.9)

find_package(fmt REQUIRED)
find_package(gflags REQUIRED)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(pgen SOURCES)
add_executable(pgen ${SOURCES})

install(TARGETS pgen EXPORT pgen DESTINATION bin)

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
