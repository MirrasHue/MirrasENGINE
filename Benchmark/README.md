# Compiling benchmarks

_You will need **CMake (3.28+)** and a recent compiler that supports **C++23**. (Only tested on Windows 11 with GCC 14.1, which can be found at: [Winlibs](https://winlibs.com/#download-release))_

For each framework directory, run (**Ninja** generator is recommended):

    mkdir build
    cd build
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .

This might take a while to download the framework. The generated executables will be located in their respective `/build` folders.

----

The files related to the benchmark of our framework are in the `Samples/Benchmark` directory. They are compiled alongside the framework when building it with (Ninja required, for ease of use):

    cmake --preset release
    cmake --build --preset release

The executables are going to be found in the `bin/release` folder.

It's important to pick the release preset, so that all frameworks are equally built with -O3 optimizations.
