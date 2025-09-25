cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 23)

include(FetchContent)
FetchContent_Declare(SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG ed2cdceb66cc03dbcaf6c44659ec56c2e6767a91
    GIT_SHALLOW OFF
    EXCLUDE_FROM_ALL
    SYSTEM)

set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3)


project(sdl_single_texture)

add_executable(sdl_single_texture single_texture.c)
target_link_libraries(sdl_single_texture SDL3::SDL3-static)


project(sdl_multi_texture)

add_executable(sdl_multi_texture multi_texture.c)
target_link_libraries(sdl_multi_texture SDL3::SDL3-static)
