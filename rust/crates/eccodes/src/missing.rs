//! Missing-value sentinels for `long` and `double` keys.

use eccodes_sys as sys;

/// Sentinel for a missing `long` value (`CODES_MISSING_LONG`).
#[allow(clippy::cast_lossless)] // i64::from is not const
pub const MISSING_LONG: i64 = sys::CODES_MISSING_LONG as i64;

/// Sentinel for a missing `double` value (`CODES_MISSING_DOUBLE`).
pub const MISSING_DOUBLE: f64 = sys::CODES_MISSING_DOUBLE;

/// Whether `x` is the missing-`long` sentinel.
#[must_use]
pub const fn is_missing_long(x: i64) -> bool {
    x == MISSING_LONG
}

/// Whether `x` is the missing-`double` sentinel (bitwise comparison — this
/// is a sentinel check, not an epsilon comparison).
#[must_use]
pub const fn is_missing_double(x: f64) -> bool {
    x.to_bits() == MISSING_DOUBLE.to_bits()
}
