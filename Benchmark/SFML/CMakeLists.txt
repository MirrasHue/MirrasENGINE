cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG b0b6013de6faa0d63217f85f5887f6a12f810dd8
    GIT_SHALLOW OFF
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)


project(sfml_single_texture)

add_executable(sfml_single_texture single_texture.cpp)
target_link_libraries(sfml_single_texture SFML::Graphics)


project(sfml_multi_texture)

add_executable(sfml_multi_texture multi_texture.cpp)
target_link_libraries(sfml_multi_texture SFML::Graphics)
