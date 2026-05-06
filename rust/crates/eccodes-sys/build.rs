//! Build script for eccodes-sys
//!
//! Supports two build modes:
//! - `vendored` (default): Clone and build eccodes from source using ecbuild
//! - `system`: Use `CMake` `find_package` to find system-installed eccodes

use std::env;
use std::path::{Path, PathBuf};

const ECCODES_VERSION: &str = "2.47.0";

fn main() {
    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-env-changed=ECCODES_DIR");
    println!("cargo:rerun-if-env-changed=CMAKE_PREFIX_PATH");
    println!("cargo:rerun-if-env-changed=DOCS_RS");

    // Skip build for docs.rs - generate stub bindings instead
    if bindman_utils::is_docs_rs() {
        let out_dir = PathBuf::from(std::env::var("OUT_DIR").expect("OUT_DIR not set"));
        std::fs::write(
            out_dir.join("bindings.rs"),
            include_str!("src/bindings_stub.rs"),
        )
        .expect("Failed to write stub bindings");
        return;
    }

    bindman_utils::validate_build_mode(cfg!(feature = "system"), cfg!(feature = "vendored"));

    let out_dir = PathBuf::from(env::var("OUT_DIR").expect("OUT_DIR not set"));

    if cfg!(feature = "system") {
        build_system(&out_dir);
    } else {
        build_vendored(&out_dir);
    }
}

/// Build using system-installed eccodes via `CMake` `find_package`
#[cfg(feature = "system")]
fn build_system(out_dir: &Path) {
    let (root, include, lib_dir) = bindman_utils::cmake_find_package("eccodes", ECCODES_VERSION);

    println!("cargo:rustc-link-search=native={}", lib_dir.display());
    println!("cargo:rustc-link-lib=dylib=eccodes");

    // Export for downstream crates
    println!("cargo:root={}", root.display());
    println!("cargo:include={}", include.display());

    // Generate bindings
    generate_bindings(out_dir, &include);
}

#[cfg(not(feature = "system"))]
fn build_system(_out_dir: &Path) {
    unreachable!("build_system called without system feature");
}

#[cfg(feature = "vendored")]
fn build_libaec(build_dir: &Path, install_dir: &Path, aec_src: &Path, num_jobs: &str) -> PathBuf {
    use std::fs;
    use std::process::Command;

    let aec_build_dir = build_dir.join("libaec");
    let aec_install_dir = install_dir.join("libaec");
    fs::create_dir_all(&aec_build_dir).expect("Failed to create libaec build directory");

    let mut aec_cmd = Command::new("cmake");
    aec_cmd
        .current_dir(&aec_build_dir)
        .arg(format!(
            "-DCMAKE_INSTALL_PREFIX={}",
            aec_install_dir.display()
        ))
        .arg(format!(
            "-DCMAKE_BUILD_TYPE={}",
            bindman_utils::cmake_build_type()
        ))
        .arg("-DBUILD_SHARED_LIBS=ON");

    #[cfg(target_os = "macos")]
    aec_cmd.arg("-DCMAKE_INSTALL_NAME_DIR=@rpath");

    aec_cmd.arg(aec_src);
    bindman_utils::run_command(&mut aec_cmd, "cmake configure libaec");

    bindman_utils::run_command(
        Command::new("cmake")
            .args(["--build", ".", "--parallel", num_jobs])
            .current_dir(&aec_build_dir),
        "cmake build libaec",
    );

    bindman_utils::run_command(
        Command::new("cmake")
            .args(["--install", "."])
            .current_dir(&aec_build_dir),
        "cmake install libaec",
    );

    aec_install_dir
}

