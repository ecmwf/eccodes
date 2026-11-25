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
- Errors as exceptions: invalid options and samples or values that cannot be coded
  (`std::invalid_argument`), output buffer too small (`std::length_error`), data that cannot be
  decoded (`spaec::DataError`); never reads or writes beyond the buffers

Not supported: RSI padding, packets and the file header of the standard, application-specific
predictors. The compressed data have no checksum: corrupt data are not always detected.

## Usage

```cpp
#include "spaec/spaec.h"

spaec::Options options;  // 16 bits per sample, blocks of 32, reference interval 128
options.bitsPerSample = 12;

std::vector<uint16_t> samples{ 100, 101, 103, 4095, 0 };  // each below 2^12
std::vector<uint8_t> data = spaec::compress(samples, options);
std::vector<uint16_t> back = spaec::decompress<uint16_t>(data, samples.size(), options);
```

The compressed data hold no parameters: `decompress` needs the options of `compress` and the
number of samples.

| `spaec::Options` | Default | Values |
|---|---|---|
| `bitsPerSample` | 16 | 1 to 32, at most the bits of the sample type (8 for `uint8_t`) |
| `blockSize` | 32 | 8, 16, 32, 64 samples |
| `referenceInterval` | 128 | 1 to 4096 blocks |
| `codeOptions` | `CodeOptions::Basic` | `Basic`, `Restricted` (1 to 4 bits per sample) |
| `preprocessor` | `Preprocessor::UnitDelay` | `UnitDelay`, `Bypass`, `None` (`None` only for unsigned samples) |
| `signedSamples` | `false` | `true`: samples are two's complement numbers of `bitsPerSample` bits (pass `int8_t`, `int16_t` or `int32_t`) |
| `threads` | 1 | 1 to 255 |

Every sample must fit `bitsPerSample` bits (unsigned: 0 to 2^n − 1, signed: −2^(n−1) to
2^(n−1) − 1). With your own buffers, any output buffer that holds the result is accepted;
`spaec::maxCompressedSize(count, options)` bytes are always enough (it includes a few bytes of
slack):

```cpp
std::vector<uint8_t> buffer(spaec::maxCompressedSize(samples.size(), options));
size_t size = spaec::compress(samples.data(), samples.size(), options, buffer.data(), buffer.size());
spaec::decompress(buffer.data(), size, options, back.data(), back.size());
```

Floating-point values are scaled to samples and compressed in one pass, as GRIB CCSDS packing
does (`value * 10^D = R + sample * 2^E`, `E` and `D` between −32767 and 32767, `R` finite). The
samples are rounded: a value is restored within half a step `2^E * 10^-D` (exactly if it lies on
the grid). A value outside the range of the samples is an error:

```cpp
std::vector<double> values{ 250.0, 251.5, 252.25 };
spaec::Scaling scaling{ 250.0, -2, 0 };  // R, E, D: steps of 0.25 from 250
std::vector<uint8_t> data = spaec::compress(values, scaling, options);
std::vector<double> restored = spaec::decompress<double>(data, values.size(), scaling, options);
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
