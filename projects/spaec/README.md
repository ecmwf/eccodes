<!--
SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
SPDX-License-Identifier: Apache-2.0
-->

# spaec

SpAec is a header-only C++17 library for lossless data compression with the CCSDS 121.0-B-3
algorithm [1]. GRIB2 uses this algorithm for CCSDS packing. SpAec writes the same data as
libaec [5].

Version 0.0.1 (`spaec::version()`, `SPAEC_VERSION` in `spaec/version.h`). Before version 1.0.0 the
interface may still change.

## Features

- One header, `spaec/spaec.h`, with `spaec::compress`, `spaec::decompress` and `spaec::Options`.
- Integer samples of 1 to 32 bits, unsigned or signed.
- Floating-point values with a GRIB scaling (reference value, binary and decimal scale factor).
- Block sizes 8, 16, 32 and 64; reference sample intervals 1 to 4096; basic and restricted code
  options.
- Preprocessor: unit-delay predictor, bypass predictor or none.
- One or more threads, with the same output.
- Two ways to choose the split-sample parameter k: `Search` (default) and `Estimate` (faster).
- Errors are exceptions. SpAec never reads or writes outside the buffers.

Not supported: RSI padding, packets, the file header of the standard and application-specific
predictors. The compressed data have no checksum, so corrupt data are not always detected.

## Usage

```cpp
#include "spaec/spaec.h"

spaec::Options options;   // 16 bits per sample, blocks of 32, reference interval 128
options.bitsPerSample = 12;

std::vector<uint16_t> samples{ 100, 101, 103, 4095, 0 };   // each below 2^12
std::vector<uint8_t> data = spaec::compress(samples, options);
std::vector<uint16_t> back = spaec::decompress<uint16_t>(data, samples.size(), options);
```

The compressed data contain no parameters. To decompress, you need the same options and the number
of samples.

| Option | Default | Values |
|---|---|---|
| `bitsPerSample` | 16 | 1 to 32 (at most the size of the sample type) |
| `blockSize` | 32 | 8, 16, 32, 64 |
| `referenceInterval` | 128 | 1 to 4096 blocks |
| `codeOptions` | `Basic` | `Basic`, `Restricted` (1 to 4 bits per sample) |
| `preprocessor` | `UnitDelay` | `UnitDelay`, `Bypass`, `None` (`None` only for unsigned samples) |
| `signedSamples` | `false` | `true`: two's complement samples (`int8_t`, `int16_t`, `int32_t`) |
| `threads` | 1 | 1 to 255 |
| `splitSelector` | `Search` | `Search`, `Estimate` |

Every sample must fit into `bitsPerSample` bits. You can also use your own buffers.
`spaec::maxCompressedSize` gives a size that is always large enough:

```cpp
std::vector<uint8_t> buffer(spaec::maxCompressedSize(samples.size(), options));
size_t size = spaec::compress(samples.data(), samples.size(), options, buffer.data(), buffer.size());
spaec::decompress(buffer.data(), size, options, back.data(), back.size());
```

Floating-point values are scaled to integer samples and compressed in one step, as in GRIB:
`value * 10^D = R + sample * 2^E`. Each value is restored to within half a step `2^E * 10^-D`.
Floats give the same data as their values converted to double.

```cpp
std::vector<double> values{ 250.0, 251.5, 252.25 };
spaec::Scaling scaling{ 250.0, -2, 0 };   // R = 250, E = -2, D = 0: steps of 0.25
std::vector<uint8_t> data = spaec::compress(values, scaling, options);
std::vector<double> restored = spaec::decompress<double>(data, values.size(), scaling, options);
```

Errors: invalid options, samples or values throw `std::invalid_argument`; an output buffer that is
too small throws `std::length_error`; data that cannot be decoded throw `spaec::DataError`.

Everything in `spaec/detail` (namespace `spaec::detail`) is internal.

## Split-sample selectors

