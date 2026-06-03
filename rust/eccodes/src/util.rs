//! Utility functions.
//!
//! Requires `features = ["advanced"]`.

use crate::error::{EcCodesError, Result};
use std::os::raw::c_long;

/// Convert a date/time to a Julian day number.
pub fn datetime_to_julian(
    year: i64,
    month: i64,
    day: i64,
    hour: i64,
    minute: i64,
    second: i64,
) -> Result<f64> {
    let mut jd: f64 = 0.0;
    let err = unsafe {
        eccodes_sys::codes_datetime_to_julian(
            year as c_long,
            month as c_long,
            day as c_long,
            hour as c_long,
            minute as c_long,
            second as c_long,
            &mut jd,
        )
    };
    EcCodesError::check(err)?;
    Ok(jd)
}

/// Convert a Julian day number to a date/time.
pub fn julian_to_datetime(jd: f64) -> Result<(i64, i64, i64, i64, i64, i64)> {
    let mut year: c_long = 0;
    let mut month: c_long = 0;
    let mut day: c_long = 0;
    let mut hour: c_long = 0;
    let mut minute: c_long = 0;
    let mut second: c_long = 0;
    let err = unsafe {
        eccodes_sys::codes_julian_to_datetime(
            jd,
            &mut year,
            &mut month,
            &mut day,
            &mut hour,
            &mut minute,
            &mut second,
        )
    };
    EcCodesError::check(err)?;
    Ok((
        year as i64,
        month as i64,
        day as i64,
        hour as i64,
        minute as i64,
        second as i64,
    ))
}

/// Convert a packed date (YYYYMMDD) to a Julian date.
pub fn date_to_julian(date: i64) -> i64 {
    unsafe { eccodes_sys::codes_date_to_julian(date as c_long) as i64 }
}

/// Convert a Julian date to a packed date (YYYYMMDD).
pub fn julian_to_date(jdate: i64) -> i64 {
    unsafe { eccodes_sys::codes_julian_to_date(jdate as c_long) as i64 }
}
