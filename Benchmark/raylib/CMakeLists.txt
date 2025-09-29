cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 23)

project(raylib_benchmark)

include(FetchContent)
FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6d9e17594bbe7b205bc34464d7ab31bfc24dd46a
    GIT_SHALLOW OFF
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(raylib)

add_executable(raylib_single_texture single_texture.c)
target_link_libraries(raylib_single_texture raylib)

add_executable(raylib_multi_textures multi_textures.c)
target_link_libraries(raylib_multi_textures raylib)
