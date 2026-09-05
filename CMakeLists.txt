cmake_minimum_required(VERSION 3.10)
project(rubiks_solver CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_executable(rubiks_solver
    src/main.cpp
    src/cube.cpp
    src/pieces.cpp
    src/pdb.cpp
    src/solver.cpp
)
