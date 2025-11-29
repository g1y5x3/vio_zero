cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project(vio_zero)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

add_compile_options(-O3 -march=native -ffast-math -funroll-loops)

find_package(OpenCV REQUIRED)

add_executable(benchmark_detectors src/benchmark_detectors.cpp)
target_link_libraries(benchmark_detectors ${OpenCV_LIBS})
