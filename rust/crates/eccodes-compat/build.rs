fn main() {
    // Rpaths for this crate's own test binaries; apps have their own build.rs.
    bindman_utils::emit_rpaths();
}
