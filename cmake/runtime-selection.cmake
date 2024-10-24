# Dynamically link against the C++ runtime, or statically (included in the executable)

if(NOT DYNAMIC_RUNTIME)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        add_link_options(-static)
    endif()
endif()

# Dynamic linking seems to be the default for GCC and MSVC, so we don't need to handle that option

# MSDF-Atlas-Gen has its own option, so we make sure to overwrite it
set(MSDF_ATLAS_DYNAMIC_RUNTIME ${DYNAMIC_RUNTIME} CACHE BOOL "" FORCE)
