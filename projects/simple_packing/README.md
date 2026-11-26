<!--
SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
SPDX-License-Identifier: Apache-2.0
-->

# simple_packing

simple_packing is a C++17 library for GRIB simple packing [1]. It chooses the packing parameters
and packs floating-point values Y into integers X of `bitsPerValue` bits:
`Y * 10^D = R + X * 2^E`, with the reference value R, the binary scale factor E and the decimal
scale factor D.

Version 0.0.1 (`sp::version()`, `SIMPLE_PACKING_VERSION` in `simple_packing/version.h`). Before
version 1.0.0 the interface may still change.

## Features

- One header, `simple_packing/simple_packing.h`, with `sp::scaling`, `sp::pack`, `sp::unpack` and
  `sp::Scaling` (reference value, binary and decimal scale factor, bits per value).
- Packing with 1 to 63 bits per value, unpacking with 1 to 64, for `float` and `double`.
- Four ways to choose the scaling, as in ecCodes:
  - `sp::Method::bits(n)`: the bits per value are given (`optimizeScaleFactor=0`).
  - `sp::Method::bestPrecision(n)`: the best use of n bits (`optimizeScaleFactor=1`).
  - `sp::Method::decimals(d)`: the decimal precision is given; the bits follow.
  - `sp::Method::bitsAndDecimals(n, d)`: both are given (CCSDS packing).
- Reference values as IEEE (GRIB2) or IBM (GRIB1) 32-bit floats.
- The packed data are byte-identical to legacy ecCodes.
- Errors are exceptions: `std::invalid_argument`, `std::runtime_error`, `std::length_error`.

## Usage

```cpp
#include "simple_packing/simple_packing.h"

std::vector<double> values{ 273.15, 280.4, 291.07, 265.9 };
sp::Scaling scaling = sp::scaling(values, sp::Method::bits(16));
std::vector<uint8_t> packed = sp::pack(values, scaling);
std::vector<double> back = sp::unpack<double>(packed, values.size(), scaling);
```

The packed data contain no parameters. To unpack, you need the scaling and the number of values.
Each value is restored to within one step `2^E * 10^-D`. Floats give the same data as their values
converted to double.

- A constant field has `bitsPerValue == 0` and no packed data.
- An empty `sp::Scaling{}` (`bitsPerValue == -1`) is an error.

With your own buffers, and the range computed only once:

```cpp
sp::Range<double> r = sp::range(values.data(), values.size());
if (std::isnan(r.min)) { /* a value is NaN or infinite: it cannot be packed */ }
sp::Scaling scaling = sp::scaling(r.min, r.max, sp::Method::bestPrecision(24));
std::vector<uint8_t> buffer(sp::packedSize(values.size(), scaling));
sp::pack(values.data(), values.size(), scaling, buffer.data(), buffer.size());
sp::unpack(buffer.data(), buffer.size(), scaling, back.data(), back.size());
```

For GRIB edition 1, use an IBM reference value and the GRIBEX rules:
`sp::Method::bits(16).referenceFormat(sp::ReferenceFormat::IBM32).gribex()`. The setters return a
modified copy.

Everything in `simple_packing/detail` (namespace `sp::detail`) is internal.

## Build and test

```sh
cmake -S projects/simple_packing -B build -G Ninja
cmake --build build
ctest --test-dir build
```

CMake targets: `simple_packing::simple_packing` and `simple_packing::headers`. Options:
`SIMPLE_PACKING_BUILD_TESTS`, `SIMPLE_PACKING_BUILD_FUZZERS` (Clang), `SIMPLE_PACKING_INSTALL`,
`SIMPLE_PACKING_NATIVE_ARCH` (`-march=native`, default on), and the floating-point options below.

## Floating-point compiler options

simple_packing must round floating-point values in exactly the same way on every compiler and CPU,
and as legacy ecCodes. It must also detect NaN and infinite values. Some compiler options break
this. simple_packing therefore compiles with three options:

| Option | CMake variable | Why |
|---|---|---|
| `-ffp-contract=off` | `SIMPLE_PACKING_FP_CONTRACT_OFF` | Correct results: no fused multiply-add, which rounds differently |
| `-fno-fast-math` | `SIMPLE_PACKING_NO_FAST_MATH` | Correct results: keeps the checks for NaN and infinity (icx/icpx use fast math by default) |
| `-fno-trapping-math` | `SIMPLE_PACKING_NO_TRAPPING_MATH` | Speed: lets GCC vectorise the range checks |

Each variable can be:

- `ON` (default): use the option with every compiler that accepts it. Where the option is not
  needed, it changes nothing and costs nothing.
- `AUTO`: use the option only with the compilers that need it.
- `OFF`: never use it.

Your own code that includes the headers gets `-ffp-contract=off` and `-fno-fast-math` through the
CMake targets, also when you compile with `-ffast-math`. If a test fails because an option is
missing, it says which one.

## In ecCodes

- `ECCODES_USE_SP=1` selects simple_packing for GRIB simple packing.
- CCSDS packing with `ECCODES_USE_SPAEC=1` uses its methods to choose the scaling.
- `codes_set_float_array` packs floats directly (GRIB2 simple packing), with the same result as
  `codes_set_double_array`.

## References

1. WMO. *Manual on Codes, Volume I.2: International Codes, Part B – Binary Codes* (WMO-No. 306).
   Geneva: World Meteorological Organization. GRIB edition 2: data representation template 5.0
   (simple packing); GRIB edition 1: binary data section.
