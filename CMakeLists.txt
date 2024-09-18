cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)

option(BUILD_SAMPLES "Build samples" ON)

##################################################################

project(MirrasENGINE)

# Fetch source files
file(GLOB_RECURSE engine_src CONFIGURE_DEPENDS MirrasENGINE/src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${engine_src})

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/MirrasENGINE/src
        MirrasENGINE/3rdParty/raylib/include
        MirrasENGINE/3rdParty/Glad/include
)

# GLFW Flags
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(MirrasENGINE/3rdParty/GLFW)

add_subdirectory(MirrasENGINE/3rdParty/ImGui)

add_subdirectory(MirrasENGINE/3rdParty/spdlog)

add_subdirectory(MirrasENGINE/3rdParty/glm)

target_link_libraries(${PROJECT_NAME} glfw ImGui spdlog glm)

##################################################################

project(Editor)

file(GLOB_RECURSE editor_src CONFIGURE_DEPENDS MirrasENGINE/Editor/src/*.cpp)

add_executable(${PROJECT_NAME} ${editor_src})

target_link_libraries(${PROJECT_NAME} PRIVATE MirrasENGINE)

##################################################################

if(BUILD_SAMPLES)
    add_subdirectory(Samples)
endif()
