cmake_minimum_required(VERSION 3.15)
project(backtester)

set(CMAKE_CXX_STANDARD 17)

# Include both C and C++ API headers
include_directories("../nlopt/src/api")   # for nlopt.h (C API)
include_directories("../nlopt/build")     # for nlopt.hpp (C++ API)

# Link to nlopt library
link_directories("../nlopt/build")

# All .cpp files in this folder are part of the build
file(GLOB SOURCES "*.cpp")

add_executable(backtester
    main.cpp
    garch.cpp
    scholes.cpp
    # chatgarch.cpp ← DO NOT list this
)
target_link_libraries(backtester nlopt)