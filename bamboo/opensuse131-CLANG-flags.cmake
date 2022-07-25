#SET( CMAKE_C_FLAGS "-fsanitize=thread" CACHE STRING "Enable clang thread sanitizer" ) 

# Compilation fails for fortran bindings and clang
#SET( ENABLE_FORTRAN OFF CACHE BOOL "Disable Fortran")

# All python tests fails due to error loading the shared python lib: undefined symbol: __tsan_init
#SET( ENABLE_PYTHON  OFF CACHE BOOL "Disable Python")

SET(ENABLE_MEMFS           ON CACHE BOOL "Enable MEMFS")
