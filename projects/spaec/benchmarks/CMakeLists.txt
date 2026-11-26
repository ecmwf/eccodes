# SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0
#
# spaec_bench: an overview of the speed of SpAec (see spaec_bench.cc). With libaec (found by its
# header and library, or by libaec_DIR), libaec is measured as well.
find_package(Threads REQUIRED)
add_executable(spaec_bench spaec_bench.cc)
target_link_libraries(spaec_bench PRIVATE spaec::spaec Threads::Threads)
target_compile_options(spaec_bench PRIVATE ${SPAEC_OPT_FLAGS})
target_compile_definitions(spaec_bench PRIVATE NDEBUG)

find_path(SPAEC_BENCH_LIBAEC_INCLUDE_DIR libaec.h)
find_library(SPAEC_BENCH_LIBAEC_LIBRARY aec)
if(SPAEC_BENCH_LIBAEC_INCLUDE_DIR AND SPAEC_BENCH_LIBAEC_LIBRARY)
  target_include_directories(spaec_bench PRIVATE ${SPAEC_BENCH_LIBAEC_INCLUDE_DIR})
  target_link_libraries(spaec_bench PRIVATE ${SPAEC_BENCH_LIBAEC_LIBRARY})
  target_compile_definitions(spaec_bench PRIVATE SPAEC_BENCH_LIBAEC)
  message(STATUS "spaec: spaec_bench compares with libaec (${SPAEC_BENCH_LIBAEC_LIBRARY})")
endif()
