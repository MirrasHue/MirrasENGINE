cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 23)

project(sdl_benchmark)

include(FetchContent)
FetchContent_Declare(SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG d42bf59c669628d13dbc8cd41be93f2216211ff9
    GIT_SHALLOW OFF
    EXCLUDE_FROM_ALL
    SYSTEM)

set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3)

add_executable(sdl_single_texture single_texture.c)
target_link_libraries(sdl_single_texture SDL3::SDL3-static)

add_executable(sdl_multi_textures multi_textures.c)
target_link_libraries(sdl_multi_textures SDL3::SDL3-static)
