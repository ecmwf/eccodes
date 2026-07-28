//! Julian <-> calendar date conversions.

use std::ffi::c_long;

use eccodes_sys as sys;

use crate::error::{Result, check};

/// Convert a calendar date-time to a Julian date
/// (`codes_datetime_to_julian`).
pub fn datetime_to_julian(
    year: i64,
    month: i64,
    day: i64,
    hour: i64,
    minute: i64,
    second: i64,
) -> Result<f64> {
    let mut jd = 0.0_f64;
    check!(sys::codes_datetime_to_julian(
        year,
        month,
        day,
        hour,
        minute,
        second,
        &raw mut jd,
    ))?;
    Ok(jd)
}

/// Convert a Julian date to `(year, month, day, hour, minute, second)`
/// (`codes_julian_to_datetime`).
pub fn julian_to_datetime(jd: f64) -> Result<(i64, i64, i64, i64, i64, i64)> {
    let (mut year, mut month, mut day) = (0 as c_long, 0 as c_long, 0 as c_long);
    let (mut hour, mut minute, mut second) = (0 as c_long, 0 as c_long, 0 as c_long);
    check!(sys::codes_julian_to_datetime(
        jd,
        &raw mut year,
        &raw mut month,
        &raw mut day,
        &raw mut hour,
        &raw mut minute,
        &raw mut second,
    ))?;
    Ok((year, month, day, hour, minute, second))
}

/// Convert a date in `YYYYMMDD` form to a Julian day number
/// (`codes_date_to_julian`).
#[must_use]
pub fn date_to_julian(date: i64) -> i64 {
    // SAFETY: pure numeric conversion.
    unsafe { sys::codes_date_to_julian(date) }
}

/// Convert a Julian day number to a date in `YYYYMMDD` form
/// (`codes_julian_to_date`).
#[must_use]
pub fn julian_to_date(jdate: i64) -> i64 {
    // SAFETY: pure numeric conversion.
    unsafe { sys::codes_julian_to_date(jdate) }
}
