cmake_minimum_required(VERSION 3.20)
project(labutlaan CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(labutlaan STATIC
    src/lab.cpp
    src/lab_utstyr.cpp
    src/student.cpp
    src/utlaan.cpp
)
target_include_directories(labutlaan PUBLIC include)
target_compile_options(labutlaan PRIVATE -Wall -Wextra)

add_executable(demo src/demo.cpp)
target_link_libraries(demo PRIVATE labutlaan)
