cmake_minimum_required(VERSION 3.20)
project(labutlaan_benchmarks CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Timings only mean something for optimised code.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# The domain library under test, built from project/impl exactly as the verifier builds it.
# fom/benchmark_versions.py points this at an earlier iteration's project/impl.
set(LABUTLAAN_IMPL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../project/impl CACHE PATH "the project/impl to benchmark")
add_subdirectory(${LABUTLAAN_IMPL_DIR} labutlaan)

add_executable(lending_benchmark lending_benchmark.cpp)
target_link_libraries(lending_benchmark PRIVATE labutlaan)
target_compile_options(lending_benchmark PRIVATE -Wall -Wextra)
