# Included by firmware/build.sh right after CODAL's project() call (CMAKE_PROJECT_INCLUDE).
#
# CODAL compiles everything as C++11. Our own sources in source/ (the domain model, the station
# and the device code) are C++20, like the PC build that the verifier checks (decision D-10).
# GCC uses the last -std it is given, and per-file options come after CODAL's flags.
#
# -Wno-register: CODAL's CMSIS header (libraries/codal-nrf52/inc/cmsis/cmsis_gcc.h) uses the
# `register` keyword, which C++17 and later warn about. Our code does not use it.
file(GLOB APP_SOURCES "${CMAKE_SOURCE_DIR}/source/*.cpp")
set_source_files_properties(${APP_SOURCES} PROPERTIES COMPILE_OPTIONS "-std=c++20;-Wno-register")
