# Dynamically link against the C++ runtime, or statically (included in the executable)

if(NOT DYNAMIC_RUNTIME)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        # https://stackoverflow.com/questions/76297301/multiple-definition-error-when-statically-linking
        # Fixes linking error 'multiple definition of std::type_info::operator==(std::type_info const&) const'
        # when building in debug mode. Release build is not affected.
        # Seems to be a bug in GCC, maybe it'll be fixed in version 15.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
        
        add_link_options(-static)
    endif()
endif()

# Dynamic linking seems to be the default for GCC and MSVC, so we don't need to handle that option

# MSDF-Atlas-Gen has its own option, so we make sure to overwrite it
set(MSDF_ATLAS_DYNAMIC_RUNTIME ${DYNAMIC_RUNTIME} CACHE BOOL "" FORCE)
