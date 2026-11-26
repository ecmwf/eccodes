# SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0
#
# sp_bench: an overview of the speed of simple_packing (see sp_bench.cc)
find_package(Threads REQUIRED)
add_executable(sp_bench sp_bench.cc)
target_link_libraries(sp_bench PRIVATE simple_packing::simple_packing Threads::Threads)
target_compile_options(sp_bench PRIVATE ${SIMPLE_PACKING_OPT_FLAGS})
target_compile_definitions(sp_bench PRIVATE NDEBUG)

# utils_bench: the speed of the utilities of src/Utils.cc (see utils_bench.cc)
add_executable(utils_bench utils_bench.cc)
target_link_libraries(utils_bench PRIVATE simple_packing::simple_packing)
target_compile_options(utils_bench PRIVATE ${SIMPLE_PACKING_OPT_FLAGS})
target_compile_definitions(utils_bench PRIVATE NDEBUG)
