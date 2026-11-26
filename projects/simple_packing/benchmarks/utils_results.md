<!--
SPDX-FileCopyrightText: 2026 European Centre for Medium-Range Weather Forecasts (ECMWF)
SPDX-License-Identifier: Apache-2.0
-->

# Benchmark results: src/Utils.cc

`utils_bench` (nanoseconds per call) before and after src/Utils.cc was modernised, optimised and
cleaned up. The results of the functions are unchanged: `tests/unit/utils_reference.cc` compares
them with a frozen copy of the earlier implementation (4.7 million cases).

- AMD Ryzen Threadripper PRO 5995WX (Zen 3), one thread; GCC 16.2.1, Clang 22.1.8, Intel icx
  2026.2 (`-fp-model=precise`); Release builds with `-O3 -march=native`
- Fixed inputs; the fastest of 7 runs, best of 3 program runs (warm caches)
- Instructions: callgrind, GCC build, per 100,000 calls (10,000 for tiny ranges)

## Before and after (nanoseconds per call)

| Function | GCC before | GCC after | Clang before | Clang after | Intel before | Intel after |
|---|---|---|---|---|---|---|
| `nearest_smaller_value`, IEEE | 49.5 | 8.2 (6.0×) | 49.2 | 6.8 (7.2×) | 48.4 | 6.8 (7.1×) |
| `nearest_smaller_value`, IBM | 45.7 | 8.1 (5.6×) | 47.7 | 6.8 (7.0×) | 46.7 | 6.6 (7.1×) |
| `number_of_bits` | 15.9 | 0.5 (31.8×) | 7.8 | 0.5 (15.6×) | 11.1 | 0.5 (22.2×) |
| `get_binary_scale_factor`, typical ranges | 45.6 | 12.2 (3.7×) | 48.3 | 7.4 (6.5×) | 45.9 | 7.2 (6.4×) |
| `get_binary_scale_factor`, tiny ranges | 1041.1 | 5.5 (189.3×) | 1015.5 | 4.3 (236.2×) | 1013.2 | 2.8 (361.9×) |
| `check_data_values_range` | 2 | 2 (1.0×) | 2 | 2 (1.0×) | 1.7 | 1.6 (1.1×) |
| `sp::scaling`, `Method::bits` | 230.3 | 181.5 (1.3×) | 268.5 | 199.8 (1.3×) | 231.6 | 180.4 (1.3×) |
| `sp::scaling`, `Method::bestPrecision` | 1700.7 | 1621.5 (1.0×) | 1768.1 | 1695.9 (1.0×) | 1791.1 | 1719.6 (1.0×) |
| `sp::scaling`, `Method::decimals` | 119.3 | 24.3 (4.9×) | 106.5 | 33.7 (3.2×) | 101.2 | 26.4 (3.8×) |
| `sp::scaling`, `Method::bitsAndDecimals` | 108.8 | 35.8 (3.0×) | 120.9 | 42.2 (2.9×) | 112.6 | 35.2 (3.2×) |

The four `sp::scaling` cases call these functions; `bestPrecision` spends its time in
`BinDecOptimizer.cc`, which was not changed.

## Each step (GCC instructions, millions)

No step made a case slower (with any compiler): the modernised code kept the algorithms and their
speed, the optimised code computes the results directly, the cleaned up code removed checks that
can no longer fail.

| Function | Before | Modernised | Optimised | Cleaned up |
|---|---|---|---|---|
| `nearest_smaller_value`, IEEE | 23.7 | 22.9 | 4.2 | 4.2 |
| `nearest_smaller_value`, IBM | 25.5 | 25.1 | 6.0 | 6.0 |
| `number_of_bits` | 5.5 | 5.5 | 1.2 | 1.2 |
| `get_binary_scale_factor`, typical ranges | 26.0 | 25.6 | 14.2 | 13.6 |
| `get_binary_scale_factor`, tiny ranges | 72.8 | 72.8 | 0.7 | 0.7 |
| `check_data_values_range` | 3.1 | 3.1 | 3.1 | 3.1 |
| `sp::scaling`, `Method::bits` | 130.2 | 129.0 | 98.9 | 98.3 |
| `sp::scaling`, `Method::bestPrecision` | 1011.2 | 1010.1 | 982.9 | 982.8 |
| `sp::scaling`, `Method::decimals` | 57.0 | 56.2 | 29.7 | 29.7 |
| `sp::scaling`, `Method::bitsAndDecimals` | 69.4 | 68.2 | 41.5 | 40.9 |

## What changed

- **Modernised**: constexpr tables instead of tables filled on first use, `std::array`, 32-bit
  unsigned integers for the bits of the 32-bit floats, named constants.
- **Optimised**:
  - IEEE: the largest float not above the value from `static_cast<float>` and one step down if
    needed, instead of a table search and three conversions.
  - IBM: the exponent from the bits of the double, the mantissa rounded down (up for negative
    values) directly; the legacy result for a mantissa of exactly 0xffffff.5 is kept.
  - `get_binary_scale_factor`: the search starts next to the result (from the exponent of the
    range) instead of at 0, and no value at once for ranges that are too small.
  - `number_of_bits`: the bit width, inline in the header.
- **Cleaned up**: no overflow checks that cannot fail any more, clearer names.
