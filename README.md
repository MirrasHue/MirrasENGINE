# MirrasENGINE

2D Game Engine in development, written in modern C++

## Features

* Cross-platform (Windows and Linux)
* OpenGL renderer (shapes, textures, shaders)
* On Windows, the application does not block upon window events (resizing, dragging or holding window buttons/borders)
* Sharp text rendering with _msdfgen_
* ImGui rendering
* Scene management with ECS (WIP)
* Editor (WIP)

And more to come...

## How to build

_You will need **CMake (3.20+)** and a compiler that supports **C++23**_

Clone the repository with:

    git clone --recursive https://github.com/MirrasHue/MirrasENGINE.git

The easiest and recommended way to compile the engine, is to simply run these two commands at the project's root directory ([Ninja](https://github.com/ninja-build/ninja) generator required):

    cmake --preset <debug|release|distribution>
    cmake --build --preset <debug|release|distribution>

---

It's also possible to use another generator of your choice (run `cmake --help` for the available options), with the drawback of not being able to use the out-of-the-box `distribution` build, provided only with the CMake presets.

By omiting `-G <generator-name>`, CMake will choose a default generator, that could end up not working (like _NMake Makefiles_ on Windows, if Visual Studio Build Tools is not installed).

For single-configuration generators (e.g., Ninja and Unix Makefiles):

    mkdir build
    cd build
    cmake [-G <generator-name>] [-DCMAKE_BUILD_TYPE=<Debug|Release>] ..
    cmake --build .

For multi-configuration generators (e.g., Visual Studio and Xcode):

    mkdir build
    cd build
    cmake [-G <generator-name>] ..
    cmake --build . [--config <Debug|Release>]

---

_After the compilation is done, the generated executables will be located in the `/bin` folder_


## Considerations

So far, the engine has been tested on Windows (with GCC/MSVC) and Ubuntu (version 22.04 with GCC 13). It probably runs on macOS as well, but I will only be able to test that once I have access to that OS.
