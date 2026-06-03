use std::env;
use std::path::PathBuf;

fn main() {
    // Allow overriding via ECCODES_DIR environment variable
    if let Ok(eccodes_dir) = env::var("ECCODES_DIR") {
        let lib_dir = PathBuf::from(&eccodes_dir).join("lib");
        println!("cargo:rustc-link-search=native={}", lib_dir.display());
    }

    // Also check ECCODES_LIB_DIR
    if let Ok(lib_dir) = env::var("ECCODES_LIB_DIR") {
        println!("cargo:rustc-link-search=native={}", lib_dir);
    }

    println!("cargo:rustc-link-lib=dylib=eccodes");

    // Re-run if env changes
    println!("cargo:rerun-if-env-changed=ECCODES_DIR");
    println!("cargo:rerun-if-env-changed=ECCODES_LIB_DIR");
}
