# Compiling benchmarks

_You will need **CMake (3.28+)** and a recent compiler that supports **C++23**_

For each framework directory, run (**Ninja** generator is recommended):

    mkdir build
    cd build
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .

The generated executables will be located in the `/build` folders.
