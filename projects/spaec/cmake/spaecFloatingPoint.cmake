# SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0
#
# Floating-point compiler options of spaec (see README.md, "Floating-point compiler options").
#
# SpAec maps floating-point values to integer samples (GRIB CCSDS packing): the result must be
# rounded exactly as written (bit-identical on every compiler and CPU), and the checks for NaN and
# infinite values must not be optimised away. The integer coder itself does no floating-point
# arithmetic. Each option is a cache variable with the values ON (the default), AUTO and OFF.
# ON adds the option for every compiler that accepts it: where it is not needed it restates the
# default and changes nothing (measured: no cost), and the results do not depend on the probes and
# compiler lists of AUTO. AUTO enables an option only for the compilers that need it; OFF never.
#
# Measured performance impact of each option alone (AMD Zen 3; GCC 16, Clang 22, icx 2026.2;
# -O3, -march=native and -march=x86-64; 7 GRIB fields, 47.8 MiB, cold caches; differences within
# about +-3 % are measurement noise):
#
#   -fno-fast-math     integer samples: none. Doubles with a scaling: none.
#                      Correctness: keeps the checks for NaN and infinity and exact rounding.
#                      AUTO: on for icx/icpx (IntelLLVM) and icc (Intel): their default floating-
#                      point model (-fp-model=fast) allows unsafe optimisations (in the attack
#                      tests, 1736 of 7103 cases changed). A -ffast-math in CMAKE_CXX_FLAGS is not
#                      undone: configure warns, the tests fail; set SPAEC_NO_FAST_MATH=ON.
#   -ffp-contract=off  integer samples: none. Doubles with a scaling: none (+-3 %).
#                      Correctness: without it, compilers fuse (x * 10^D - R) * 2^-E into
#                      multiply-adds where the CPU has them, which round differently: the coded
#                      values would depend on the compiler and the CPU.
#                      AUTO: on if a probe compiled with the project flags fuses a * b - c. That is
#                      GCC (default -ffp-contract=fast), Clang >= 14 (default "on") and icx/icpx
#                      (fast and precise models) on CPUs with fused multiply-add: -march=native or
#                      -march=haswell and later on x86-64; always on AArch64, POWER and s390x. When
#                      cross-compiling (the probe cannot run): on for GCC, Clang and IntelLLVM.
#   -fno-trapping-math integer samples: none. Doubles with a scaling: GCC +12 to +21 % encoding,
#                      decoding unchanged; Clang and icx/icpx none (their default already).
#                      Performance only: GCC's default -ftrapping-math keeps it from vectorising
#                      the range checks of the values. SpAec does not use floating-point exception
#                      flags. AUTO: on for GCC.
#
# -fno-fast-math is also a link option (SPAEC_FP_LINK_FLAGS): linking with -ffast-math (GCC, Clang:
# crtfastmath.o) or with icx/icpx sets the CPU to flush subnormal numbers to zero, which
# -fno-fast-math at compile time does not undo.
#
# Defines SPAEC_FP_FLAGS (all enabled options, in this order: -fno-fast-math, -fno-trapping-math,
# -ffp-contract=off; Clang and icx reset the contraction with -fno-fast-math) and
# SPAEC_FP_SAFETY_FLAGS (the options needed for correct results, for the usage requirements of
# spaec::spaec and for code that does the same arithmetic).

include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
include(CheckCXXSourceRuns)
include(CheckLinkerFlag)

set(SPAEC_NO_FAST_MATH ON CACHE STRING "Compile with -fno-fast-math: ON (default), AUTO (icx/icpx, icc) or OFF")
set(SPAEC_FP_CONTRACT_OFF ON CACHE STRING "Compile with -ffp-contract=off: ON (default), AUTO (compilers that fuse multiply-adds) or OFF")
set(SPAEC_NO_TRAPPING_MATH ON CACHE STRING "Compile with -fno-trapping-math: ON (default), AUTO (GCC: faster) or OFF")
foreach(spaec_option SPAEC_NO_FAST_MATH SPAEC_FP_CONTRACT_OFF SPAEC_NO_TRAPPING_MATH)
  set_property(CACHE ${spaec_option} PROPERTY STRINGS ON AUTO OFF)
  if(NOT ${spaec_option} MATCHES "^(AUTO|ON|OFF)$")
    message(FATAL_ERROR "${spaec_option} must be ON, AUTO or OFF (it is ${${spaec_option}})")
  endif()
