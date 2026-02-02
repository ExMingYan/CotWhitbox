cmake_minimum_required(VERSION 3.21.0)
project(hitbox LANGUAGES C CXX)
set(CURRENT_COMPILER_ID ${CMAKE_CXX_COMPILER_ID})
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(builder)
add_subdirectory(DLLInjector)
add_subdirectory(Lua)

# Build Hitbox DLL
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/hitbox/font/MainFont.h
    COMMAND $<TARGET_FILE:builder> ${CMAKE_SOURCE_DIR}/hitbox/font/MainFont.ttf ${CMAKE_SOURCE_DIR}/hitbox/font/
    DEPENDS builder ${CMAKE_SOURCE_DIR}/hitbox/font/MainFont.ttf
    COMMENT "Generating MainFont.h from MainFont.ttf"
)
add_custom_target(GenerateFontHeader DEPENDS ${CMAKE_SOURCE_DIR}/hitbox/font/MainFont.h)

file(GLOB_RECURSE HitboxSOURCES hitbox/dllmain.cpp hitbox/**/*.cpp hitbox/minhook/src/*.c hitbox/minhook/src/hde/*.c)
list(APPEND HitboxSOURCES ${CMAKE_SOURCE_DIR}/resources/version.rc)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/hitbox/minhook/src/*.c
    ${CMAKE_SOURCE_DIR}/hitbox/minhook/src/hde/*.c
    PROPERTIES LANGUAGE C
)

add_library(hitbox SHARED ${HitboxSOURCES})
add_dependencies(hitbox GenerateFontHeader)

set_target_properties(hitbox PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/x64")

target_include_directories(hitbox PRIVATE
    ${CMAKE_SOURCE_DIR}/hitbox
)

target_compile_definitions(hitbox PRIVATE
    UNICODE
    _UNICODE
)

target_compile_options(hitbox PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-std:c++17>
    $<$<COMPILE_LANGUAGE:CXX>:/arch:AVX2>
)

set_target_properties(hitbox PROPERTIES CXX_EXTENSIONS OFF)

target_compile_features(hitbox PRIVATE cxx_std_17)
target_link_libraries(hitbox PRIVATE
    d3d12
    dxgi
    dbghelp
)

target_link_options(hitbox PRIVATE
    /MANIFEST:NO
)

# Build launcher executable
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/launcher/hitbox.h
    COMMAND $<TARGET_FILE:builder> $<TARGET_FILE:hitbox> ${CMAKE_SOURCE_DIR}/launcher/
    DEPENDS builder hitbox
    COMMENT "Generating hitbox.h from hitbox.dll"
)
add_custom_target(GenerateHitboxHeader DEPENDS ${CMAKE_SOURCE_DIR}/launcher/hitbox.h)

file(GLOB_RECURSE LauncherSOURCES launcher/main.cpp launcher/**/*.cpp)
file(GLOB_RECURSE LauncherResources ${CMAKE_SOURCE_DIR}/resources/*.rc)
add_executable(launcher ${LauncherSOURCES} ${LauncherResources})
add_dependencies(launcher GenerateHitboxHeader)

set_target_properties(launcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/x64")

target_compile_options(launcher PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-std:c++17>
    $<$<COMPILE_LANGUAGE:CXX>:/arch:AVX2>
)

set_target_properties(launcher PROPERTIES CXX_EXTENSIONS OFF)

target_compile_features(launcher PRIVATE cxx_std_17)
target_link_options(launcher PRIVATE
    /MANIFEST:NO
)

target_link_libraries(launcher PRIVATE
    user32
    kernel32
    advapi32
    shell32
)

# Build Proxy DLL
file(GLOB_RECURSE ProxySOURCES proxy/dllmain.cpp proxy/**/*.cpp)
add_library(proxy SHARED ${ProxySOURCES})
add_dependencies(proxy Lua)

set_target_properties(proxy PROPERTIES OUTPUT_NAME "dsoundProxy")
set_target_properties(proxy PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/x64")

target_include_directories(proxy PRIVATE
    ${CMAKE_SOURCE_DIR}/proxy
    ${CMAKE_SOURCE_DIR}
)

target_compile_definitions(proxy PRIVATE
    UNICODE
    _UNICODE
)

target_compile_options(proxy PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-std:c++17>
    $<$<COMPILE_LANGUAGE:CXX>:/arch:AVX2>
)

set_target_properties(proxy PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(proxy PRIVATE cxx_std_17)
target_link_libraries(proxy PRIVATE
    user32
    kernel32
    advapi32
    shell32
    Lua
)

target_link_options(proxy PRIVATE
    /MANIFEST:NO
)
