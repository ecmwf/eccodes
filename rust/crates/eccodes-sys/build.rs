//! Build script for eccodes-sys
//!
//! Supports two build modes:
//! - `vendored` (default): Clone and build eccodes from source using ecbuild
//! - `system`: Use `CMake` `find_package` to find system-installed eccodes

use std::env;
use std::path::{Path, PathBuf};

fn main() {
    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-env-changed=ECCODES_DIR");
    println!("cargo:rerun-if-env-changed=CMAKE_PREFIX_PATH");

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
    let (root, include, lib_dir) = bindman_utils::cmake_find_package("eccodes", "2.20.0");

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
fn build_libaec_if_enabled(
    src_dir: &Path,
    build_dir: &Path,
    install_dir: &Path,
    num_jobs: &str,
) -> Option<PathBuf> {
    if !cfg!(feature = "aec") {
        return None;
    }
    const AEC_REPO: &str = "https://gitlab.dkrz.de/k202009/libaec.git";
    const AEC_TAG: &str = "v1.1.4";
    let aec_src = bindman_utils::git_clone(AEC_REPO, AEC_TAG, &src_dir.join("libaec"));
    Some(build_libaec(build_dir, install_dir, &aec_src, num_jobs))
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
    aec_install_dir: Option<&Path>,
    num_jobs: &str,
) -> PathBuf {
    use std::fs;
    use std::process::Command;

    let eccodes_build_dir = build_dir.join("eccodes");
    let eccodes_install_dir = install_dir.join("eccodes");
    fs::create_dir_all(&eccodes_build_dir).expect("Failed to create eccodes build directory");

    let cmake_prefix_path = aec_install_dir.as_ref().map_or_else(
        || eckit_root.to_string(),
        |p| format!("{eckit_root};{}", p.display()),
    );

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
fn emit_link_directives(
    libs_dest: &Path,
    eccodes_install_dir: &Path,
    aec_install_dir: Option<&Path>,
) {
    println!("cargo:rustc-link-search=native={}", libs_dest.display());
    println!("cargo:rustc-link-lib=dylib=eccodes");
    if aec_install_dir.is_some() {
        println!("cargo:rustc-link-lib=dylib=aec");
    }

    // Export for downstream crates (still point to install dir for headers)
    println!("cargo:root={}", eccodes_install_dir.display());
    println!(
        "cargo:include={}",
        eccodes_install_dir.join("include").display()
    );
    if let Some(p) = aec_install_dir {
        println!("cargo:aec_root={}", p.display());
    }
}

/// Copy resources (definitions, samples, libs) to target directory for portable binaries.
/// Returns the path to the libs directory where libraries were copied.
#[cfg(feature = "vendored")]
fn copy_resources_to_output(eccodes_install_dir: &Path, aec_install_dir: Option<&Path>) -> PathBuf {
    #[cfg(not(feature = "memfs"))]
    use fs_extra::dir::{CopyOptions, copy};

    let target_dir = bindman_utils::target_profile_dir();

    let eccodes_lib_dir = bindman_utils::resolve_lib_dir(eccodes_install_dir);
    let aec_lib_dir = aec_install_dir.map(bindman_utils::resolve_lib_dir);

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
    if let Some(aec_lib_dir) = aec_lib_dir.as_ref() {
        bindman_utils::copy_shared_libs(aec_lib_dir, &libs_dest, "libaec");
    }

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

    const ECCODES_REPO: &str = "https://github.com/ecmwf/eccodes.git";
    const ECCODES_TAG: &str = "2.47.0";

    let src_dir = out_dir.join("src");
    let build_dir = out_dir.join("build");
    let install_dir = out_dir.join("install");

    fs::create_dir_all(&src_dir).expect("Failed to create src directory");

    // Get eckit paths from dependency
    let eckit_root = env::var("DEP_ECKIT_SYS_ROOT")
        .expect("DEP_ECKIT_SYS_ROOT not set - eckit-sys must be a dependency");

    // Clone sources
    let ecbuild_src = bindman_utils::git_clone(ECBUILD_REPO, ECBUILD_TAG, &src_dir.join("ecbuild"));
    let eccodes_src = bindman_utils::git_clone(ECCODES_REPO, ECCODES_TAG, &src_dir.join("eccodes"));

    let ecbuild_bin = ecbuild_src.join("bin/ecbuild");
    let num_jobs = bindman_utils::build_parallelism();

    // Build libaec (only when the `aec` feature is enabled)
    let aec_install_dir = build_libaec_if_enabled(&src_dir, &build_dir, &install_dir, &num_jobs);

    // Build eccodes
    let eccodes_install_dir = build_eccodes_impl(
        &build_dir,
        &install_dir,
        &eccodes_src,
        &ecbuild_bin,
        &eckit_root,
        aec_install_dir.as_deref(),
        &num_jobs,
    );

    // IMPORTANT: Copy resources FIRST, then link against the copied location.
    let libs_dest = copy_resources_to_output(&eccodes_install_dir, aec_install_dir.as_deref());

    // Link against the copied location in target directory
    emit_link_directives(&libs_dest, &eccodes_install_dir, aec_install_dir.as_deref());

    // Emit RPATH flags for runtime library discovery
    bindman_utils::emit_rpath_flags(&["eccodes_libs"]);

    // Generate bindings
    generate_bindings(out_dir, &eccodes_install_dir.join("include"));
}

#[cfg(not(feature = "vendored"))]
fn build_vendored(_out_dir: &Path) {
    unreachable!("build_vendored called without vendored feature");
}

// Curated API surface — mirrors the maintainer's reference at
// ecmwf/eccodes#feature/rust_api:rust/eccodes-sys/src/lib.rs
// Keep these lists in lockstep with that file when bumping eccodes.

const ALLOWED_FUNCTIONS: &[&str] = &[
    // -- libc stdio (used by codes_handle_new_from_file etc.) --
    "fopen",
    "fclose",
    // -- Version / info --
    "codes_get_api_version",
    "codes_get_git_sha1",
    "codes_get_git_branch",
    "codes_get_build_date",
    "codes_get_package_name",
    "codes_get_error_message",
    "codes_get_type_name",
    // -- Context --
    "codes_context_get_default",
    "codes_context_delete",
    "codes_context_set_definitions_path",
    "codes_context_set_samples_path",
    // -- Counting --
    "codes_count_in_file",
    "codes_count_in_filename",
    // -- Handle creation --
    "codes_handle_new_from_file",
    "codes_grib_handle_new_from_file",
    "codes_bufr_handle_new_from_file",
    "codes_handle_new_from_message",
    "codes_handle_new_from_message_copy",
    "codes_handle_new_from_samples",
    "codes_grib_handle_new_from_samples",
    "codes_bufr_handle_new_from_samples",
    "codes_handle_clone",
    "codes_handle_clone_headers_only",
    // -- Handle deletion --
    "codes_handle_delete",
    // -- Get scalar --
    "codes_get_long",
    "codes_get_double",
    "codes_get_float",
    "codes_get_string",
    "codes_get_bytes",
    // -- Get array --
    "codes_get_long_array",
    "codes_get_double_array",
    "codes_get_float_array",
    "codes_get_string_array",
    // -- Get element --
    "codes_get_double_element",
    "codes_get_float_element",
    "codes_get_double_elements",
    "codes_get_float_elements",
    // -- Get metadata --
    "codes_get_size",
    "codes_get_length",
    "codes_get_offset",
    "codes_get_native_type",
    "codes_is_missing",
    "codes_is_defined",
    "codes_get_product_kind",
    // -- Set scalar --
    "codes_set_long",
    "codes_set_double",
    "codes_set_string",
    "codes_set_bytes",
    "codes_set_missing",
    // -- Set array --
    "codes_set_long_array",
    "codes_set_double_array",
    "codes_set_float_array",
    "codes_set_string_array",
    // -- Set force --
    "codes_set_force_double_array",
    "codes_set_force_float_array",
    // -- Write --
    "codes_write_message",
    "codes_get_message",
    "codes_get_message_copy",
    "codes_get_message_offset",
    "codes_get_message_size",
    // -- Keys iterator --
    "codes_keys_iterator_new",
    "codes_keys_iterator_next",
    "codes_keys_iterator_get_name",
    "codes_keys_iterator_delete",
    "codes_keys_iterator_rewind",
    // -- Geo iterator --
    "codes_grib_iterator_new",
    "codes_grib_iterator_next",
    "codes_grib_iterator_previous",
    "codes_grib_iterator_has_next",
    "codes_grib_iterator_reset",
    "codes_grib_iterator_delete",
    "codes_grib_get_data",
    // -- Nearest --
    "codes_grib_nearest_new",
    "codes_grib_nearest_find",
    "codes_grib_nearest_delete",
    "codes_grib_nearest_find_multiple",
    // -- Index --
    "codes_index_new",
    "codes_index_new_from_file",
    "codes_index_add_file",
    "codes_index_read",
    "codes_index_write",
    "codes_index_delete",
    "codes_index_get_size",
    "codes_index_get_long",
    "codes_index_get_double",
    "codes_index_get_string",
    "codes_index_select_long",
    "codes_index_select_double",
    "codes_index_select_string",
    "codes_handle_new_from_index",
    // -- Multi handle --
    "codes_grib_multi_handle_new",
    "codes_grib_multi_handle_append",
    "codes_grib_multi_handle_delete",
    "codes_grib_multi_handle_write",
    // -- Copy / Namespace --
    "codes_copy_namespace",
    // -- BUFR --
    "codes_bufr_copy_data",
    // -- Context config --
    "codes_context_set_debug",
    "codes_grib_multi_support_on",
    "codes_grib_multi_support_off",
    "codes_samples_path",
    // -- Utilities --
    "codes_get_gaussian_latitudes",
    "codes_datetime_to_julian",
    "codes_julian_to_datetime",
    "codes_julian_to_date",
    "codes_date_to_julian",
    "codes_dump_content",
    "codes_key_is_computed",
    "codes_extract_offsets_malloc",
];

const ALLOWED_TYPES: &[&str] = &[
    "codes_handle",
    "codes_context",
    "codes_keys_iterator",
    "codes_iterator",
    "codes_nearest",
    "codes_index",
    "codes_multi_handle",
    "codes_bufr_header",
    "ProductKind",
    "FILE",
];

const ALLOWED_VARS: &[&str] = &[
    // Type constants
    "CODES_TYPE_UNDEFINED",
    "CODES_TYPE_LONG",
    "CODES_TYPE_DOUBLE",
    "CODES_TYPE_STRING",
    "CODES_TYPE_BYTES",
    "CODES_TYPE_SECTION",
    "CODES_TYPE_LABEL",
    "CODES_TYPE_MISSING",
    // Error codes
    "CODES_SUCCESS",
    "CODES_END_OF_FILE",
    "CODES_INTERNAL_ERROR",
    "CODES_BUFFER_TOO_SMALL",
    "CODES_NOT_IMPLEMENTED",
    "CODES_7777_NOT_FOUND",
    "CODES_ARRAY_TOO_SMALL",
    "CODES_FILE_NOT_FOUND",
    "CODES_CODE_NOT_FOUND_IN_TABLE",
    "CODES_WRONG_ARRAY_SIZE",
    "CODES_NOT_FOUND",
    "CODES_IO_PROBLEM",
    "CODES_INVALID_MESSAGE",
    "CODES_DECODING_ERROR",
    "CODES_ENCODING_ERROR",
    "CODES_NO_MORE_IN_SET",
    "CODES_GEOCALCULUS_PROBLEM",
    "CODES_OUT_OF_MEMORY",
    "CODES_READ_ONLY",
    "CODES_INVALID_ARGUMENT",
    "CODES_NULL_HANDLE",
    "CODES_INVALID_SECTION_NUMBER",
    "CODES_VALUE_CANNOT_BE_MISSING",
    "CODES_WRONG_LENGTH",
    "CODES_INVALID_TYPE",
    "CODES_WRONG_STEP",
    "CODES_WRONG_STEP_UNIT",
    "CODES_INVALID_FILE",
    "CODES_INVALID_GRIB",
    "CODES_INVALID_INDEX",
    "CODES_INVALID_ITERATOR",
    "CODES_INVALID_KEYS_ITERATOR",
    "CODES_INVALID_NEAREST",
    "CODES_INVALID_ORDERBY",
    "CODES_MISSING_KEY",
    "CODES_OUT_OF_AREA",
    "CODES_CONCEPT_NO_MATCH",
    "CODES_HASH_ARRAY_NO_MATCH",
    "CODES_NO_DEFINITIONS",
    "CODES_WRONG_TYPE",
    "CODES_END",
    "CODES_NO_VALUES",
    "CODES_WRONG_GRID",
    "CODES_END_OF_INDEX",
    "CODES_NULL_INDEX",
    "CODES_PREMATURE_END_OF_FILE",
    "CODES_INTERNAL_ARRAY_TOO_SMALL",
    "CODES_MESSAGE_TOO_LARGE",
    "CODES_CONSTANT_FIELD",
    "CODES_SWITCH_NO_MATCH",
    "CODES_UNDERFLOW",
    "CODES_MESSAGE_MALFORMED",
    "CODES_CORRUPTED_INDEX",
    "CODES_INVALID_BPV",
    "CODES_DIFFERENT_EDITION",
    "CODES_VALUE_DIFFERENT",
    "CODES_INVALID_KEY_VALUE",
    "CODES_STRING_TOO_SMALL",
    "CODES_WRONG_CONVERSION",
    "CODES_MISSING_BUFR_ENTRY",
    "CODES_NULL_POINTER",
    "CODES_ATTRIBUTE_CLASH",
    "CODES_TOO_MANY_ATTRIBUTES",
    "CODES_ATTRIBUTE_NOT_FOUND",
    "CODES_UNSUPPORTED_EDITION",
    "CODES_OUT_OF_RANGE",
    "CODES_WRONG_BITMAP_SIZE",
    "CODES_FUNCTIONALITY_NOT_ENABLED",
    // Keys iterator flags
    "CODES_KEYS_ITERATOR_ALL_KEYS",
    "CODES_KEYS_ITERATOR_SKIP_READ_ONLY",
    "CODES_KEYS_ITERATOR_SKIP_OPTIONAL",
    "CODES_KEYS_ITERATOR_SKIP_EDITION_SPECIFIC",
    "CODES_KEYS_ITERATOR_SKIP_CODED",
    "CODES_KEYS_ITERATOR_SKIP_COMPUTED",
    "CODES_KEYS_ITERATOR_SKIP_DUPLICATES",
    "CODES_KEYS_ITERATOR_SKIP_FUNCTION",
    "CODES_KEYS_ITERATOR_DUMP_ONLY",
    // Nearest flags
    "CODES_NEAREST_SAME_GRID",
    "CODES_NEAREST_SAME_DATA",
    "CODES_NEAREST_SAME_POINT",
    // Geoiterator flags
    "CODES_GEOITERATOR_NO_VALUES",
    // Missing value sentinels
    "CODES_MISSING_LONG",
    "CODES_MISSING_DOUBLE",
];

fn generate_bindings(out_dir: &Path, include_dir: &Path) {
    let header = include_dir.join("eccodes.h");
    let mut builder = bindgen::Builder::default()
        .header(header.to_str().expect("Invalid header path"))
        .clang_arg(format!("-I{}", include_dir.display()));

    for name in ALLOWED_FUNCTIONS {
        builder = builder.allowlist_function(name);
    }
    for name in ALLOWED_TYPES {
        builder = builder.allowlist_type(name);
    }
    for name in ALLOWED_VARS {
        builder = builder.allowlist_var(name);
    }

    let bindings = builder.generate().expect("Failed to generate bindings");

    bindings
        .write_to_file(out_dir.join("bindings.rs"))
        .expect("Failed to write bindings");
}
