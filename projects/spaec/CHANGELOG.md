<!--
SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
SPDX-License-Identifier: Apache-2.0
-->

# Changelog

All notable changes to spaec. The version is defined in `include/spaec/version.h`. Before version
1.0.0 the interface may still change.

## 0.0.1 (unreleased)

First version.

- Lossless compression with CCSDS 121.0-B-3: `spaec::compress`, `spaec::decompress`,
  `spaec::Options`.
- Integer samples of 1 to 32 bits, unsigned or signed; floating-point values with a GRIB scaling.
- Block sizes 8, 16, 32, 64; reference sample intervals 1 to 4096; basic and restricted code
  options; unit-delay, bypass or no preprocessor.
- Split-sample selectors `Search` (the data of libaec) and `Estimate` (faster).
- One or more threads, with the same output (work in progress).
- Floating-point compiler options `ON` by default (`SPAEC_FP_CONTRACT_OFF`,
  `SPAEC_NO_FAST_MATH`, `SPAEC_NO_TRAPPING_MATH`).
- Tested on Linux (GCC, Clang, Intel icx; x86-64, AArch64, s390x) and on Windows with MinGW-w64.