#[cfg(feature = "vendored")]
fn build_eccodes_impl(
    build_dir: &Path,
    install_dir: &Path,
    eccodes_src: &Path,
    ecbuild_bin: &Path,
    eckit_root: &str,
    aec_install_dir: &Path,
    num_jobs: &str,
) -> PathBuf {
    use std::fs;
    use std::process::Command;

    let eccodes_build_dir = build_dir.join("eccodes");
    let eccodes_install_dir = install_dir.join("eccodes");
    fs::create_dir_all(&eccodes_build_dir).expect("Failed to create eccodes build directory");

    let cmake_prefix_path = format!("{eckit_root};{}", aec_install_dir.display());

    let mut cmd = Command::new(ecbuild_bin);
    cmd.current_dir(&eccodes_build_dir)
        .arg(format!("--prefix={}", eccodes_install_dir.display()))
        .arg("--")
        .arg(eccodes_src)
        .arg(format!("-DCMAKE_PREFIX_PATH={cmake_prefix_path}"))
        .arg(format!(
            "-DCMAKE_BUILD_TYPE={}",
            bindman_utils::cmake_build_type()
        ))
        // Always disabled (no features)
        .arg("-DENABLE_TESTS=OFF")
        .arg("-DENABLE_EXAMPLES=OFF")
        .arg("-DENABLE_FORTRAN=OFF")
        .arg("-DENABLE_PYTHON=OFF")
        .arg("-DENABLE_BUILD_TOOLS=OFF");

    // Feature-gated options
    cmd.arg(format!(
        "-DENABLE_PRODUCT_GRIB={}",
        bindman_utils::on_off(cfg!(feature = "product-grib"))
    ));
    cmd.arg(format!(
        "-DENABLE_PRODUCT_BUFR={}",
        bindman_utils::on_off(cfg!(feature = "product-bufr"))
    ));
    cmd.arg(format!(
        "-DENABLE_GEOGRAPHY={}",
        bindman_utils::on_off(cfg!(feature = "geography"))
    ));
    cmd.arg(format!(
        "-DENABLE_AEC={}",
        bindman_utils::on_off(cfg!(feature = "aec"))
    ));
    cmd.arg(format!(
        "-DENABLE_JPG={}",
        bindman_utils::on_off(cfg!(feature = "jpg"))
    ));
    cmd.arg(format!(
        "-DENABLE_PNG={}",
        bindman_utils::on_off(cfg!(feature = "png"))
    ));
    cmd.arg(format!(
        "-DENABLE_NETCDF={}",
        bindman_utils::on_off(cfg!(feature = "netcdf"))
    ));
    cmd.arg(format!(
        "-DENABLE_MEMFS={}",
        bindman_utils::on_off(cfg!(feature = "memfs"))
    ));
    cmd.arg(format!(
        "-DENABLE_ECCODES_THREADS={}",
        bindman_utils::on_off(cfg!(feature = "eccodes-threads"))
    ));
    cmd.arg(format!(
        "-DENABLE_ECCODES_OMP_THREADS={}",
        bindman_utils::on_off(cfg!(feature = "eccodes-omp-threads"))
    ));

    #[cfg(target_os = "macos")]
    cmd.arg("-DCMAKE_INSTALL_NAME_DIR=@rpath");

    bindman_utils::run_command(&mut cmd, "ecbuild configure eccodes");

    bindman_utils::run_command(
        Command::new("cmake")
            .args(["--build", ".", "--parallel", num_jobs])
            .current_dir(&eccodes_build_dir),
        "cmake build eccodes",
    );

    bindman_utils::run_command(
        Command::new("cmake")
            .args(["--install", "."])
            .current_dir(&eccodes_build_dir),
        "cmake install eccodes",
    );

    eccodes_install_dir
}

/// Emit link directives pointing to the target directory where libs are copied.
#[cfg(feature = "vendored")]
fn emit_link_directives(libs_dest: &Path, eccodes_install_dir: &Path, aec_install_dir: &Path) {
    println!("cargo:rustc-link-search=native={}", libs_dest.display());
    println!("cargo:rustc-link-lib=dylib=eccodes");
    println!("cargo:rustc-link-lib=dylib=aec");

    // Export for downstream crates (still point to install dir for headers)
    println!("cargo:root={}", eccodes_install_dir.display());
    println!(
        "cargo:include={}",
        eccodes_install_dir.join("include").display()
    );
    println!("cargo:aec_root={}", aec_install_dir.display());
}

