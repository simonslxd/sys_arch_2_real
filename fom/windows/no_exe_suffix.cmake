# Read after CMake's Windows platform files (see toolchain.cmake), so it can change what they set.
# Two changes, both so that programs built here look to the verifier as they do in WSL:
#
# 1. No ".exe" suffix. An empty suffix is not enough: the MinGW linker adds ".exe" to an output
#    name without an extension. With "." it sees an extension, and Windows drops a trailing dot
#    from a file name, so "demo." is created as "demo".
set(CMAKE_EXECUTABLE_SUFFIX ".")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".")
set(CMAKE_EXECUTABLE_SUFFIX_C ".")

# 2. No section symbols. A PE program's symbol table also lists a ".text" symbol at the start of
#    every object file's code, which ELF programs don't. That symbol shares its address with the
#    object's first function (in the trace runtime: uml_trace_reference). The verifier keeps one
#    name per address, so the function is lost and no traced call can be resolved. objcopy
#    removes only those symbols, after linking; the program itself is unchanged.
find_program(FOM_OBJCOPY objcopy REQUIRED)
foreach(lang C CXX)
  if(DEFINED CMAKE_${lang}_LINK_EXECUTABLE)
    string(APPEND CMAKE_${lang}_LINK_EXECUTABLE
           " && \"${FOM_OBJCOPY}\" --wildcard --strip-symbol=.text* <TARGET>")
  endif()
endforeach()
