//! Guard against version drift between the crate and the C sources.
//!
//! The crate version is duplicated from the repo's `VERSION` file by
//! necessity — cargo cannot read it dynamically — and it drives which
//! eccodes tag the vendored build clones. This test fails CI whenever one
//! is bumped without the other.

#[test]
fn crate_version_matches_repo_version_file() {
    let path = concat!(env!("CARGO_MANIFEST_DIR"), "/../../../VERSION");
    let Ok(version) = std::fs::read_to_string(path) else {
        // Not building from the repo checkout (e.g. a packaged crate).
        return;
    };
    assert_eq!(
        version.trim(),
        env!("CARGO_PKG_VERSION"),
        "rust/crates/eccodes-sys/Cargo.toml version and VERSION file are out of sync"
    );
}