/// Copy resources (definitions, samples, libs) to target directory for portable binaries.
/// Returns the path to the libs directory where libraries were copied.
#[cfg(feature = "vendored")]
fn copy_resources_to_output(eccodes_install_dir: &Path, aec_install_dir: &Path) -> PathBuf {
    #[cfg(not(feature = "memfs"))]
    use fs_extra::dir::{CopyOptions, copy};

    let target_dir = bindman_utils::target_profile_dir();

    let eccodes_lib_dir = bindman_utils::resolve_lib_dir(eccodes_install_dir);
    let aec_lib_dir = bindman_utils::resolve_lib_dir(aec_install_dir);

    // Copy definitions and samples to target directory.
    // With `feature = "memfs"` the tables are baked into `libeccodes` itself.
    #[cfg(not(feature = "memfs"))]
    {
        let share_dir = eccodes_install_dir.join("share/eccodes");
        let resources_dest = target_dir.join("eccodes_resources");
        if share_dir.exists() {
            let options = CopyOptions::new().overwrite(true);
            let _ = std::fs::create_dir_all(&resources_dest);

            let definitions_src = share_dir.join("definitions");
            if definitions_src.exists() {
                if let Err(e) = copy(&definitions_src, &resources_dest, &options) {
                    eprintln!("Warning: Failed to copy definitions: {e}");
                } else {
                    eprintln!(
                        "Copied eccodes definitions to {}",
                        resources_dest.join("definitions").display()
                    );
                }
            }

            let samples_src = share_dir.join("samples");
            if samples_src.exists() {
                if let Err(e) = copy(&samples_src, &resources_dest, &options) {
                    eprintln!("Warning: Failed to copy samples: {e}");
                } else {
                    eprintln!(
                        "Copied eccodes samples to {}",
                        resources_dest.join("samples").display()
                    );
                }
            }
        }
    }

    // Copy dynamic libraries to target directory
    let libs_dest = target_dir.join("eccodes_libs");
    bindman_utils::copy_shared_libs(&eccodes_lib_dir, &libs_dest, "eccodes");
    bindman_utils::copy_shared_libs(&aec_lib_dir, &libs_dest, "libaec");

    #[cfg(not(feature = "memfs"))]
    println!("cargo:rustc-env=ECCODES_RESOURCES_DIR=eccodes_resources");
    println!("cargo:rustc-env=ECCODES_LIBS_DIR=eccodes_libs");

    libs_dest
}

/// Build eccodes from source using ecbuild
#[cfg(feature = "vendored")]
fn build_vendored(out_dir: &Path) {
    use std::fs;

    const ECBUILD_REPO: &str = "https://github.com/ecmwf/ecbuild.git";
    const ECBUILD_TAG: &str = "3.13.1";

    const AEC_REPO: &str = "https://gitlab.dkrz.de/k202009/libaec.git";
    const AEC_TAG: &str = "v1.1.4";

    const ECCODES_REPO: &str = "https://github.com/ecmwf/eccodes.git";

    let src_dir = out_dir.join("src");
    let build_dir = out_dir.join("build");
    let install_dir = out_dir.join("install");

    fs::create_dir_all(&src_dir).expect("Failed to create src directory");

    // Get eckit paths from dependency
    let eckit_root = env::var("DEP_ECKIT_SYS_ROOT")
        .expect("DEP_ECKIT_SYS_ROOT not set - eckit-sys must be a dependency");

    // Clone sources
    let ecbuild_src = bindman_utils::git_clone(ECBUILD_REPO, ECBUILD_TAG, &src_dir.join("ecbuild"));
    let aec_src = bindman_utils::git_clone(AEC_REPO, AEC_TAG, &src_dir.join("libaec"));
    let eccodes_src =
        bindman_utils::git_clone(ECCODES_REPO, ECCODES_VERSION, &src_dir.join("eccodes"));

    let ecbuild_bin = ecbuild_src.join("bin/ecbuild");
    let num_jobs = bindman_utils::build_parallelism();

    // Build libaec
    let aec_install_dir = build_libaec(&build_dir, &install_dir, &aec_src, &num_jobs);

    // Build eccodes
    let eccodes_install_dir = build_eccodes_impl(
        &build_dir,
        &install_dir,
        &eccodes_src,
        &ecbuild_bin,
        &eckit_root,
        &aec_install_dir,
        &num_jobs,
    );

    // IMPORTANT: Copy resources FIRST, then link against the copied location.
    let libs_dest = copy_resources_to_output(&eccodes_install_dir, &aec_install_dir);

    // Link against the copied location in target directory
    emit_link_directives(&libs_dest, &eccodes_install_dir, &aec_install_dir);

    // Emit RPATH flags for runtime library discovery
    bindman_utils::emit_rpath_flags(&["eccodes_libs"]);

    // Generate bindings
    generate_bindings(out_dir, &eccodes_install_dir.join("include"));
}

#[cfg(not(feature = "vendored"))]
fn build_vendored(_out_dir: &Path) {
    unreachable!("build_vendored called without vendored feature");
}

fn generate_bindings(out_dir: &Path, include_dir: &Path) {
    let header = include_dir.join("eccodes.h");
    let bindings = bindgen::Builder::default()
        .header(header.to_str().expect("Invalid header path"))
        .clang_arg(format!("-I{}", include_dir.display()))
        .allowlist_function("codes_.*")
        .allowlist_function("grib_.*")
        .allowlist_type("codes_.*")
        .allowlist_type("grib_.*")
        .allowlist_var("CODES_.*")
        .allowlist_var("GRIB_.*")
        .generate()
        .expect("Failed to generate bindings");

    bindings
        .write_to_file(out_dir.join("bindings.rs"))
        .expect("Failed to write bindings");
}