endforeach()

# The compiler flags of each configuration (CMAKE_CXX_FLAGS_<CONFIG>: all configurations of a
# multi-configuration generator, else CMAKE_BUILD_TYPE), followed by the flags the project adds: the
# probes below compile with what the build compiles with (CMAKE_CXX_FLAGS are added by the checks)
function(spaec_configuration_flags out)
  if(CMAKE_CONFIGURATION_TYPES)
    set(configs ${CMAKE_CONFIGURATION_TYPES})
  else()
    set(configs "${CMAKE_BUILD_TYPE}")
  endif()
  set(result "")
  foreach(config IN LISTS configs)
    string(TOUPPER "${config}" upper)
    list(APPEND result "${CMAKE_CXX_FLAGS_${upper}}")
  endforeach()
  set(${out} "${result}" PARENT_SCOPE)
endfunction()

# spaec_floating_point_flags(<arch flags>): sets SPAEC_FP_FLAGS and SPAEC_FP_SAFETY_FLAGS in the
# caller's scope; <arch flags> (e.g. -march=native) are used by the contraction probe
function(spaec_floating_point_flags)
  set(arch_flags ${ARGN})
  set(flags "")
  set(safety "")
  set(link_flags "")
  set(reasons "")

  # -fno-fast-math
  unset(reason)
  set(enable OFF)
  if(SPAEC_NO_FAST_MATH STREQUAL "ON")
    set(enable ON)
  elseif(SPAEC_NO_FAST_MATH STREQUAL "AUTO" AND CMAKE_CXX_COMPILER_ID MATCHES "^(IntelLLVM|Intel)$")
    set(enable ON)
    set(reason "-fno-fast-math (the default floating-point model of ${CMAKE_CXX_COMPILER_ID} is fast)")
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(IntelLLVM|Intel)$" AND NOT CMAKE_CXX_FLAGS MATCHES "-fp-model|-fno-fast-math")
    message(WARNING "spaec: SPAEC_NO_FAST_MATH=OFF with ${CMAKE_CXX_COMPILER_ID}, whose default floating-point "
                    "model is fast (it defines no macro to check): the results may differ and the tests fail. "
                    "Configure with -DSPAEC_NO_FAST_MATH=ON (the default) or AUTO.")
  endif()
  if(enable)
    check_cxx_compiler_flag(-fno-fast-math SPAEC_HAVE_NO_FAST_MATH)
    if(SPAEC_HAVE_NO_FAST_MATH)
      list(APPEND flags -fno-fast-math)
      list(APPEND safety -fno-fast-math)
      if(DEFINED reason)
        list(APPEND reasons "${reason}")
      endif()
    else()
      message(STATUS "spaec: the compiler does not accept -fno-fast-math: not added")
    endif()
    check_linker_flag(CXX -fno-fast-math SPAEC_LINKER_HAS_NO_FAST_MATH)
    if(SPAEC_LINKER_HAS_NO_FAST_MATH)
      set(link_flags -fno-fast-math)
    endif()
  endif()

  # -fno-trapping-math
  unset(reason)
  set(enable OFF)
  if(SPAEC_NO_TRAPPING_MATH STREQUAL "ON")
    set(enable ON)
  elseif(SPAEC_NO_TRAPPING_MATH STREQUAL "AUTO" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(enable ON)
    set(reason "-fno-trapping-math (GCC vectorises the range checks only with it)")
  endif()
  if(enable)
    check_cxx_compiler_flag(-fno-trapping-math SPAEC_HAVE_NO_TRAPPING_MATH)
    if(SPAEC_HAVE_NO_TRAPPING_MATH)
      list(APPEND flags -fno-trapping-math)
      if(DEFINED reason)
        list(APPEND reasons "${reason}")
      endif()
    else()
      message(STATUS "spaec: the compiler does not accept -fno-trapping-math: not added")
    endif()
  endif()

  # -ffp-contract=off: AUTO probes whether the compiler fuses a * b - c with the flags of the build
  unset(reason)
  set(enable OFF)
  spaec_configuration_flags(config_flags)
  if(SPAEC_FP_CONTRACT_OFF STREQUAL "ON")
    set(enable ON)
  elseif(SPAEC_FP_CONTRACT_OFF STREQUAL "AUTO")
    if(CMAKE_CROSSCOMPILING AND NOT CMAKE_CROSSCOMPILING_EMULATOR)
      if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang|IntelLLVM)$")
        set(enable ON)
        set(reason "-ffp-contract=off (cross-compiling: ${CMAKE_CXX_COMPILER_ID} contracts by default where the target has fused multiply-add)")
      endif()
    else()
      # a * b rounds to 1 (2^-54 is lost); fused, a * b - c is -2^-54. The result is compared as a
      # bit pattern: with -ffast-math a floating-point comparison could be folded away.
      foreach(config_flag_set IN LISTS config_flags)
        string(MD5 key "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} ${config_flag_set} ${arch_flags} ${flags}")
        set(CMAKE_REQUIRED_FLAGS "${config_flag_set} -O3 ${flags} ${arch_flags}")
        string(REPLACE ";" " " CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
        set(CMAKE_REQUIRED_QUIET ON)
        check_cxx_source_runs("
          #include <cstdint>
          #include <cstring>
          volatile double a = 1.0 + 0x1p-27, b = 1.0 - 0x1p-27, c = 1.0;
          int main()
          {
              const double x = a, y = b, z = c;
              const double r = x * y - z;
              std::uint64_t bits = 0;
              std::memcpy(&bits, &r, sizeof(bits));
              return bits == 0 ? 0 : 1;
          }"
          SPAEC_NOT_CONTRACTING_${key})
        if(NOT SPAEC_NOT_CONTRACTING_${key})
          set(enable ON)
          set(reason "-ffp-contract=off (the compiler fuses multiply-adds with these flags)")
        endif()
      endforeach()
    endif()
  endif()
  if(enable)
    check_cxx_compiler_flag(-ffp-contract=off SPAEC_HAVE_FP_CONTRACT_OFF)
    if(SPAEC_HAVE_FP_CONTRACT_OFF)
      list(APPEND flags -ffp-contract=off)
      list(APPEND safety -ffp-contract=off)
      if(DEFINED reason)
        list(APPEND reasons "${reason}")
      endif()
    else()
      message(STATUS "spaec: the compiler does not accept -ffp-contract=off: not added")
    endif()
  endif()

  # A -ffast-math (or -ffinite-math-only) of the user that the flags above do not undo, with the flags
  # of the build (a later -O level undoes -Ofast, but not -ffast-math)
  foreach(config_flag_set IN LISTS config_flags)
    string(MD5 key "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} ${config_flag_set} ${flags}")
    set(CMAKE_REQUIRED_FLAGS "${config_flag_set} -O3 ${flags}")
    string(REPLACE ";" " " CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
    set(CMAKE_REQUIRED_QUIET ON)
    check_cxx_source_compiles("
      #if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__)
      #error fast math
      #endif
      int main() { return 0; }" SPAEC_NO_FAST_MATH_IN_EFFECT_${key})
    if(NOT SPAEC_NO_FAST_MATH_IN_EFFECT_${key})
      message(WARNING "spaec: the compiler flags (CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}; of the configuration: "
                      "${config_flag_set}) enable -ffast-math or -ffinite-math-only. SpAec then cannot detect NaN "
                      "and infinite values and may round differently: the tests fail. Configure with "
                      "-DSPAEC_NO_FAST_MATH=ON (the default; adds -fno-fast-math) or remove the option.")
      break()
    endif()
  endforeach()

  if(reasons)
    string(REPLACE ";" ", " text "${reasons}")
    message(STATUS "spaec: floating-point options enabled automatically: ${text}")
  endif()
  message(STATUS "spaec: floating-point options: ${flags}")
  set(SPAEC_FP_FLAGS ${flags} PARENT_SCOPE)
  set(SPAEC_FP_SAFETY_FLAGS ${safety} PARENT_SCOPE)
  set(SPAEC_FP_LINK_FLAGS ${link_flags} PARENT_SCOPE)
endfunction()
