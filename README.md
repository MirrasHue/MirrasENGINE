# MirrasENGINE

2D Game Engine in development, written in modern C++

## Features

* OpenGL renderer (shapes, textures, shaders)
* On Windows, main loop does not block upon window events
* ImGui rendering

And more to come...

## How to build

_You will need **CMake (3.20+)** and a compiler that supports **C++23**_

Clone the repository with:

    git clone --recursive https://github.com/MirrasHue/MirrasENGINE.git

The easiest way to compile the engine, is to simply run these two commands at the root directory ([Ninja](https://github.com/ninja-build/ninja) generator required):

    cmake --preset <debug|release|distribution>
    cmake --build --preset <debug|release|distribution>

It's also possible to use another generator of your choice (`cmake --help` for the available options). By omiting `-G <generator-name>`, CMake will choose a default generator, that could end up not working (like _NMake Makefiles_, in my case). 

    mkdir build
    cd build
    cmake [-G <generator-name>] ..
    cmake --build . [--config Debug|Release]

After the compilation is done, the location of the generated executables may vary depending on the generator, they're only specific if using the CMake presets
