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

std::vector<double> values = ...;
sp::Scaling scaling = sp::scaling(values, sp::Method::bits(16));
std::vector<uint8_t> packed = sp::pack(values, scaling);
std::vector<double> back = sp::unpack<double>(packed, values.size(), scaling);
```

The packed data hold no parameters: `unpack` needs the scaling and the number of values. A
constant field has `bitsPerValue == 0` and no packed data.

With your own buffers, and the range computed once:

```cpp
sp::Range<double> r = sp::range(values, count);  // NaN if a value is NaN or infinite
sp::Scaling scaling = sp::scaling(r.min, r.max, sp::Method::bestPrecision(24));
sp::pack(values, count, scaling, output, sp::packedSize(count, scaling));
sp::unpack(packed, size, scaling, values, count);
```

For GRIB edition 1: `sp::Method::bits(16).referenceFormat(sp::ReferenceFormat::IBM32).gribex()`.

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
`SIMPLE_PACKING_NATIVE_ARCH` (`-march=native`, default on).

## In ecCodes

The simple packing accessors use it with `ECCODES_USE_SP=1`; CCSDS packing uses its optimisers with
`ECCODES_USE_SPAEC=1`.
