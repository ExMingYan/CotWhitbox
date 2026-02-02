cmake_minimum_required(VERSION 3.21.0)
project(builder LANGUAGES CXX)
set(CURRENT_COMPILER_ID ${CMAKE_CXX_COMPILER_ID})
set(CMAKE_CXX_STANDARD 17)

add_executable(builder "")
set_target_properties(builder PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/x64")

target_compile_definitions(builder PRIVATE
    UNICODE
    _UNICODE
)

target_compile_options(builder PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-std:c++17>
    $<$<COMPILE_LANGUAGE:CXX>:/arch:AVX2>
)

if(CURRENT_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(builder PRIVATE /EHsc)
elseif(CURRENT_COMPILER_ID STREQUAL "Clang")
    target_compile_options(builder PRIVATE -fexceptions)
    target_compile_options(builder PRIVATE -fcxx-exceptions)
elseif(CURRENT_COMPILER_ID STREQUAL "GNU")
    target_compile_options(builder PRIVATE -fexceptions)
endif()

set_target_properties(builder PROPERTIES CXX_EXTENSIONS OFF)

target_compile_features(builder PRIVATE cxx_std_17)
target_link_libraries(builder PRIVATE
    user32
    kernel32
    advapi32
    shell32
)

target_link_options(builder PRIVATE
    /MANIFEST:NO
)

target_sources(builder PRIVATE 
    main.cpp
)
