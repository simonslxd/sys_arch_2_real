cmake_minimum_required(VERSION 3.20)
project(labutlaan_benchmarks CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Timings only mean something for optimised code.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# The domain library under test, built from project/impl exactly as the verifier builds it.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../project/impl labutlaan)

add_executable(lending_benchmark lending_benchmark.cpp)
target_link_libraries(lending_benchmark PRIVATE labutlaan)
target_compile_options(lending_benchmark PRIVATE -Wall -Wextra)
