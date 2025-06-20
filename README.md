# MirrasENGINE

2D Game Engine in development, written in modern C++

## Features

* Cross-platform (Windows, Linux and macOS)
* OpenGL renderer (shapes, textures, shaders)
* On Windows, the application does not block upon window events (resizing, dragging or holding window buttons/borders)
* Sharp text rendering with _msdfgen_
* ImGui rendering
* Scene management with ECS (WIP)
* Editor (WIP)

And more to come...

## How to build

_You will need **CMake (3.20+)** and a recent compiler that supports **C++23**_

_For **Linux**, you will also need to install some dependencies required by GLFW. Just follow these [instructions](https://www.glfw.org/docs/latest/compile_guide.html#compile_deps), according to your specific system_

Clone the repository with:

    git clone https://github.com/MirrasHue/MirrasENGINE.git
    cd MirrasENGINE
    git submodule update --init --recursive --depth 1

_Or if you don't want the full commit history of the engine, clone with this instead:_

    git clone --recursive --depth 1 --shallow-submodules https://github.com/MirrasHue/MirrasENGINE.git

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

So far, the engine has been tested on Windows 10 / 11 (with GCC/MSVC) and Ubuntu (version 22.04 with GCC 13.1). It is compiling on macOS 15 (Apple Clang 16.0) via GitHub Actions, and I was able to run it on macOS 13 at my university. Apple Clang 14.0.2 had to be used, and it's missing some features, so the code was adjusted slightly just to make it work there (setting the working directory in particular seems to behave differently, as the current path when the program starts doesn't look to be the same where the executable is located at, or it's a problem with their implementation of std::filesystem not working with ../../ to get to the project's root directory).
