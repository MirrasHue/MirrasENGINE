cmake_minimum_required(VERSION 3.20)

project(raylib)

# Fetch source files
file(GLOB_RECURSE raylib_src CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_library(${PROJECT_NAME} STATIC ${raylib_src})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
