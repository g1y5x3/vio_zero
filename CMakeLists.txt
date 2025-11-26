cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_COMPILER   clang)
set(CMAKE_CXX_COMPILER clang++)

project(vio_zero)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -mcpu=cortex-a53 -flto")

find_package(OpenCV REQUIRED)

# Optimization flags for Pi Zero 2 W (Cortex-A53)

add_executable(benchmark_detectors src/benchmark_detectors.cpp)
target_link_libraries(benchmark_detectors ${OpenCV_LIBS})
