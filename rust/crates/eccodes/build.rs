fn main() {
    // Rpaths for this crate's own test/example binaries; apps have their own build.rs.
    bindman_utils::emit_rpaths();
}
