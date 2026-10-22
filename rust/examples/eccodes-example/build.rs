fn main() {
    // Copies the vendored shared libraries into `<install-root>/lib/<app>`
    // (sibling of the `bin/` dir `cargo install` uses) and emits rpaths for
    // the target-dir locations (preferred while they exist), the stable
    // copy, and a relative `../lib/<app>` entry so a relocated install root
    // keeps working. Apps that never get installed can call
    // `bindman_utils::emit_rpaths()` instead.
    bindman_utils::install_runtime_libs();
}
