<!--
SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
SPDX-License-Identifier: Apache-2.0
-->

# simple_packing

C++17 library for GRIB simple packing: the choice of the packing parameters and the packing of
values into `bitsPerValue`-bit integers, `X = (Y * 10^D - R) / 2^E`.

## Features

- One header, `simple_packing/simple_packing.h`: `sp::scaling`, `sp::pack` and `sp::unpack` with a
  `sp::Scaling` struct (reference value, binary and decimal scale factor, bits per value)
- Packing of 1 to 63 and unpacking of 1 to 64 bits per value, for `float` and `double`
- Four methods to choose the scaling, compatible with ecCodes:
  - `sp::Method::bits(n)`: bits per value given (`optimizeScaleFactor=0`)
  - `sp::Method::bestPrecision(n)`: best use of the bits (`optimizeScaleFactor=1`)
  - `sp::Method::decimals(d)`: decimal precision given, the bits follow
  - `sp::Method::bitsAndDecimals(n, d)`: both given (CCSDS packing)
- Reference values as IEEE or IBM 32-bit floats (GRIB2, GRIB1)
- Packed data byte-identical to legacy ecCodes; errors as exceptions (`std::invalid_argument`,
  `std::runtime_error`, `std::length_error`)

## Usage

```cpp
#include "simple_packing/simple_packing.h"

std::vector<double> values{ 273.15, 280.4, 291.07, 265.9 };
sp::Scaling scaling = sp::scaling(values, sp::Method::bits(16));
std::vector<uint8_t> packed = sp::pack(values, scaling);
std::vector<double> back = sp::unpack<double>(packed, values.size(), scaling);
```

The packed data hold no parameters: `unpack` needs the scaling and the number of values. The
values are restored within one step `2^E * 10^-D`. A constant field has `bitsPerValue == 0` and no
packed data; a `Scaling` that is not filled in (`bitsPerValue == -1`) is an error.

With your own buffers, and the range computed once:

```cpp
sp::Range<double> r = sp::range(values.data(), values.size());
if (std::isnan(r.min)) { /* a value is NaN or infinite: cannot be packed */ }
sp::Scaling scaling = sp::scaling(r.min, r.max, sp::Method::bestPrecision(24));
std::vector<uint8_t> buffer(sp::packedSize(values.size(), scaling));
sp::pack(values.data(), values.size(), scaling, buffer.data(), buffer.size());
sp::unpack(buffer.data(), buffer.size(), scaling, back.data(), back.size());
```

For GRIB edition 1: `sp::Method::bits(16).referenceFormat(sp::ReferenceFormat::IBM32).gribex()`
(the setters return a modified copy).

The implementation is in `simple_packing/detail` (namespace `sp::detail`) and is not part of the
interface.

## Build and test

```sh
cmake -S projects/simple_packing -B build -G Ninja
cmake --build build
ctest --test-dir build
```

CMake targets `simple_packing::simple_packing` and `simple_packing::headers`. Options:
`SIMPLE_PACKING_BUILD_TESTS`, `SIMPLE_PACKING_BUILD_FUZZERS` (Clang), `SIMPLE_PACKING_INSTALL`,
`SIMPLE_PACKING_NATIVE_ARCH` (`-march=native`, default on); `SIMPLE_PACKING_FP_CONTRACT_OFF`, `SIMPLE_PACKING_NO_FAST_MATH`,
`SIMPLE_PACKING_NO_TRAPPING_MATH` (see below).

## Floating-point compiler options

Simple packing scales floating-point values to integers and computes the packing parameters: the
rounding must be exactly as written (bit-identical results on every compiler and CPU, and to legacy
ecCodes), and the checks for NaN and infinite values must hold. The three options below make the
compiler keep that arithmetic exactly as written. None of them is enabled by default: each is a
CMake cache variable with the values `AUTO` (the default: enabled only for the compilers that need
it), `ON` and `OFF`. The configure step prints the options it enabled and why (`simple_packing:
floating-point options: ...`).

