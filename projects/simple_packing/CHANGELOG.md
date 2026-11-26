<!--
SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
SPDX-License-Identifier: Apache-2.0
-->

# Changelog

All notable changes to simple_packing. The version is defined in
`include/simple_packing/version.h`. Before version 1.0.0 the interface may still change.

## 0.0.1 (unreleased)

First version.

- GRIB simple packing: `sp::scaling`, `sp::pack`, `sp::unpack`, `sp::Scaling`.
- Packing with 1 to 63 and unpacking with 1 to 64 bits per value, for `float` and `double`.
- The scaling methods of ecCodes: `bits`, `bestPrecision`, `decimals`, `bitsAndDecimals`; IEEE
  (GRIB2) and IBM (GRIB1) reference values.
- Packed data byte-identical to legacy ecCodes.
- Floating-point compiler options `ON` by default (`SIMPLE_PACKING_FP_CONTRACT_OFF`,
  `SIMPLE_PACKING_NO_FAST_MATH`, `SIMPLE_PACKING_NO_TRAPPING_MATH`).
- Tested on Linux (GCC, Clang, Intel icx; x86-64, AArch64, s390x) and on Windows with MinGW-w64.
