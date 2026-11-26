# SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0
#
# Floating-point compiler options of simple_packing (see README.md, "Floating-point compiler options").
#
# Simple packing scales floating-point values to integers and computes the packing parameters: the
# results must be rounded exactly as written (bit-identical on every compiler and CPU, and to the
# legacy ecCodes code), and the checks for NaN and infinite values must not be optimised away. Each option is a cache variable with the values AUTO (the default), ON and OFF.
# AUTO enables an option only for the compilers that need it.
#
# Measured performance impact of each option alone (AMD Zen 3; GCC 16, Clang 22, icx 2026.2;
# -O3, -march=native and -march=x86-64; 7 GRIB fields, 47.8 MiB, cold caches; differences within
# about +-3 % are measurement noise):
#
#   -fno-fast-math     packing and unpacking: none.
#                      Correctness: keeps the checks for NaN and infinity and exact rounding.
#                      AUTO: on for icx/icpx (IntelLLVM) and icc (Intel): their default floating-
#                      point model (-fp-model=fast) allows unsafe optimisations (in the attack
#                      tests, 1736 of 7103 cases changed). A -ffast-math in CMAKE_CXX_FLAGS is not
#                      undone: configure warns, the tests fail; set SIMPLE_PACKING_NO_FAST_MATH=ON.
#   -ffp-contract=off  packing and unpacking: none (+-3 %).
#                      Correctness: without it, compilers fuse (x * 10^D - R) * 2^-E into
#                      multiply-adds where the CPU has them, which round differently: the coded
#                      values would depend on the compiler and the CPU.
#                      AUTO: on if a probe compiled with the project flags fuses a * b - c. That is
#                      GCC (default -ffp-contract=fast), Clang >= 14 (default "on") and icx/icpx
#                      (fast and precise models) on CPUs with fused multiply-add: -march=native or
#                      -march=haswell and later on x86-64; always on AArch64, POWER and s390x. When
#                      cross-compiling (the probe cannot run): on for GCC, Clang and IntelLLVM.
#   -fno-trapping-math packing: GCC +2 to +5 %, unpacking unchanged; Clang and icx/icpx none
#                      (their default already). Performance only: GCC's default -ftrapping-math
#                      keeps it from vectorising the range checks of the values. Simple packing does
#                      not use floating-point exception flags. AUTO: on for GCC.
#
# -fno-fast-math is also a link option (SIMPLE_PACKING_FP_LINK_FLAGS): linking with -ffast-math (GCC, Clang:
# crtfastmath.o) or with icx/icpx sets the CPU to flush subnormal numbers to zero, which
# -fno-fast-math at compile time does not undo.
#
# Defines SIMPLE_PACKING_FP_FLAGS (all enabled options, in this order: -fno-fast-math, -fno-trapping-math,
# -ffp-contract=off; Clang and icx reset the contraction with -fno-fast-math) and
# SIMPLE_PACKING_FP_SAFETY_FLAGS (the options needed for correct results, for the usage requirements of
# simple_packing::headers and for code that does the same arithmetic).

include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
include(CheckCXXSourceRuns)
include(CheckLinkerFlag)

set(SIMPLE_PACKING_NO_FAST_MATH AUTO CACHE STRING "Compile with -fno-fast-math: AUTO (icx/icpx, icc), ON or OFF")
set(SIMPLE_PACKING_FP_CONTRACT_OFF AUTO CACHE STRING "Compile with -ffp-contract=off: AUTO (compilers that fuse multiply-adds), ON or OFF")
set(SIMPLE_PACKING_NO_TRAPPING_MATH AUTO CACHE STRING "Compile with -fno-trapping-math: AUTO (GCC: faster), ON or OFF")
foreach(sp_option SIMPLE_PACKING_NO_FAST_MATH SIMPLE_PACKING_FP_CONTRACT_OFF SIMPLE_PACKING_NO_TRAPPING_MATH)
  set_property(CACHE ${sp_option} PROPERTY STRINGS AUTO ON OFF)
  if(NOT ${sp_option} MATCHES "^(AUTO|ON|OFF)$")
    message(FATAL_ERROR "${sp_option} must be AUTO, ON or OFF (it is ${${sp_option}})")
  endif()
endforeach()

