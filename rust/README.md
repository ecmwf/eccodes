# eccodes Rust bindings

Rust bindings for ECMWF's [eccodes](https://github.com/ecmwf/eccodes) C
library.

## Overview

eccodes decodes and encodes the WMO formats ECMWF distributes its data in —
GRIB editions 1 and 2, BUFR editions 3 and 4. These crates make it usable
from Rust: the C library is built from source (or located on the system) at
build time, and its messages, keys, grids and indexes are exposed through a
safe API.

## Installation

Add the safe wrapper to your `Cargo.toml`:

```toml
[dependencies]
eccodes = "2.49"
```

Building requires CMake and a C compiler. To link against an existing eccodes
installation instead of building from source, use the `system` feature:

```toml
[dependencies]
eccodes = { version = "2.49", default-features = false, features = ["system"] }
```

## Crates

- **eccodes**: the safe API. Message files, typed key access, grids,
  nearest-point search, indexes, and eccodes return codes surfaced as typed
  Rust errors.
- **eccodes-sys**: the low-level FFI layer, generated with bindgen. Builds
  the eccodes C library from source (`vendored`, default) or links against a
  system installation (`system`). Its Cargo features control which parts of
  the C library are compiled.

Both are versioned to track the eccodes release they build, and depend on
[`eckit-sys`](https://crates.io/crates/eckit-sys) for the eckit C++ library
underneath.

## Development

The workspace also holds `examples/eccodes-example`, which is not published:
it exercises the `cargo install` flow and checks that every vendored shared
library resolves at runtime from an installed binary.

Inside a checkout of this repository, a `vendored` build compiles the C
sources in the tree rather than cloning the release tag, so changes to `src/`
take effect without a network round-trip.

## Copyright and License

Copyright 1996- European Centre for Medium-Range Weather Forecasts (ECMWF).

This software is licensed under the terms of the [Apache License, Version 2.0](LICENSE) which can also be obtained at http://www.apache.org/licenses/LICENSE-2.0.

In applying this licence, ECMWF does not waive the privileges and immunities granted to it by virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