| Option | CMake variable | Needed for | `AUTO` enables it for | Measured impact |
|---|---|---|---|---|
| `-ffp-contract=off` | `SIMPLE_PACKING_FP_CONTRACT_OFF` | correct results | compilers that fuse multiply-adds with the project flags (probe at configure time) | none (±3 %) |
| `-fno-fast-math` | `SIMPLE_PACKING_NO_FAST_MATH` | correct results | icx/icpx and icc | none (±3 %) |
| `-fno-trapping-math` | `SIMPLE_PACKING_NO_TRAPPING_MATH` | speed only | GCC | GCC: +2 to +5 % packing, else none |

- **`-ffp-contract=off`**: compilers fuse a multiplication and an addition into one multiply-add
  instruction, which rounds once instead of twice, wherever the CPU has one. The scaled values `(x ·
  10^D − R) · 2^−E` then differ in the last bit, and with them the coded integers: the results would
  depend on the compiler and the CPU. GCC (default `-ffp-contract=fast`), Clang ≥ 14 (default `on`)
  and icx/icpx (models `fast` and `precise`) all fuse by default on CPUs with fused multiply-add:
  x86-64 with `-march=native` (Haswell and later) or `-march=x86-64-v3`, and every AArch64, POWER
  and s390x target. `AUTO` compiles a probe with the project flags and enables the option if it
  fuses (when cross-compiling without an emulator: for GCC, Clang and IntelLLVM). Plain x86-64
  (SSE2) has no fused multiply-add: no option is needed. Tests that fail without it: `codec_binary`, `pack_fp_contraction`, `pack_fp_contraction_consumer` and `fp_environment`.
- **`-fno-fast-math`**: `-ffast-math` lets the compiler assume that there are no NaN and infinite
  values (the checks for them are removed: invalid values are coded, scale searches loop forever)
  and reorder arithmetic. GCC and Clang do not use it by default; icx/icpx and icc do (their default
  floating-point model `fast`: 1736 of 7103 cases of the compiler tests of these projects changed,
  and without `-fno-fast-math` 6 tests fail, among them the subnormal ranges of `utils_ranges` and
  `binary_scale_tiny_range`). `AUTO` enables `-fno-fast-math` for them. A `-ffast-math` in your own
  `CMAKE_CXX_FLAGS` is not undone: configure warns, and the tests fail (14 tests, among them `api`,
  `errors` and `field_range`; `binary_scale_large_range` and `consumer_flags`, which would hang,
  check the options first). Set `SIMPLE_PACKING_NO_FAST_MATH=ON` (adds `-fno-fast-math` after your
  flags) or remove `-ffast-math`. The option is also a link option: linking with `-ffast-math` (GCC
  and Clang link `crtfastmath.o`) or with icx/icpx sets the CPU to flush subnormal numbers to zero
  at program start, which `-fno-fast-math` at compile time does not undo.
- **`-fno-trapping-math`**: tells the compiler that the code does not use floating-point exception
  flags (it does not). GCC's default `-ftrapping-math` keeps it from vectorising the range checks of
  the values; Clang and icx/icpx do not assume trapping math anyway. No effect on the results.

Measured on an AMD Zen 3 (GCC 16, Clang 22, icx 2026.2; `-O3` with `-march=native` and with
`-march=x86-64`; the seven benchmark fields, cold caches; each option alone against none). On these
fields the results were bit-identical in all 30 configurations (3 compilers, 2 architectures, 5 sets of options: none, each alone, all three): the differences need values at a
rounding boundary, which the tests construct.

**When a test fails** because an option is missing, it says which one and how to enable it, e.g.
"The compiler fuses multiplications and additions ... Configure with
-DSIMPLE_PACKING_FP_CONTRACT_OFF=ON". The test `simple_packing_fp_environment` checks the options of
the test build alone.

**Your own code** that includes the headers gets the options needed for correct results through the
usage requirements of the CMake targets (as resolved when the project was configured). If you
compile it with other flags than the project, e.g. for a CPU with fused multiply-add when the
project was configured without, add `-ffp-contract=off` yourself. With `-ffast-math` in your own
flags, the header functions (`sp::range`, `sp::scaling`) no longer detect NaN and infinite values
(`std::isfinite` is assumed true): add `-fno-fast-math` after it, also to the link flags.

## In ecCodes

The simple packing accessors use it with `ECCODES_USE_SP=1`; CCSDS packing uses its optimisers with
`ECCODES_USE_SPAEC=1`.
