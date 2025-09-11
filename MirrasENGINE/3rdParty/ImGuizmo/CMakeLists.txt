cmake_minimum_required(VERSION 3.20)

project(ImGuizmo)

# Fetch source files
file(GLOB_RECURSE imguizmo_src CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(${PROJECT_NAME} STATIC ${imguizmo_src})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/MirrasENGINE/3rdParty/ImGui/include
)
