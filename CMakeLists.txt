cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)

set(BUILD_SAMPLES ON)

# Whether to dynamically link to the C++ runtime or not
# (Static linking didn't work on Ubuntu, because of some GLFW dependency)
set(DYNAMIC_RUNTIME ON)

set(RUN_UPDATE_THREAD_WIN32 ON)
set(RUN_UPDATE_THREAD_LINUX OFF)

##################################################################

project(MirrasENGINE)

include(cmake/runtime-selection.cmake)

# Fetch source files
file(GLOB_RECURSE engine_src CONFIGURE_DEPENDS MirrasENGINE/src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${engine_src})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}
        MirrasENGINE/src
        MirrasENGINE/3rdParty/Glad/include
        MirrasENGINE/3rdParty/sol2/include
        MirrasENGINE/3rdParty/stb/include
        MirrasENGINE/3rdParty/fkYAML/include
)

# GLFW Flags
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(MirrasENGINE/3rdParty/GLFW)
add_subdirectory(MirrasENGINE/3rdParty/raylib)
add_subdirectory(MirrasENGINE/3rdParty/ImGui)
add_subdirectory(MirrasENGINE/3rdParty/EnTT)
add_subdirectory(MirrasENGINE/3rdParty/GLM)
add_subdirectory(MirrasENGINE/3rdParty/MSDF-Atlas-Gen)
add_subdirectory(MirrasENGINE/3rdParty/Lua)
add_subdirectory(MirrasENGINE/3rdParty/ImGuizmo)
add_subdirectory(MirrasENGINE/3rdParty/PortableFileDialogs)

target_link_libraries(${PROJECT_NAME} glfw raylib ImGui EnTT glm msdf-atlas-gen Lua ImGuizmo pfd)

# Don't include spdlog in the distribution build
if(NOT DEFINED PRESET_NAME OR NOT PRESET_NAME STREQUAL "distribution")
    add_subdirectory(MirrasENGINE/3rdParty/spdlog)
    target_link_libraries(${PROJECT_NAME} spdlog)
endif()

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

if(WIN32 AND RUN_UPDATE_THREAD_WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC RUN_UPDATE_THREAD)
endif()

if(UNIX AND RUN_UPDATE_THREAD_LINUX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC RUN_UPDATE_THREAD)
endif()

# Forces GLM to default initialize its types
target_compile_definitions(${PROJECT_NAME} PUBLIC GLM_FORCE_CTOR_INIT)

if(MSVC)
    add_compile_options(/MP)
endif()

##################################################################

project(Editor)

file(GLOB_RECURSE editor_src CONFIGURE_DEPENDS MirrasENGINE/Editor/src/*.cpp)

add_executable(${PROJECT_NAME} ${editor_src})

target_link_libraries(${PROJECT_NAME} PRIVATE MirrasENGINE)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        MirrasENGINE/Editor/src
)

##################################################################

if(BUILD_SAMPLES)
    add_subdirectory(Samples)
endif()
