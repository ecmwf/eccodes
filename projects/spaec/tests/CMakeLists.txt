# Test names are prefixed to avoid collisions when built as part of ecCodes
set(SPAEC_TEST_PREFIX "spaec_")

# spaec_add_test(<name> [LIBS <libs...>])
# Builds <name>.cc into an executable and registers it as test spaec_<name>.
# Always compiled with ${SPAEC_OPT_FLAGS} (release mode).
function(spaec_add_test name)
  cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})
  add_executable(${name} ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cc ${ARG_SOURCES})
  target_link_libraries(${name} PRIVATE spaec::spaec ${ARG_LIBS})
  target_compile_options(${name} PRIVATE ${SPAEC_OPT_FLAGS})
  target_compile_definitions(${name} PRIVATE NDEBUG)
  add_test(NAME ${SPAEC_TEST_PREFIX}${name} COMMAND ${name})
endfunction()

find_package(Threads REQUIRED)

# spaec_label_tests(<label>)
# Labels all tests registered in the current directory with <label> and "spaec"
# (select them with ctest -L <label>).
function(spaec_label_tests label)
  get_property(tests DIRECTORY PROPERTY TESTS)
  if(tests)
    set_tests_properties(${tests} PROPERTIES LABELS "${label};spaec")
  endif()
endfunction()

# Unit tests of the components; regression tests for bugs that were found
add_subdirectory(unit)
add_subdirectory(regression)
