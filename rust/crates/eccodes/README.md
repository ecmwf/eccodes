# eccodes

Read and write GRIB and BUFR messages, through ECMWF's
[eccodes](https://github.com/ecmwf/eccodes) C library.

## Overview

This crate provides a safe API on top of the low-level
[`eccodes-sys`](https://crates.io/crates/eccodes-sys) bindings:

- `MessageFile` - a file of messages: count it, index it, or iterate it.
  `GribFile` and `BufrFile` fix the product; bytes that are not a file go
  through `messages_from`, which takes any `Read`
- `Message` - one message, with keys read and written by name and typed by
  what you ask for. `GribMessage` adds grids and nearest-point search,
  `BufrMessage` adds `unpack`
- `Index` - select messages by key value without decoding the rest
- `Kind` - what a message actually is; the markers in `kind` make the product
  a type rather than an argument
- `Library` - the C library itself: version, search paths, debug output
- `Error` - eccodes return codes surfaced as typed Rust errors
- `logging` - routes the C library's own stderr diagnostics through the Rust
  [`log`](https://crates.io/crates/log) crate
- `missing` - absent and missing-coded values, as `Option` and as bitmaps

## Installation

```toml
[dependencies]
eccodes = "2.49"
```

The default `vendored` feature builds the eccodes C library from source,
which requires CMake and a C compiler.

## Usage

```rust,no_run
use eccodes::GribFile;

fn main() -> eccodes::Result<()> {
    let file = GribFile::open("data.grib2")?;
    println!("{} messages", file.count()?);

    for message in &file {
        let message = message?;
        let name: String = message.get("shortName")?;
        let values = message.values()?;
        println!("{name}: {} points", values.len());
    }
    Ok(())
}
```

## Cargo build features

- `vendored` (default) - Build the eccodes C library from source (forwards
  `eccodes-sys/vendored`). Forces `threads` on: the `Send` impl on messages
  relies on `libeccodes` locking its global context internally.
- `system` - Link against a system-installed eccodes (forwards
  `eccodes-sys/system`). `threads` is a no-op here — it is on you to link a
  library built with it.
- `raw` - Expose `Message::as_raw` and re-export `eccodes_sys` as `sys`, for
  handing a message to another library built on eccodes.

See the [`eccodes-sys` README](https://crates.io/crates/eccodes-sys) for the
full set of underlying C build features and environment variables.

## Copyright and License

Copyright 1996- European Centre for Medium-Range Weather Forecasts (ECMWF).

This software is licensed under the terms of the [Apache License, Version 2.0](LICENSE) which can also be obtained at http://www.apache.org/licenses/LICENSE-2.0.

In applying this licence, ECMWF does not waive the privileges and immunities granted to it by virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
