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
- Two selectors of the split-sample parameter k: `Search` (the smallest data, as libaec; the default)
  and `Estimate` (k from the sum of the block, Kiely 2004: encoding 15 to 33 % faster, data 0.05 %
  larger)
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
| `signedSamples` | `false` | `true`: samples are two's complement numbers of `bitsPerSample` bits (pass `int8_t`, `int16_t` or `int32_t`; an unsigned type is read as the two's complement bit pattern) |
| `threads` | 1 | 1 to 255 |
| `splitSelector` | `SplitSelector::Search` | `Search`, `Estimate` (see below; `compress` only) |

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

## Split-sample selectors

For each block the encoder chooses between the zero-block, second-extension, split-sample
(parameter k: the k low bits of each sample are sent as they are, the rest as a fundamental
sequence; k = 0 is the fundamental sequence option) and no-compression options (CCSDS 121.0-B-3,
3). Both selectors compute the exact size of the split-sample option with their k, so the choice
between the options, and `maxCompressedSize`, are exact for both. Both write standard CCSDS
121.0-B-3 data: `decompress` (and any other decoder, e.g. libaec) reads them without knowing the
selector.

- **`Search`** (default): the k of minimum size. The size of a block as a function of k is
  convex; one pass over the block computes the sum and the sizes for the k of the previous block
  and its neighbours (AVX2 for 16-bit samples), further passes follow only while the size
  decreases (CCSDS 120.0-G-4, 5.5). With two k of equal size the result depends on the previous
  block, as in libaec: the data are byte-identical to those of libaec.
- **`Estimate`**: k from the mean µ = S / n of the n samples of the block (A. Kiely, *Selecting the
  Golomb Parameter in Rice Coding*, IPN Progress Report 42-159, 2004, IV.A): the largest k with
  2^k ≤ µ + f*, f* = 2 − φ ≈ 49/128 (φ: golden ratio), in integers without divisions:
  n · 128 · 2^k ≤ 128 · S + 49 · n. Kiely proves that this k is at most one away from the k of minimum size and that the block costs at most 1/2 bit per sample (and about 13 %) more than with the best k, for any distribution of the samples (IV.C); for geometrically distributed samples the exact worst case is 0.04 bit per sample (0.6 %), and f* gives the same largest redundancy against the entropy (about 4.2 %) as the best k (IV.B). Two passes per block (the sum, then the size with this k), vectorised for
  samples of 8, 16 and 32 bits with AVX2 and with SSE2 (`_mm_sad_epu8` for 8-bit samples, 16-bit
  lanes and `_mm_madd_epi16` where they cannot overflow, 32-bit lanes for samples of up to about
  29 bits). No state across blocks: blocks are independent (consecutive blocks are processed in
  parallel by the CPU; a guess of k from the previous block was 3 times slower), and the
  multi-threaded encoder needs no fix-up of the k at chunk starts.

Also evaluated, from F. Solano Donado, *On the Optimal Calculation of the Rice Coding Parameter*,
Algorithms 13 (2020) 181: the estimate ⌊log2(ln 2 · µ)⌋ (⌈…⌉) costs 1.3 % (1.0 %) more on the
benchmark fields, against 0.05 % for Kiely's rule (f = 0, f = 1/2 and Kiely's table for geometric
sources give the same 0.05 %); its search from the estimate to the minimum gives the data of
`Search` without being faster. Neither is offered.

```cpp
spaec::Options options;
options.splitSelector = spaec::SplitSelector::Estimate;  // faster encoding, slightly larger data
std::vector<uint8_t> data = spaec::compress(samples, options);
std::vector<uint16_t> back = spaec::decompress<uint16_t>(data, samples.size(), options);
```

The implementation is in `spaec/detail/encoder/EncoderBase.h` (`selectSearched`,
`selectEstimated`) and `spaec/detail/encoder/BlockSums.h` (the sums of `Estimate`). The selector is a
template parameter of `EncoderSeq` and `EncoderPar` (`selectBestMethod` chooses at compile time); the
only run-time choice is made once per `compress` call. `Search` executes the same instructions as
before the second selector (Clang, icx; GCC within 0.2 %).

## Build and test

```sh
cmake -S projects/spaec -B build -G Ninja
cmake --build build
ctest --test-dir build        # labels: unit, regression
```

CMake target `spaec::spaec`. Options: `SPAEC_BUILD_TESTS`, `SPAEC_BUILD_FUZZERS` (Clang),
`SPAEC_INSTALL`, `SPAEC_NATIVE_ARCH` (`-march=native`, default on); `SPAEC_FP_CONTRACT_OFF`, `SPAEC_NO_FAST_MATH`,
`SPAEC_NO_TRAPPING_MATH` (see below).

## Floating-point compiler options

SpAec maps floating-point values to integer samples (GRIB CCSDS packing): the rounding must be
exactly as written (bit-identical results on every compiler and CPU), and the checks for NaN and
infinite values must hold. The integer coder does no floating-point arithmetic: the options do not
affect it. The three options below make the compiler keep that arithmetic exactly as written. Each
is a CMake cache variable with the values `ON` (the default: added for every compiler that accepts
it), `AUTO` (only for the compilers that need it) and `OFF`. Where an option is not needed it
restates the compiler's default and changes nothing (no measurable cost), so `ON` gives the same
code as `AUTO` there, without depending on its probes and compiler lists. The configure step prints
the options (`spaec: floating-point options: ...`).

