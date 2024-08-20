
option(BUILD_PLAYGROUND "Build sample: Playground" ON)

if(BUILD_PLAYGROUND)
    # Fetch source files
    file(GLOB_RECURSE playground_src CONFIGURE_DEPENDS Playground/src/*.cpp)

    add_executable(Playground ${playground_src})

    target_link_libraries(Playground MirrasENGINE)

endif()

#############################################################################


