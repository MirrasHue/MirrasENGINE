cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)

option(BUILD_SAMPLES "Build samples" OFF)

##################################################################

project(MirrasENGINE)

# Fetch source files
file(GLOB_RECURSE engine_src CONFIGURE_DEPENDS MirrasENGINE/src/*.cpp)
file(GLOB_RECURSE imgui_src CONFIGURE_DEPENDS MirrasENGINE/3rdParty/ImGui/src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${engine_src} ${imgui_src})

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/MirrasENGINE/src
        MirrasENGINE/3rdParty/ImGui/include
        MirrasENGINE/3rdParty/ImGui/include/imgui # So that internal ImGui includes don't brake
        MirrasENGINE/3rdParty/ImGui/include/imgui/backends # Same, just for not braking include paths
        MirrasENGINE/3rdParty/Glad/include
)

# GLFW Flags
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(MirrasENGINE/3rdParty/GLFW)

add_subdirectory(MirrasENGINE/3rdParty/spdlog)

target_link_libraries(${PROJECT_NAME} glfw spdlog)

##################################################################

project(Editor)

file(GLOB_RECURSE editor_src CONFIGURE_DEPENDS MirrasENGINE/Editor/src/*.cpp)

add_executable(${PROJECT_NAME} ${editor_src})

target_link_libraries(${PROJECT_NAME} PRIVATE MirrasENGINE)

##################################################################

if(BUILD_SAMPLES)
    add_subdirectory(Samples)
endif()
