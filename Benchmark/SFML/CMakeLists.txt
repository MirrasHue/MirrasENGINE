cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 23)

project(sfml_benchmark)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG dbe4f499ba2a290f1ff0b8b21e5352484f7300fa
    GIT_SHALLOW OFF
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(sfml_single_texture single_texture.cpp)
target_link_libraries(sfml_single_texture SFML::Graphics)

add_executable(sfml_multi_textures multi_textures.cpp)
target_link_libraries(sfml_multi_textures SFML::Graphics)
