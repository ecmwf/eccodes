<!--
SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
SPDX-License-Identifier: Apache-2.0
-->

# simple_packing

C++17 library for GRIB simple packing: the choice of the packing parameters and the packing of
values into `bitsPerValue`-bit integers, `X = (Y * 10^D - R) / 2^E`.

## Features

- Packing of 1 to 63 and unpacking of 1 to 64 bits per value, for `float` and `double`
- Optimisers for the packing parameters, compatible with ecCodes:
  - `DecOptimizer`: bits per value given (`optimizeScaleFactor=0`)
  - `BinDecOptimizer`: best use of the bits (`optimizeScaleFactor=1`)
  - `BPVOptimizer`: decimal precision given
  - `BinOptimizer`: decimal scale factor and bits per value given (CCSDS packing)
- Reference values as IEEE or IBM 32-bit floats (GRIB2, GRIB1)
- Packed data byte-identical to legacy ecCodes, invalid input reported as exceptions

## Usage

```cpp
#include "simple_packing/simple/Binary.h"
#include "simple_packing/optimizer/DecOptimizer.h"

sp::SimplePackingBinary<double> packer(std::make_unique<sp::DecOptimizer>(16, 127));
auto [params, buffer] = packer.pack(values);
std::vector<double> restored = packer.unpack(params, buffer, values.size());
```

`packTo`/`unpackTo` work on caller-provided buffers.

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
