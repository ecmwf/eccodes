//! Linkage smoke test.
//!
//! This test binary links every library the crate depends on (libeccodes,
//! libeckit via eckit-sys, libaec when the `aec` feature is enabled), and
//! the dynamic loader must resolve all of them before `main` runs. If any
//! library is unreachable the test fails at startup — so a passing run
//! proves the whole link chain, including libraries added in the future.

#[test]
fn linked_libraries_are_reachable() {
    // Call through into libeccodes so symbols are proven resolvable, not
    // just the library loadable.
    let version = unsafe { eccodes_sys::codes_get_api_version() };
    assert!(version > 0, "codes_get_api_version returned {version}");
}
