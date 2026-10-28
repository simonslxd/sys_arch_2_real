cmake_minimum_required(VERSION 3.20)
project(labutlaan_tests CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The domain library under test, built from project/impl exactly as the verifier builds it.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../project/impl labutlaan)

add_executable(domain_tests
    main.cpp
    test_lab.cpp
    test_lab_utstyr.cpp
    test_student.cpp
    test_utlaan.cpp
)
target_link_libraries(domain_tests PRIVATE labutlaan)
target_compile_options(domain_tests PRIVATE -Wall -Wextra)
