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
        MirrasENGINE/3rdParty/stb/include
)

# GLFW Flags
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(MirrasENGINE/3rdParty/GLFW)

add_subdirectory(MirrasENGINE/3rdParty/ImGui)

add_subdirectory(MirrasENGINE/3rdParty/spdlog)

add_subdirectory(MirrasENGINE/3rdParty/glm)

target_link_libraries(${PROJECT_NAME} glfw ImGui spdlog glm)

# Define a location for the generated binaries, separated by build type.
# Also simplify the specification of relative paths within the project structure,
# since we can easily go to its root directory and specify the desired path from there.
if(DEFINED PRESET_NAME) # Defined in the CMakePresets.json file
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PRESET_NAME})

elseif(DEFINED CMAKE_BUILD_TYPE) # Single-config generator
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

else(DEFINED CMAKE_CONFIGURATION_TYPES) # Multi-config generator
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()

# For the distribution build on Windows, disable the console window
if(WIN32 AND PRESET_NAME STREQUAL "distribution")
    if(MSVC)
        target_link_options(${PROJECT_NAME} INTERFACE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
    else()
        target_link_options(${PROJECT_NAME} INTERFACE -mwindows)
    endif()
endif()

##################################################################

project(Editor)

file(GLOB_RECURSE editor_src CONFIGURE_DEPENDS MirrasENGINE/Editor/src/*.cpp)

add_executable(${PROJECT_NAME} ${editor_src})

target_link_libraries(${PROJECT_NAME} PRIVATE MirrasENGINE)

##################################################################

if(BUILD_SAMPLES)
    add_subdirectory(Samples)
endif()
