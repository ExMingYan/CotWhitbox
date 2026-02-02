cmake_minimum_required(VERSION 3.21.0)
project(Lua LANGUAGES C)
set(CURRENT_COMPILER_ID ${CMAKE_C_COMPILER_ID})
set(CMAKE_C_STANDARD 11)

file(GLOB LUA_SOURCES "*.c")

add_library(Lua STATIC ${LUA_SOURCES})

set_target_properties(Lua PROPERTIES OUTPUT_NAME "Lua")
set_target_properties(Lua PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/x64")

target_compile_definitions(Lua PRIVATE
    UNICODE
    _UNICODE
)

target_compile_options(Lua PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-std:c11>
    $<$<COMPILE_LANGUAGE:C>:/arch:AVX2>
)