# simple_packing_floating_point_flags(<arch flags>): sets SIMPLE_PACKING_FP_FLAGS and SIMPLE_PACKING_FP_SAFETY_FLAGS in the
# caller's scope; <arch flags> (e.g. -march=native) are used by the contraction probe
function(simple_packing_floating_point_flags)
  set(arch_flags ${ARGN})
  set(flags "")
  set(safety "")
  set(link_flags "")
  set(reasons "")

  # -fno-fast-math
  set(enable OFF)
  if(SIMPLE_PACKING_NO_FAST_MATH STREQUAL "ON")
    set(enable ON)
  elseif(SIMPLE_PACKING_NO_FAST_MATH STREQUAL "AUTO" AND CMAKE_CXX_COMPILER_ID MATCHES "^(IntelLLVM|Intel)$")
    set(enable ON)
    list(APPEND reasons "-fno-fast-math (the default floating-point model of ${CMAKE_CXX_COMPILER_ID} is fast)")
  endif()
  if(enable)
    check_cxx_compiler_flag(-fno-fast-math SIMPLE_PACKING_HAVE_NO_FAST_MATH)
    if(SIMPLE_PACKING_HAVE_NO_FAST_MATH)
      list(APPEND flags -fno-fast-math)
      list(APPEND safety -fno-fast-math)
    endif()
    check_linker_flag(CXX -fno-fast-math SIMPLE_PACKING_LINKER_HAS_NO_FAST_MATH)
    if(SIMPLE_PACKING_LINKER_HAS_NO_FAST_MATH)
      set(link_flags -fno-fast-math)
    endif()
  endif()

  # -fno-trapping-math
  set(enable OFF)
  if(SIMPLE_PACKING_NO_TRAPPING_MATH STREQUAL "ON")
    set(enable ON)
  elseif(SIMPLE_PACKING_NO_TRAPPING_MATH STREQUAL "AUTO" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(enable ON)
    list(APPEND reasons "-fno-trapping-math (GCC vectorises the range checks only with it)")
  endif()
  if(enable)
    check_cxx_compiler_flag(-fno-trapping-math SIMPLE_PACKING_HAVE_NO_TRAPPING_MATH)
    if(SIMPLE_PACKING_HAVE_NO_TRAPPING_MATH)
      list(APPEND flags -fno-trapping-math)
    endif()
  endif()

  # -ffp-contract=off: AUTO probes whether the compiler fuses a * b - c with the project flags
  set(enable OFF)
  if(SIMPLE_PACKING_FP_CONTRACT_OFF STREQUAL "ON")
    set(enable ON)
  elseif(SIMPLE_PACKING_FP_CONTRACT_OFF STREQUAL "AUTO")
    if(CMAKE_CROSSCOMPILING AND NOT CMAKE_CROSSCOMPILING_EMULATOR)
      if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang|IntelLLVM)$")
        set(enable ON)
        list(APPEND reasons "-ffp-contract=off (cross-compiling: ${CMAKE_CXX_COMPILER_ID} contracts by default where the target has fused multiply-add)")
      endif()
    else()
      # a * b rounds to 1 (2^-54 is lost); fused, a * b - c is -2^-54
      string(MD5 key "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} ${arch_flags} ${flags}")
      set(CMAKE_REQUIRED_FLAGS "-O2 ${arch_flags} ${flags}")
      string(REPLACE ";" " " CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
      set(CMAKE_REQUIRED_QUIET ON)
      check_cxx_source_runs("
        volatile double a = 1.0 + 0x1p-27, b = 1.0 - 0x1p-27, c = 1.0;
        int main() { const double x = a, y = b, z = c; return x * y - z == 0.0 ? 0 : 1; }"
        SIMPLE_PACKING_NOT_CONTRACTING_${key})
      if(NOT SIMPLE_PACKING_NOT_CONTRACTING_${key})
        set(enable ON)
        list(APPEND reasons "-ffp-contract=off (the compiler fuses multiply-adds with these flags)")
      endif()
    endif()
  endif()
  if(enable)
    check_cxx_compiler_flag(-ffp-contract=off SIMPLE_PACKING_HAVE_FP_CONTRACT_OFF)
    if(SIMPLE_PACKING_HAVE_FP_CONTRACT_OFF)
      list(APPEND flags -ffp-contract=off)
      list(APPEND safety -ffp-contract=off)
    endif()
  endif()

  # A -ffast-math (or -ffinite-math-only) of the user that the flags above do not undo
  string(MD5 key "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} ${flags}")
  set(CMAKE_REQUIRED_FLAGS "${flags}")
  string(REPLACE ";" " " CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
  set(CMAKE_REQUIRED_QUIET ON)
  check_cxx_source_compiles("
    #if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__)
    #error fast math
    #endif
    int main() { return 0; }" SIMPLE_PACKING_NO_FAST_MATH_IN_EFFECT_${key})
  if(NOT SIMPLE_PACKING_NO_FAST_MATH_IN_EFFECT_${key})
    message(WARNING "simple_packing: the compiler flags (CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}) enable -ffast-math or "
                    "-ffinite-math-only. Simple packing then cannot detect NaN and infinite values and may round "
                    "differently: the tests fail. Configure with -DSIMPLE_PACKING_NO_FAST_MATH=ON (adds -fno-fast-math) "
                    "or remove the option.")
  endif()

  if(reasons)
    string(REPLACE ";" ", " text "${reasons}")
    message(STATUS "simple_packing: floating-point options enabled automatically: ${text}")
  endif()
  message(STATUS "simple_packing: floating-point options: ${flags}")
  set(SIMPLE_PACKING_FP_FLAGS ${flags} PARENT_SCOPE)
  set(SIMPLE_PACKING_FP_SAFETY_FLAGS ${safety} PARENT_SCOPE)
  set(SIMPLE_PACKING_FP_LINK_FLAGS ${link_flags} PARENT_SCOPE)
endfunction()
