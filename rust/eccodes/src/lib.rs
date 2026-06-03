//! # ecCodes Rust API
//!
//! A modern, idiomatic Rust interface to the ecCodes C library for reading and
//! writing GRIB and BUFR messages.
//!
//! ## Basic Usage (default features)
//!
//! ```rust,no_run
//! use eccodes::{MessageIterator, Message, Result};
//!
//! fn main() -> Result<()> {
//!     for message in MessageIterator::from_grib_file("data.grib2")? {
//!         let message = message?;
//!         let name: String = message.get("shortName")?;
//!         let step: i64 = message.get("step")?;
//!         let values: Vec<f64> = message.get("values")?;
//!         println!("{name} step={step} nvalues={}", values.len());
//!     }
//!     Ok(())
//! }
//! ```
//!
//! ## Advanced Usage (requires `features = ["advanced"]`)
//!
//! Enables: [`index`], [`nearest`], [`geo_iterator`], [`multi_handle`],
//! [`context`], [`util`].

pub mod error;
pub mod message;
pub mod keys_iterator;
pub mod types;

#[cfg(feature = "advanced")]
pub mod index;
#[cfg(feature = "advanced")]
pub mod nearest;
#[cfg(feature = "advanced")]
pub mod geo_iterator;
#[cfg(feature = "advanced")]
pub mod multi_handle;
#[cfg(feature = "advanced")]
pub mod context;
#[cfg(feature = "advanced")]
pub mod util;

pub use error::{EcCodesError, Result};
pub use message::{Message, MessageIterator};
pub use types::{ProductKind, KeyType};

use std::ffi::CStr;

// ── Internal helper ──────────────────────────────────────────────────────────

/// Call libc `free` to release C-allocated memory (e.g. strings from
/// `codes_get_string_array` / `codes_index_get_string`).
///
/// # Safety
/// `ptr` must have been allocated by the C runtime (malloc/calloc/strdup)
/// or be null (which is a no-op).
pub(crate) unsafe fn libc_free(ptr: *mut std::ffi::c_void) {
    extern "C" {
        fn free(ptr: *mut std::ffi::c_void);
    }
    free(ptr);
}

// ── Missing-value sentinels ──────────────────────────────────────────────────

/// The value used by ecCodes to represent missing doubles.
///
/// Matches the C macro `GRIB_MISSING_DOUBLE` (`-1e+100`).
pub const MISSING_DOUBLE: f64 = eccodes_sys::CODES_MISSING_DOUBLE;

/// The value used by ecCodes to represent missing longs.
///
/// Matches the C macro `GRIB_MISSING_LONG` (`2147483647`).
/// Note: this is `i32::MAX` regardless of the platform `c_long` width;
/// ecCodes defines the constant as `2147483647` in all builds.
pub const MISSING_LONG: i64 = eccodes_sys::CODES_MISSING_LONG as i64;

// ── Free functions (basic) ───────────────────────────────────────────────────

/// Count the number of messages in a file.
#[must_use]
pub fn count_messages(path: &str) -> Result<usize> {
    let c_path = std::ffi::CString::new(path).map_err(|_| EcCodesError::InvalidArgument)?;
    let mut n: std::os::raw::c_int = 0;
    let err = unsafe {
        eccodes_sys::codes_count_in_filename(
            std::ptr::null_mut(),
            c_path.as_ptr(),
            &mut n,
        )
    };
    EcCodesError::check(err)?;
    Ok(n as usize)
}

/// Return the ecCodes library version as an integer (e.g., 24800 for 2.48.0).
#[must_use]
pub fn api_version() -> i64 {
    unsafe { eccodes_sys::codes_get_api_version() as i64 }
}

/// Return the Git SHA1 of the ecCodes build.
#[must_use]
pub fn git_sha1() -> &'static str {
    unsafe {
        let p = eccodes_sys::codes_get_git_sha1();
        if p.is_null() { "" } else { CStr::from_ptr(p).to_str().unwrap_or("") }
    }
}

/// Return the package name.
#[must_use]
pub fn package_name() -> &'static str {
    unsafe {
        let p = eccodes_sys::codes_get_package_name();
        if p.is_null() { "" } else { CStr::from_ptr(p).to_str().unwrap_or("") }
    }
}

/// Return the build date string.
#[must_use]
pub fn build_date() -> &'static str {
    unsafe {
        let p = eccodes_sys::codes_get_build_date();
        if p.is_null() { "" } else { CStr::from_ptr(p).to_str().unwrap_or("") }
    }
}

/// Return the Git branch of the ecCodes build.
#[must_use]
pub fn git_branch() -> &'static str {
    unsafe {
        let p = eccodes_sys::codes_get_git_branch();
        if p.is_null() { "" } else { CStr::from_ptr(p).to_str().unwrap_or("") }
    }
}

/// Get the samples path for the default context.
///
/// The returned string is owned by the ecCodes context and remains valid
/// for the lifetime of the process.
#[must_use]
pub fn samples_path() -> &'static str {
    unsafe {
        let p = eccodes_sys::codes_samples_path(std::ptr::null());
        if p.is_null() { "" } else { CStr::from_ptr(p).to_str().unwrap_or("") }
    }
}

// ── Multi-field support (not gated behind "advanced") ────────────────────────

/// Turn on multi-field support for GRIB2 messages (global, default context).
///
/// When enabled, `MessageIterator` will unpack individual fields from
/// multi-field GRIB2 messages. This is required for most standard GRIB2 files.
pub fn grib_multi_support_on() {
    unsafe { eccodes_sys::codes_grib_multi_support_on(std::ptr::null_mut()); }
}

/// Turn off multi-field support for GRIB2 messages (global, default context).
pub fn grib_multi_support_off() {
    unsafe { eccodes_sys::codes_grib_multi_support_off(std::ptr::null_mut()); }
}
