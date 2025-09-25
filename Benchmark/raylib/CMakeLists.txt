cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 23)

include(FetchContent)
FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 3f6d67c61c59affa4561de2ddf988e487093bc80
    GIT_SHALLOW OFF
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(raylib)


project(raylib_single_texture)

add_executable(raylib_single_texture single_texture.c)
target_link_libraries(raylib_single_texture raylib)


project(raylib_multi_texture)

add_executable(raylib_multi_texture multi_texture.c)
target_link_libraries(raylib_multi_texture raylib)
