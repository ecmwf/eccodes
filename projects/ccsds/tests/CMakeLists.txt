# Test names are prefixed to avoid collisions when built as part of ecCodes
set(CCSDS_TEST_PREFIX "ccsds_")

# ccsds_add_test(<name> [LIBS <libs...>])
# Builds <name>.cc into an executable and registers it as test ccsds_<name>.
# Always compiled with -march=native -mtune=native -O3 (release mode).
function(ccsds_add_test name)
  cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})
  add_executable(${name} ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cc ${ARG_SOURCES})
  target_link_libraries(${name} PRIVATE ccsds::spaec ${ARG_LIBS})
  target_compile_options(${name} PRIVATE -march=native -mtune=native -O3)
  target_compile_definitions(${name} PRIVATE NDEBUG)
  add_test(NAME ${CCSDS_TEST_PREFIX}${name} COMMAND ${name})
endfunction()

find_package(Threads REQUIRED)

# ccsds_label_tests(<label>)
# Labels all tests registered in the current directory with <label> and "ccsds"
# (select them with ctest -L <label>).
function(ccsds_label_tests label)
  get_property(tests DIRECTORY PROPERTY TESTS)
  if(tests)
    set_tests_properties(${tests} PROPERTIES LABELS "${label};ccsds")
  endif()
endfunction()

# Unit tests of the components; regression tests for bugs that were found
add_subdirectory(unit)
add_subdirectory(regression)
