
option(BUILD_PLAYGROUND "Build sample: Playground" ON)

if(BUILD_PLAYGROUND)
    # Fetch source files
    file(GLOB_RECURSE playground_src CONFIGURE_DEPENDS Playground/src/*.cpp)

    add_executable(Playground ${playground_src})

    target_link_libraries(Playground MirrasENGINE)

endif()

#############################################################################

option(BUILD_BENCHMARK "Build sample: Benchmark" ON)

if(BUILD_BENCHMARK)

    add_executable(mirras_single_texture Benchmark/src/single_texture.cpp)
    target_link_libraries(mirras_single_texture MirrasENGINE)

    add_executable(mirras_multi_textures Benchmark/src/multi_textures.cpp)
    target_link_libraries(mirras_multi_textures MirrasENGINE)

    if(MSVC)
        target_link_options(mirras_single_texture PRIVATE /STACK:2048000)
    endif()

endif()

#############################################################################

option(BUILD_ANIMATION "Build sample: Animation" ON)

if(BUILD_ANIMATION)
    # Fetch source files
    file(GLOB_RECURSE anim_src CONFIGURE_DEPENDS Animation/src/*.cpp)

    add_executable(Animation ${anim_src})

    target_link_libraries(Animation MirrasENGINE)

endif()