For each block of samples, the encoder chooses the code option with the fewest bits. One of these
options is the split-sample option with a parameter k: the k low bits of each sample are written
as they are, the rest with a unary code [1, sections 3.3 and 3.7]. The two selectors differ only in
how they find k. Both write standard data, and every decoder reads them.

- **`Search`** (default) finds the k with the smallest size. The size of a block is a convex
  function of k [2, section 5.5], so the search can stop as soon as the size grows again. The data
  are byte-identical to those of libaec.
- **`Estimate`** computes k from the sum S of the n samples of the block, with the rule of
  Kiely [3, section IV.A]: k is the largest value with `n * 128 * 2^k <= 128 * S + 49 * n`. This
  needs only integer arithmetic. Kiely shows that this k is at most one away from the best k, and
  that a block costs at most 1/2 bit per sample more [3, section IV.C]. In practice the data are
  about 0.07 % larger, and encoding is faster (see "Performance").

```cpp
options.splitSelector = spaec::SplitSelector::Estimate;
```

We also tried the estimates of Solano Donado [4]. On our test fields they gave about 1 % larger
data, so SpAec does not offer them.

## Build and test

```sh
cmake -S projects/spaec -B build -G Ninja
cmake --build build
ctest --test-dir build
```

Tested on Linux (GCC, Clang and Intel icx; x86-64, AArch64 and s390x) and on Windows with MinGW-w64
GCC (tests run under wine). The code also supports MSVC, but MSVC has not been tested yet. All
integers have fixed widths (`int32_t`, `uint32_t`, `int64_t`, ...), so they are the same on every
platform.

CMake target: `spaec::spaec`. Options: `SPAEC_BUILD_TESTS`, `SPAEC_BUILD_FUZZERS` (Clang),
`SPAEC_INSTALL`, `SPAEC_NATIVE_ARCH` (`-march=native`, default on), and the floating-point options
below.

`-DSPAEC_BUILD_BENCHMARKS=ON` builds `benchmarks/spaec_bench`, an overview of the speed of
compression and decompression (both selectors, several threads; with libaec for comparison if it is
found). It uses synthetic fields, or your own samples (see `spaec_bench.cc`).

## Floating-point compiler options

SpAec must round floating-point values in exactly the same way on every compiler and CPU, and it
must detect NaN and infinite values. Some compiler options break this. SpAec therefore compiles
with three options:

| Option | CMake variable | Why |
|---|---|---|
| `-ffp-contract=off` | `SPAEC_FP_CONTRACT_OFF` | Correct results: no fused multiply-add, which rounds differently |
| `-fno-fast-math` | `SPAEC_NO_FAST_MATH` | Correct results: keeps the checks for NaN and infinity (icx/icpx use fast math by default) |
| `-fno-trapping-math` | `SPAEC_NO_TRAPPING_MATH` | Speed: lets GCC vectorise the range checks |

Each variable can be:

- `ON` (default): use the option with every compiler that accepts it. Where the option is not
  needed, it changes nothing and costs nothing.
- `AUTO`: use the option only with the compilers that need it.
- `OFF`: never use it.

Your own code that includes the headers gets `-ffp-contract=off` and `-fno-fast-math` through the
CMake target `spaec::spaec`, also when you compile with `-ffast-math`. If a test fails because an
option is missing, it says which one.

## Performance

Legacy CCSDS packing in ecCodes (libaec) against SpAec, through the ecCodes API. The numbers give an
overview; they are not exact.

- Hardware: AMD Ryzen Threadripper PRO 5995WX (Zen 3, AVX2), one thread.
- Compilers: GCC 16.2.1, Clang 22.1.8 and Intel icx 2026.2 (`-fp-model=precise`). ecCodes and
  libaec 1.1.8 are Release builds with `-O3 -march=native`.
- Data: 191 GRIB2 messages without missing values, 206 million values in total. 187 fields are ECMWF
  open data (IFS, 0.25°, forecast of 26 November 2026, 00 UTC, step 24 h); 4 fields come from the
  ecCodes test data. They have 7 to 24 bits per value (most have 9 to 12). CCSDS settings as in the
  ECMWF data: block size 32, reference sample interval 128, preprocessor.
