fn main() {
    // Rpaths for all C/C++ dependency libraries: the vendored install trees
    // under target/ for `cargo build`/`cargo run`, and the stable
    // `ECCODES_SYS_LIB_DIR` export (if set) for binaries that outlive the
    // target dir, e.g. via `cargo install`.
    bindman_utils::emit_rpaths();
}
