# eccodes-example

Minimal `eccodes-sys` consumer. Calls `codes_get_api_version()` and prints
the library version — if it runs, every linked library (eccodes, eckit,
aec) resolved at runtime.

## Run

```sh
cargo run -p eccodes-example
```

`build.rs` calls `bindman_utils::emit_rpaths()` so the binary finds the
vendored libraries at runtime. Every app using `eccodes-sys` needs that one
call in its own `build.rs`.

## Install

```sh
cargo install --path . --locked
eccodes-example
```

Works out of the box: every vendored build copies the shared libraries to
`$CARGO_HOME/lib/eccodes-sys` (i.e. `~/.cargo/lib/eccodes-sys`) and the
binary's rpaths include that directory, so installed binaries don't depend
on the build's `target/` dir. Set `ECCODES_SYS_LIB_DIR=/some/dir` to use a
different directory instead.
