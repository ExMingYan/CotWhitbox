cmake_minimum_required(VERSION 3.21.0)
project(DLLInjector LANGUAGES CXX)
set(CURRENT_COMPILER_ID ${CMAKE_CXX_COMPILER_ID})
set(CMAKE_CXX_STANDARD 17)

add_executable(DLLInjector "")
set_target_properties(DLLInjector PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/x64")

target_compile_definitions(DLLInjector PRIVATE
    UNICODE
    _UNICODE
)

target_compile_options(DLLInjector PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-std:c++17>
    $<$<COMPILE_LANGUAGE:CXX>:/arch:AVX2>
)

if(CURRENT_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(DLLInjector PRIVATE /EHsc)
elseif(CURRENT_COMPILER_ID STREQUAL "Clang")
    target_compile_options(DLLInjector PRIVATE -fexceptions)
    target_compile_options(DLLInjector PRIVATE -fcxx-exceptions)
elseif(CURRENT_COMPILER_ID STREQUAL "GNU")
    target_compile_options(DLLInjector PRIVATE -fexceptions)
endif()

set_target_properties(DLLInjector PROPERTIES CXX_EXTENSIONS OFF)

target_compile_features(DLLInjector PRIVATE cxx_std_17)
target_link_libraries(DLLInjector PRIVATE
    user32
    kernel32
    advapi32
    shell32
)

target_link_options(DLLInjector PRIVATE
    /MANIFEST:NO
)

target_sources(DLLInjector PRIVATE 
    main.cpp
    DLLInjector.cpp
)
