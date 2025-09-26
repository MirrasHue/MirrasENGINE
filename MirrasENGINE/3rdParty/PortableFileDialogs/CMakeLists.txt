cmake_minimum_required(VERSION 3.20)

project(PortableFileDialogs)

# Fetch source files
file(GLOB_RECURSE pfd_src CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(pfd STATIC ${pfd_src})

target_include_directories(pfd
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
