# eccodes-sys

Raw FFI bindings to ECMWF's [eccodes](https://github.com/ecmwf/eccodes) C
library.

The bindings are generated with [bindgen](https://rust-lang.github.io/rust-bindgen/)
against a curated allowlist of the `codes_*` API. For a safe API, use the
higher-level [`eccodes`](https://crates.io/crates/eccodes) crate.

## Installation

```toml
[dependencies]
eccodes-sys = "2.49"
```

The crate version tracks the eccodes release it builds.

## Cargo build features

These flags control what the underlying C eccodes library is compiled with.
Defaults mirror the eccodes CMake defaults, except that `netcdf` and `memfs`
are off — they need external libraries or extra setup.

### Build strategy (mutually exclusive)

- `vendored` (default) - Clone and build eccodes, ecbuild and (with `aec`)
  libaec from source. The eccodes tag cloned is the crate version; inside a
  checkout of the eccodes repository the in-tree sources are built instead,
  so branch changes take effect without a network round-trip.
- `system` - Link against a system-installed eccodes, located via CMake
  `find_package(eccodes)` (2.20.0 or newer). Honours `ECCODES_DIR` and
  `CMAKE_PREFIX_PATH`.

Either way, eckit comes from [`eckit-sys`](https://crates.io/crates/eckit-sys),
which follows the same strategy.

### Format support (on by default)

- `product-grib` - GRIB encoding and decoding.
- `product-bufr` - BUFR encoding and decoding.

### Geospatial

- `geography` (default) - Geoiterator and nearest-neighbour support.
- `eckit-geo` - Use the `eckit::geo` Geoiterator backend (implies
  `geography`, and eckit's `eckit-geo`, `eckit-codec`, `eckit-spec`).

### Compression codecs

- `jpg` (default) - JPEG 2000 packing.
- `aec` - Adaptive Entropy Coding; builds libaec from source.
- `png` - PNG packing.

### Optional

- `netcdf` - NetCDF support; requires a system NetCDF.
- `memfs` - Bake `definitions/` and `samples/` into `libeccodes` rather than
  copying them next to the build output.

### Threading (mutually exclusive, off by default)

- `eccodes-threads` - POSIX-threads locking inside `libeccodes`.
- `eccodes-omp-threads` - OpenMP locking inside `libeccodes`.

## Environment variables

- `ECCODES_DIR` - Install prefix of an eccodes build, used by `system` mode.
- `CMAKE_PREFIX_PATH` - Additional CMake search paths.
- `DOCS_RS` - When set, the native build is skipped and the bindings are
  generated from the headers under `docs-headers/` (for docs.rs).

## Runtime resources

Unless built with `memfs`, `libeccodes` reads its `definitions/` and
`samples/` from disk. The build copies both next to the build output, in
`eccodes_resources/`; a leaf binary resolves them through
`bindman_utils::emit_rpaths()` and the `codes_context_set_*_path` calls
wrapped by `eccodes::Library`.

## Copyright and License

Copyright 1996- European Centre for Medium-Range Weather Forecasts (ECMWF).

This software is licensed under the terms of the [Apache License, Version 2.0](LICENSE) which can also be obtained at http://www.apache.org/licenses/LICENSE-2.0.

In applying this licence, ECMWF does not waive the privileges and immunities granted to it by virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
