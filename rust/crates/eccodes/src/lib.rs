//! Safe Rust wrapper for ECMWF's eccodes (GRIB/BUFR decoding and encoding).
//!
//! Provides:
//! - [`Handle`] — a single GRIB/BUFR message; typed key access via
//!   [`Handle::get`]/[`Handle::set`], compile-time product kinds via
//!   [`kind`]
//! - [`MessageReader`] — iteration over the messages in a file
//! - [`Index`] — indexed access to messages across files
//! - [`GeoIterator`]/[`Nearest`] — grid point iteration and
//!   nearest-neighbour search on GRIB messages
//!
//! # Example
//!
//! ```no_run
//! use eccodes::MessageReader;
//!
//! # fn main() -> eccodes::Result<()> {
//! for handle in MessageReader::grib("data.grib2")? {
//!     let handle = handle?;
//!     let short_name: String = handle.get("shortName")?;
//!     let values: Vec<f64> = handle.get("values")?;
//!     println!("{short_name}: {} values", values.len());
//! }
//! # Ok(())
//! # }
//! ```

use eccodes_sys as sys;

mod context;
mod datetime;
mod error;
mod geo_iter;
mod handle;
mod index;
mod key;
mod keys_iter;
pub mod kind;
mod missing;
mod multi;
mod nearest;

pub use context::Context;
pub use datetime::{date_to_julian, datetime_to_julian, julian_to_date, julian_to_datetime};
pub use error::{Error, Result};
pub use geo_iter::{GeoFlags, GeoIterator, GeoPoint};
pub use handle::{
    GribMessagesInBytes, Handle, MessageReader, count_bufr_in_file, count_grib_in_file,
    count_in_file,
};
pub use index::Index;
pub use key::{Force, KeyGet, KeySet};
pub use keys_iter::{KeyFlags, KeysIterator};
pub use missing::{MISSING_DOUBLE, MISSING_LONG, is_missing_double, is_missing_long};
pub use nearest::{Nearest, NearestFlags, NearestPoint};

/// Product kind of a message, as reported by
/// [`Handle::product_kind`].
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Kind {
    /// Undetermined.
    Any,
    /// GRIB.
    Grib,
    /// BUFR.
    Bufr,
    /// METAR.
    Metar,
    /// GTS.
    Gts,
    /// TAF.
    Taf,
}

impl Kind {
    pub(crate) const fn from_sys(product: sys::ProductKind) -> Option<Self> {
        match product {
            sys::ProductKind_PRODUCT_ANY => Some(Self::Any),
            sys::ProductKind_PRODUCT_GRIB => Some(Self::Grib),
            sys::ProductKind_PRODUCT_BUFR => Some(Self::Bufr),
            sys::ProductKind_PRODUCT_METAR => Some(Self::Metar),
            sys::ProductKind_PRODUCT_GTS => Some(Self::Gts),
            sys::ProductKind_PRODUCT_TAF => Some(Self::Taf),
            _ => None,
        }
    }
}

/// Native type of a key, as reported by
/// [`Handle::native_type`] (`CODES_TYPE_*`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum NativeType {
    /// `CODES_TYPE_UNDEFINED`
    Undefined,
    /// `CODES_TYPE_LONG`
    Long,
    /// `CODES_TYPE_DOUBLE`
    Double,
    /// `CODES_TYPE_STRING`
    String,
    /// `CODES_TYPE_BYTES`
    Bytes,
    /// `CODES_TYPE_SECTION`
    Section,
    /// `CODES_TYPE_LABEL`
    Label,
    /// `CODES_TYPE_MISSING`
    Missing,
}

impl NativeType {
    #[allow(clippy::cast_sign_loss)] // negative codes simply fail to match
    pub(crate) const fn from_code(code: std::ffi::c_int) -> Option<Self> {
        match code as u32 {
            sys::CODES_TYPE_UNDEFINED => Some(Self::Undefined),
            sys::CODES_TYPE_LONG => Some(Self::Long),
            sys::CODES_TYPE_DOUBLE => Some(Self::Double),
            sys::CODES_TYPE_STRING => Some(Self::String),
            sys::CODES_TYPE_BYTES => Some(Self::Bytes),
            sys::CODES_TYPE_SECTION => Some(Self::Section),
            sys::CODES_TYPE_LABEL => Some(Self::Label),
            sys::CODES_TYPE_MISSING => Some(Self::Missing),
            _ => None,
        }
    }

    /// The raw `CODES_TYPE_*` code.
    #[allow(clippy::cast_possible_wrap)] // CODES_TYPE_* are tiny positives
    pub(crate) const fn as_code(self) -> std::ffi::c_int {
        (match self {
            Self::Undefined => sys::CODES_TYPE_UNDEFINED,
            Self::Long => sys::CODES_TYPE_LONG,
            Self::Double => sys::CODES_TYPE_DOUBLE,
            Self::String => sys::CODES_TYPE_STRING,
            Self::Bytes => sys::CODES_TYPE_BYTES,
            Self::Section => sys::CODES_TYPE_SECTION,
            Self::Label => sys::CODES_TYPE_LABEL,
            Self::Missing => sys::CODES_TYPE_MISSING,
        }) as std::ffi::c_int
    }
}

/// Version of the underlying eccodes C library, e.g. `24700` for 2.47.0
/// (`codes_get_api_version`).
#[must_use]
pub fn version() -> i64 {
    // SAFETY: no arguments, returns a plain integer.
    unsafe { sys::codes_get_api_version() }
}

/// Git SHA1 of the underlying eccodes C library.
#[must_use]
pub fn git_sha1() -> String {
    // SAFETY: returns a static NUL-terminated string, never NULL.
    unsafe { std::ffi::CStr::from_ptr(sys::codes_get_git_sha1()) }
        .to_string_lossy()
        .into_owned()
}

/// Build date of the underlying eccodes C library.
#[must_use]
pub fn build_date() -> String {
    // SAFETY: returns a static NUL-terminated string, never NULL.
    unsafe { std::ffi::CStr::from_ptr(sys::codes_get_build_date()) }
        .to_string_lossy()
        .into_owned()
}

/// Name of a native key type, e.g. `"long"` (`codes_get_type_name`).
#[must_use]
pub fn type_name(native: NativeType) -> String {
    // SAFETY: valid for any type code; returns a static NUL-terminated
    // string (a fallback for unknown codes), never NULL.
    unsafe { std::ffi::CStr::from_ptr(sys::codes_get_type_name(native.as_code())) }
        .to_string_lossy()
        .into_owned()
}