- Method: all messages in one file, held in memory. Each call runs over all messages three times;
  the fastest run counts.
- Program: `tests/benchmarks/packing_api.cc` of ecCodes. `tests/benchmarks/packing_api.py
  --program <build>/tests/packing_api --ccsds <file> --simple <file>` prints these tables.
- MiB/s counts the array passed to ecCodes: 8 bytes per double, 4 per float.
- Legacy: `ECCODES_USE_SPAEC=0`. New: `ECCODES_USE_SPAEC=1` with `ECCODES_SPAEC_SELECTOR=search`
  (default) or `estimate`. The selector only affects encoding.

| Call | Compiler | Legacy MiB/s | Legacy Mvalues/s | Search MiB/s | Search Mvalues/s | Speed-up | Estimate MiB/s | Estimate Mvalues/s | Speed-up |
|---|---|---|---|---|---|---|---|---|---|
| `codes_get_double_array` | GCC | 1,336 | 175 | 3,282 | 430 | 2.5× | — | — | — |
| `codes_get_double_array` | Clang | 1,393 | 183 | 4,101 | 537 | 2.9× | — | — | — |
| `codes_get_double_array` | Intel | 1,253 | 164 | 3,975 | 521 | 3.2× | — | — | — |
| `codes_get_float_array` | GCC | 662 | 174 | 1,607 | 421 | 2.4× | — | — | — |
| `codes_get_float_array` | Clang | 700 | 183 | 2,009 | 527 | 2.9× | — | — | — |
| `codes_get_float_array` | Intel | 632 | 166 | 2,024 | 531 | 3.2× | — | — | — |
| `codes_set_double_array` | GCC | 1,661 | 218 | 2,728 | 358 | 1.6× | 3,126 | 410 | 1.9× |
| `codes_set_double_array` | Clang | 1,824 | 239 | 2,936 | 385 | 1.6× | 3,357 | 440 | 1.8× |
| `codes_set_double_array` | Intel | 1,861 | 244 | 3,058 | 401 | 1.6× | 3,236 | 424 | 1.7× |
| `codes_set_float_array` | GCC | 784 | 206 | 1,406 | 369 | 1.8× | 1,594 | 418 | 2.0× |
| `codes_set_float_array` | Clang | 840 | 220 | 1,533 | 402 | 1.8× | 1,779 | 466 | 2.1× |
| `codes_set_float_array` | Intel | 919 | 241 | 1,595 | 418 | 1.7× | 1,760 | 461 | 1.9× |

Legacy and `search` write byte-identical messages. `estimate` writes 0.07 % more bytes, with the same
values, and encodes 6 to 16 % faster than `search`.

## In ecCodes

- `ECCODES_USE_SPAEC=1` selects SpAec for GRIB CCSDS packing. Without libaec, ecCodes always uses
  SpAec. Signed samples and RSI padding are left to libaec.
- `ECCODES_SPAEC_SELECTOR` selects the split-sample selector: `search` (default) or `estimate`.
- `codes_set_float_array` packs floats directly, with the same result as `codes_set_double_array`.

## References

1. CCSDS. *Lossless Data Compression*. Recommendation for Space Data System Standards (Blue Book),
   CCSDS 121.0-B-3, Issue 3. Washington, D.C.: CCSDS, August 2020.
2. CCSDS. *Lossless Data Compression*. Informational Report (Green Book), CCSDS 120.0-G-4, Issue 4.
   Washington, D.C.: CCSDS, November 2021.
3. A. Kiely. "Selecting the Golomb Parameter in Rice Coding." *IPN Progress Report* 42-159. Pasadena,
   California: Jet Propulsion Laboratory, November 15, 2004.
4. F. Solano Donado. "On the Optimal Calculation of the Rice Coding Parameter." *Algorithms* 13, no.
   8 (2020): 181. https://doi.org/10.3390/a13080181
5. libaec, Adaptive Entropy Coding library. Deutsches Klimarechenzentrum (DKRZ).
   https://gitlab.dkrz.de/k202009/libaec