| Option | CMake variable | Needed for | `AUTO` enables it for | Measured impact |
|---|---|---|---|---|
| `-ffp-contract=off` | `SPAEC_FP_CONTRACT_OFF` | correct results | compilers that fuse multiply-adds with the project flags (probe at configure time) | none (±3 %) |
| `-fno-fast-math` | `SPAEC_NO_FAST_MATH` | correct results | icx/icpx and icc | none (±3 %) |
| `-fno-trapping-math` | `SPAEC_NO_TRAPPING_MATH` | speed only | GCC | GCC: +12 to +21 % encoding of doubles, else none |

- **`-ffp-contract=off`**: compilers fuse a multiplication and an addition into one multiply-add
  instruction, which rounds once instead of twice, wherever the CPU has one. The scaled values `(x ·
  10^D − R) · 2^−E` then differ in the last bit, and with them the coded integers: the results would
  depend on the compiler and the CPU. GCC (default `-ffp-contract=fast`), Clang ≥ 14 (default `on`)
  and icx/icpx (models `fast` and `precise`) all fuse by default on CPUs with fused multiply-add:
  x86-64 with `-march=native` (Haswell and later) or `-march=x86-64-v3`, and every AArch64, POWER
  and s390x target. `AUTO` compiles a probe with the project flags and enables the option if it
  fuses (when cross-compiling without an emulator: for GCC, Clang and IntelLLVM). Plain x86-64
  (SSE2) has no fused multiply-add: no option is needed. Tests that fail without it: `value_mapping`, `encode_fp_contraction`, `encode_fp_contraction_consumer` and `fp_environment`.
- **`-fno-fast-math`**: `-ffast-math` lets the compiler assume that there are no NaN and infinite
  values (the checks for them are removed: invalid values are coded) and reorder arithmetic. GCC and
  Clang do not use it by default; icx/icpx and icc do (their default floating-point model `fast`:
  1736 of 7103 cases of the compiler tests of these projects changed; of the spaec tests only
  `fp_environment` notices it, simple packing has 6 failing tests). `AUTO` enables `-fno-fast-math`
  for them. A `-ffast-math` in your own `CMAKE_CXX_FLAGS` is undone by the default `ON`
  (`-fno-fast-math` comes after your flags); with `AUTO` (GCC, Clang) or `OFF` it is not: configure
  warns, and the tests fail (`api`, `spaec_api`, `errors`, `parameter_validation`,
  `encode_value_range`, `consumer_flags` and `fp_environment`). The option is also a link option: linking with `-ffast-math` (GCC and Clang link
  `crtfastmath.o`) or with icx/icpx sets the CPU to flush subnormal numbers to zero at program
  start, which `-fno-fast-math` at compile time does not undo.
- **`-fno-trapping-math`**: tells the compiler that the code does not use floating-point exception
  flags (it does not). GCC's default `-ftrapping-math` keeps it from vectorising the range checks of
  the values; Clang and icx/icpx do not assume trapping math anyway. No effect on the results.

Measured on an AMD Zen 3 (GCC 16, Clang 22, icx 2026.2; `-O3` with `-march=native` and with
`-march=x86-64`; the seven benchmark fields, cold caches; each option alone against none). On these
fields the results were bit-identical in all 30 configurations (3 compilers, 2 architectures, 5 sets of options: none, each alone, all three): the differences need values at a
rounding boundary, which the tests construct.

**When a test fails** because an option is missing, it says which one and how to enable it, e.g.
"The compiler fuses multiplications and additions ... Configure with -DSPAEC_FP_CONTRACT_OFF=ON".
The test `spaec_fp_environment` checks the options of the test build alone.

**Your own code** that includes the headers gets the options needed for correct results through the
usage requirements of the CMake targets: with the default `ON`, `-fno-fast-math` and
`-ffp-contract=off` after your target's own options (and `-fno-fast-math` as a link option), also
when you compile with other flags than the project (`-ffast-math`, or for a CPU with fused
multiply-add). With `AUTO` they hold what the project's compiler and flags needed: with other flags
add `-ffp-contract=off` yourself, and `-fno-fast-math` after a `-ffast-math` (also to the link
flags). With `OFF` your code gets none of them. Translation units of one program compiled for
different instruction sets (e.g. one with `-mavx2`) use separate instances of the functions: they
are in an inline namespace per instruction set (`spaec/detail/Isa.h`).

## In ecCodes

GRIB CCSDS packing uses SpAec with `ECCODES_USE_SPAEC=1`, and always when ecCodes is built without
libaec. Signed samples and RSI padding are left to libaec. `codes_set_float_array` packs the floats
without converting them to doubles, with the result of `codes_set_double_array` with the converted
values. `ECCODES_SPAEC_SELECTOR` selects the split-sample selector: `search` (the default; the data
of libaec) or `estimate`.
