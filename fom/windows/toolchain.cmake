# Native Windows (MSYS2 MinGW-w64) only: fom/measure_fom.py and fom/benchmark_versions.py set
# CMAKE_TOOLCHAIN_FILE to this file (decision D-18). Executables get no
# ".exe" suffix and no section symbols, as in WSL (details: no_exe_suffix.cmake). The verifier
# looks for "scenario_<name>" and reads nm, and tools/ must not be changed.
set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_CURRENT_LIST_DIR}/no_exe_suffix.cmake")
