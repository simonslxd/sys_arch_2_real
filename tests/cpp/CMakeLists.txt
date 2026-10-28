cmake_minimum_required(VERSION 3.20)
project(labutlaan_tests CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The domain library under test, built from project/impl exactly as the verifier builds it.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../project/impl labutlaan)

# The firmware's hardware-free part (firmware/station), the same sources the micro:bit runs.
set(STATION_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../firmware/station)

add_executable(unit_tests
    main.cpp
    test_event_writer.cpp
    test_input_decoder.cpp
    test_lab.cpp
    test_lab_utstyr.cpp
    test_station_controller.cpp
    test_student.cpp
    test_utlaan.cpp
    ${STATION_DIR}/event_writer.cpp
    ${STATION_DIR}/input_decoder.cpp
    ${STATION_DIR}/station_controller.cpp
)
target_include_directories(unit_tests PRIVATE ${STATION_DIR})
target_link_libraries(unit_tests PRIVATE labutlaan)
target_compile_options(unit_tests PRIVATE -Wall -Wextra)
target_compile_definitions(unit_tests PRIVATE
    GOLDEN_LINES_FILE="${CMAKE_CURRENT_SOURCE_DIR}/../../spec/serial-protocol-examples.txt")

# Replays a serial log recorded from the micro:bit through the same station code (tests/hardware).
add_executable(replay_log
    ../hardware/replay_log.cpp
    ${STATION_DIR}/event_writer.cpp
    ${STATION_DIR}/station_controller.cpp
)
target_include_directories(replay_log PRIVATE ${STATION_DIR})
target_link_libraries(replay_log PRIVATE labutlaan)
target_compile_options(replay_log PRIVATE -Wall -Wextra)
