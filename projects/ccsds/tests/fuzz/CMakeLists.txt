# libFuzzer fuzzers for SpAec (CCSDS_BUILD_FUZZERS, Clang only).
# Each fuzzer is linked with the same sanitizer instrumentation as the library code
# (see CCSDS_FUZZER_* options in the top-level CMakeLists.txt).

# ccsds_add_fuzzer(<name> [LIBS <libs...>])
function(ccsds_add_fuzzer name)
  cmake_parse_arguments(ARG "" "" "LIBS" ${ARGN})
  add_executable(${name} ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cc)
  target_link_libraries(${name} PRIVATE ccsds::spaec ${ARG_LIBS})
  target_compile_options(${name} PRIVATE ${CCSDS_FUZZER_COMPILE_OPTIONS})
  target_link_options(${name} PRIVATE ${CCSDS_FUZZER_LINK_OPTIONS})
endfunction()

find_package(Threads REQUIRED)

# Sequential vs parallel SpAec decoder agreement
ccsds_add_fuzzer(fuzz_spaec_decode LIBS Threads::Threads)

# Seed corpus generator (a normal executable, not instrumented)
add_executable(ccsds_fuzz_seeds ${CMAKE_CURRENT_SOURCE_DIR}/generate_seeds.cc)
target_link_libraries(ccsds_fuzz_seeds PRIVATE ccsds::spaec)

