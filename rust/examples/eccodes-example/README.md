# eccodes-example

Minimal `eccodes-sys` consumer. Calls `codes_get_api_version()` and prints
the library version — if it runs, every linked library (eccodes, eckit,
aec) resolved at runtime.

## Run

```sh
cargo run -p eccodes-example
```

## Install

```sh
cargo install --path . --locked
eccodes-example
```

Works out of the box. `build.rs` calls
`bindman_utils::install_runtime_libs()`, which copies the vendored shared
libraries to `lib/eccodes-example` under the cargo install root — next to
the `bin/` dir the binary lands in (default: `~/.cargo/bin/eccodes-example`
+ `~/.cargo/lib/eccodes-example`). Each app gets its own lib dir, so apps
never interfere with each other's libraries.

To install elsewhere, use cargo's own mechanism:

```sh
CARGO_INSTALL_ROOT=/opt/tools cargo install --path . --locked
# -> /opt/tools/bin/eccodes-example + /opt/tools/lib/eccodes-example
```

The binary also carries a relative `../lib/eccodes-example` rpath, so the
whole install root can be moved afterwards and keeps working. (Use the env
var, not `--root` — cargo doesn't expose the flag to build scripts.)

Every app using `eccodes-sys` needs that one `build.rs` call (or
`bindman_utils::emit_rpaths()` if it will only ever run via `cargo run`).
