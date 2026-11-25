# SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# libFuzzer fuzzers for simple packing (SIMPLE_PACKING_BUILD_FUZZERS, Clang only).
# Built standalone: the library objects are instrumented with the same sanitizers as the
# fuzzers (see SIMPLE_PACKING_FUZZER_* options in the top-level CMakeLists.txt), so this
# configuration cannot be combined with SIMPLE_PACKING_BUILD_TESTS in one build directory.

# sp_add_fuzzer(<name>)
function(sp_add_fuzzer name)
  add_executable(${name} ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cc)
  target_link_libraries(${name} PRIVATE simple_packing::simple_packing)
  target_compile_options(${name} PRIVATE ${SIMPLE_PACKING_FUZZER_COMPILE_OPTIONS} -Wall -Wextra)
  target_link_options(${name} PRIVATE ${SIMPLE_PACKING_FUZZER_LINK_OPTIONS})
endfunction()

sp_add_fuzzer(fuzz_simple_unpack)
