//! The sentinels the library codes a missing value as.
//!
//! Reading a key as `Option<T>` is the ordinary way to handle missing values:
//!
//! ```no_run
//! # fn main() -> eccodes::Result<()> {
//! # let message: eccodes::GribMessage = unimplemented!();
//! let level: Option<i64> = message.get("levelist")?;   // None if missing
//! # Ok(())
//! # }
//! ```
//!
//! These constants are for the case that leaves no room for `Option`: a
//! `values` array where individual points are missing, which the message
//! codes with a bitmap and a sentinel rather than element by element.
//!
//! ```no_run
//! use eccodes::missing;
//!
//! # fn main() -> eccodes::Result<()> {
//! # let mut message: eccodes::GribMessage = unimplemented!();
//! let mut values = message.values()?;
//! values[0] = missing::F64;
//! message.set("missingValue", missing::F64)?;
//! message.set("bitmapPresent", true)?;
//! message.set_values(&values)?;
//!
//! let decoded = message.values()?;
//! let present = decoded.iter().filter(|value| !missing::is_f64(**value)).count();
//! println!("{present} points carry data");
//! # Ok(())
//! # }
//! ```

use eccodes_sys as sys;

/// The sentinel for a missing integer value (`CODES_MISSING_LONG`).
#[allow(clippy::cast_lossless)] // `i64::from` is not const
pub const I64: i64 = sys::CODES_MISSING_LONG as i64;

/// The sentinel for a missing floating-point value (`CODES_MISSING_DOUBLE`).
pub const F64: f64 = sys::CODES_MISSING_DOUBLE;

/// Whether `value` is the missing-integer sentinel.
#[must_use]
pub const fn is_i64(value: i64) -> bool {
    value == I64
}

/// Whether `value` is the missing floating-point sentinel.
///
/// Compared bit for bit: this asks whether the message coded *that* sentinel,
/// not whether a computed number happens to be near it.
#[must_use]
pub const fn is_f64(value: f64) -> bool {
    value.to_bits() == F64.to_bits()
}
