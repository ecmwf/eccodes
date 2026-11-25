<!--
SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
SPDX-License-Identifier: Apache-2.0
-->

# spaec

Header-only C++17 implementation of CCSDS 121.0-B-3 lossless data compression (Adaptive Entropy
Coding), the coder of GRIB2 CCSDS packing.

## Features

- Encoder and decoder, sequential (`EncoderSeq`, `DecoderSeq`) and multi-threaded (`EncoderPar`,
  `DecoderPar`) with identical output
- 1 to 32 bits per sample, block sizes 8, 16, 32, 64, reference sample intervals 1 to 4096
- Basic and restricted code option sets
- Preprocessing: unit-delay predictor, bypass predictor or none; unsigned and signed samples
- `SpAec<T>`: floating-point interface with GRIB simple packing parameters (used by ecCodes)
- Streams byte-identical to libaec for the options both support
- Corrupt input is reported as an exception, never read beyond the buffer

Not supported: RSI padding, packets and the file header of the standard, application-specific
predictors.

## Usage

```cpp
#include "spaec/encoder/EncoderSeq.h"
#include "spaec/decoder/DecoderSeq.h"

spaec::encoder::EncoderSeq<uint16_t, uint16_t> enc(16, 32, 128, spaec::CodeOptionSet::Basic);
std::vector<uint8_t> out(spaec::encoder::SpAec<double>::maxEncodedSize(n, 16, 32), 0);
out.resize(enc.encode(spaec::Span<uint16_t>(samples), spaec::Span<uint8_t>(out)));

spaec::DecoderSeq<uint16_t, uint16_t> dec(16, 32, 128, spaec::CodeOptionSet::Basic);
dec.decode(spaec::Span<uint8_t>(out), spaec::Span<uint16_t>(decoded));
```

The output buffer must be zeroed. Options for other preprocessing: last constructor argument
`spaec::Preprocessing{ spaec::Prediction::None }`, `{ spaec::Prediction::UnitDelay, true }` (signed).

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
