<!--
SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
SPDX-License-Identifier: Apache-2.0
-->

# spaec

Header-only C++17 implementation of CCSDS 121.0-B-3 lossless data compression (Adaptive Entropy
Coding), the coder of GRIB2 CCSDS packing.

## Features

- One header, `spaec/spaec.h`: `spaec::compress` and `spaec::decompress` with a `spaec::Options` struct
- Integer samples of 1 to 32 bits (unsigned or signed), and floating-point values with a GRIB
  scaling (reference value, binary and decimal scale factor)
- Block sizes 8, 16, 32, 64; reference sample intervals 1 to 4096; basic and restricted code options
- Preprocessors: unit-delay predictor, bypass predictor or none
- Single-threaded and multi-threaded with identical output
- Compressed data byte-identical to libaec for the options both support
- Errors as exceptions: invalid options (`std::invalid_argument`), output buffer too small
  (`std::length_error`), corrupt or truncated data (`spaec::DataError`); never reads or writes
  beyond the buffers

Not supported: RSI padding, packets and the file header of the standard, application-specific
predictors.

## Usage

```cpp
#include "spaec/spaec.h"

spaec::Options options;      // 16 bits per sample, blocks of 32, reference interval 128
options.bitsPerSample = 12;

std::vector<uint16_t> samples = ...;
std::vector<uint8_t> data = spaec::compress(samples, options);
std::vector<uint16_t> back = spaec::decompress<uint16_t>(data, samples.size(), options);
```

The compressed data hold no parameters: `decompress` needs the options of `compress` and the
number of samples.

| `spaec::Options` | Default | Values |
|---|---|---|
| `bitsPerSample` | 16 | 1 to 32 |
| `blockSize` | 32 | 8, 16, 32, 64 samples |
| `referenceInterval` | 128 | 1 to 4096 blocks |
| `codeOptions` | `CodeOptions::Basic` | `Basic`, `Restricted` (1 to 4 bits per sample) |
| `preprocessor` | `Preprocessor::UnitDelay` | `UnitDelay`, `Bypass`, `None` |
| `signedSamples` | `false` | `true`: pass `int8_t`, `int16_t` or `int32_t` samples |
| `threads` | 1 | 1 to 255 |

With your own buffers (any output buffer that holds the result is accepted;
`spaec::maxCompressedSize(count, options)` bytes are always enough):

```cpp
size_t size = spaec::compress(samples, count, options, output, capacity);
spaec::decompress(data, size, options, samples, count);
```

Floating-point values are scaled to samples and compressed in one pass, as GRIB CCSDS packing
does (`value * 10^D = R + sample * 2^E`):

```cpp
spaec::Scaling scaling{ 250.0, -4, 0 };  // R, E, D
std::vector<uint8_t> data = spaec::compress(values, scaling, options);
std::vector<double> back = spaec::decompress<double>(data, values.size(), scaling, options);
```

The implementation is in `spaec/detail` (namespace `spaec::detail`) and is not part of the interface.

## Build and test

```sh
cmake -S projects/spaec -B build -G Ninja
cmake --build build
ctest --test-dir build        # labels: unit, regression
```

CMake target `spaec::spaec`. Options: `SPAEC_BUILD_TESTS`, `SPAEC_BUILD_FUZZERS` (Clang),
`SPAEC_INSTALL`, `SPAEC_NATIVE_ARCH` (`-march=native`, default on).

## In ecCodes

GRIB CCSDS packing uses SpAec with `ECCODES_USE_SPAEC=1`, and always when ecCodes is built without
libaec. Signed samples and RSI padding are left to libaec.
