cmake_minimum_required(VERSION 3.31)
project(proj_monte_carlo_pricer)

set(CMAKE_CXX_STANDARD 20)

add_executable(proj_monte_carlo_pricer
        src/main.cpp
        src/Option.cpp
        src/MonteCarlo.cpp)
