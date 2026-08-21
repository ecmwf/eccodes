//! [`Library`] — the eccodes C library itself: which build is loaded, where
//! it looks for its definitions, and the tables it can compute.
//!
//! ```no_run
//! use eccodes::{Library, Version};
//!
//! # fn main() -> eccodes::Result<()> {
//! println!("eccodes {}", Library::version());
//! if Library::version() < Version::new(2, 49, 0) {
//!     println!("older than this crate was written against");
//! }
//! Library::set_definitions_path("/usr/share/eccodes/definitions")?;
//! # Ok(())
//! # }
//! ```
//!
//! Everything here is process-wide: the C library keeps one context, and
//! these calls configure it for the whole program.

use std::fmt;
use std::path::{Path, PathBuf};
use std::ptr;

use eccodes_sys as sys;

use crate::error::{Code, Error, Result, check};
use crate::ffi;

/// A released version of the eccodes C library.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Version {
    /// Major version.
    pub major: u32,
    /// Minor version.
    pub minor: u32,
    /// Patch version.
    pub patch: u32,
}

impl Version {
    /// A version from its three parts.
    #[must_use]
    pub const fn new(major: u32, minor: u32, patch: u32) -> Self {
        Self {
            major,
            minor,
            patch,
        }
    }

    /// Unpack the library's `MMmmpp` encoding, e.g. `24900` for 2.49.0.
    fn from_packed(packed: i64) -> Self {
        let part = |value: i64| u32::try_from(value).unwrap_or(0);
        Self {
            major: part(packed / 10_000),
            minor: part((packed / 100) % 100),
            patch: part(packed % 100),
        }
    }
}

impl fmt::Display for Version {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}.{}.{}", self.major, self.minor, self.patch)
    }
}

/// The eccodes C library loaded into this process.
///
/// A handle to something global rather than something owned — hence
/// associated functions and no constructor.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Library;

impl Library {
    /// The library's version.
    #[must_use]
    pub fn version() -> Version {
        // SAFETY: takes no arguments and returns a packed integer.
        Version::from_packed(unsafe { sys::codes_get_api_version() })
    }

    /// The commit the library was built from.
    #[must_use]
    pub fn git_sha1() -> &'static str {
        // SAFETY: returns a static NUL-terminated string, never NULL.
        unsafe { ffi::static_str(sys::codes_get_git_sha1()) }.unwrap_or_default()
    }

    /// When the library was built.
    #[must_use]
    pub fn build_date() -> &'static str {
        // SAFETY: returns a static NUL-terminated string, never NULL.
        unsafe { ffi::static_str(sys::codes_get_build_date()) }.unwrap_or_default()
    }

    /// Where the library looks for sample messages — the names
    /// [`Message::from_sample`](crate::Message::from_sample) accepts.
    pub fn samples_path() -> Result<PathBuf> {
        // SAFETY: a NULL context selects the default one; the returned string
        // is owned by the library and outlives the call.
        let path = unsafe { ffi::static_str(sys::codes_samples_path(ptr::null())) };
        path.map(PathBuf::from)
            .ok_or_else(|| Error::from(Code::NullPointer))
    }

    /// Look for sample messages in `path` instead.
    pub fn set_samples_path(path: impl AsRef<Path>) -> Result<()> {
        let path = path.as_ref();
        let cpath = ffi::cpath(path)?;
        // SAFETY: a NULL context selects the default one; `cpath` is
        // NUL-terminated and copied by the library.
        unsafe { sys::codes_context_set_samples_path(ptr::null_mut(), cpath.as_ptr()) };
        Ok(())
    }

    /// Look for key definitions in `path` instead.
    pub fn set_definitions_path(path: impl AsRef<Path>) -> Result<()> {
        let path = path.as_ref();
        let cpath = ffi::cpath(path)?;
        // SAFETY: as for `set_samples_path`.
        unsafe { sys::codes_context_set_definitions_path(ptr::null_mut(), cpath.as_ptr()) };
        Ok(())
    }

    /// Print the library's debug output to stderr.
    pub fn set_debug(enabled: bool) {
        // SAFETY: a NULL context selects the default one.
        unsafe { sys::codes_context_set_debug(ptr::null_mut(), i32::from(enabled)) };
    }

    /// The `2 * n` Gaussian latitudes of a global Gaussian grid of order `n`,
    /// from north to south.
    ///
    /// `n` is the Gaussian number — 640 for O640, and so on.
    pub fn gaussian_latitudes(n: usize) -> Result<Vec<f64>> {
        let mut latitudes = vec![0.0_f64; n.saturating_mul(2)];
        check!(sys::codes_get_gaussian_latitudes(
            ffi::to_c_long(n)?,
            latitudes.as_mut_ptr()
        ))?;
        Ok(latitudes)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn version_unpacks_the_librarys_encoding() {
        // The C API reports 2.49.0 as the integer 24900; the point of `Version`
        // is that callers never see that.
        assert_eq!(Version::from_packed(24_900), Version::new(2, 49, 0));
        assert_eq!(Version::from_packed(21_007), Version::new(2, 10, 7));
        assert_eq!(Version::from_packed(0), Version::new(0, 0, 0));
    }

    #[test]
    fn versions_order_by_significance() {
        // Ordering is what callers actually do with a version.
        assert!(Version::new(2, 49, 0) > Version::new(2, 9, 9));
        assert!(Version::new(2, 49, 0) < Version::new(10, 0, 0));
    }

    #[test]
    fn version_displays_as_three_parts() {
        assert_eq!(Version::new(2, 49, 0).to_string(), "2.49.0");
    }
}
