//! Dates and times, as [`time`] types.
//!
//! Message keys code dates and times as integers — `dataDate` as `20170301`,
//! `dataTime` as `1200`. Ask for a [`Date`] or a [`Time`] and the packing is
//! handled for you:
//!
//! ```no_run
//! use time::{Date, Time};
//!
//! # fn main() -> eccodes::Result<()> {
//! # let mut message: eccodes::GribMessage = unimplemented!();
//! let date: Date = message.get("dataDate")?;
//! let time: Time = message.get("dataTime")?;
//! println!("{date} {time}");
//!
//! message.set("dataDate", Date::from_calendar_date(2017, time::Month::March, 1)?)?;
//! # Ok(())
//! # }
//! ```
//!
//! A key is a [`Date`] or a [`Time`]; no date-time type of this crate's own
//! reaches the API. The library's Julian day stays inside, where it does one
//! job — normalising a date a message codes out of range.

use eccodes_sys as sys;
use time::{Date, Month, Time};

use crate::error::{Code, Error, ErrorContext, Result};
use crate::key::{KeyGet, KeySet};
use crate::kind::MessageKind;
use crate::message::Message;

impl KeyGet for Date {
    /// Reads the `YYYYMMDD` form of `dataDate`, `validityDate` and their kin.
    ///
    /// A date the message codes out of range — 30 February, say — is
    /// normalised the way the C tools normalise it, through a Julian-day
    /// round trip, rather than rejected.
    fn get_from<K: MessageKind>(message: &Message<K>, key: &str) -> Result<Self> {
        let packed: i64 = message.get(key)?;
        from_yyyymmdd(packed).with_key(key)
    }
}

impl KeySet for Date {
    /// Writes the `YYYYMMDD` form.
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()> {
        message.set(key, to_yyyymmdd(self))
    }
}

impl KeyGet for Time {
    /// Reads the `HHMM` form of `dataTime`, `validityTime` and their kin.
    fn get_from<K: MessageKind>(message: &Message<K>, key: &str) -> Result<Self> {
        let packed: i64 = message.get(key)?;
        from_hhmm(packed).with_key(key)
    }
}

impl KeySet for Time {
    /// Writes the `HHMM` form, dropping seconds — which these keys cannot
    /// carry.
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()> {
        message.set(key, to_hhmm(self))
    }
}

/// Unpack a `YYYYMMDD` date, normalising one the library would normalise.
fn from_yyyymmdd(packed: i64) -> Result<Date> {
    if let Ok(date) = split_date(packed) {
        return Ok(date);
    }
    // Out of range as coded. The C library resolves such dates through its
    // Julian-day arithmetic — 20170230 comes back as 20170301 — and its tools
    // read messages that way, so a wrapper that rejected them would disagree
    // with `grib_ls` about what a file says.
    // SAFETY: pure numeric conversions, no pointers involved.
    let normalised = unsafe { sys::codes_julian_to_date(sys::codes_date_to_julian(packed)) };
    split_date(normalised)
}

/// Pack a date into the `YYYYMMDD` form a message key expects.
fn to_yyyymmdd(date: Date) -> i64 {
    i64::from(date.year()) * 10_000
        + i64::from(u8::from(date.month())) * 100
        + i64::from(date.day())
}

/// Unpack an `HHMM` time.
fn from_hhmm(packed: i64) -> Result<Time> {
    build_time(packed / 100, packed % 100, 0)
}

/// Pack a time into the `HHMM` form a message key expects.
fn to_hhmm(time: Time) -> i64 {
    i64::from(time.hour()) * 100 + i64::from(time.minute())
}

fn split_date(packed: i64) -> Result<Date> {
    build_date(packed / 10_000, (packed / 100) % 100, packed % 100)
}

fn build_date(year: i64, month: i64, day: i64) -> Result<Date> {
    let invalid = || Error::from(Code::InvalidKeyValue);
    let year = i32::try_from(year).map_err(|_| invalid())?;
    let month = u8::try_from(month).map_err(|_| invalid())?;
    let day = u8::try_from(day).map_err(|_| invalid())?;
    Ok(Date::from_calendar_date(
        year,
        Month::try_from(month)?,
        day,
    )?)
}

fn build_time(hour: i64, minute: i64, second: i64) -> Result<Time> {
    let invalid = || Error::from(Code::InvalidKeyValue);
    let hour = u8::try_from(hour).map_err(|_| invalid())?;
    let minute = u8::try_from(minute).map_err(|_| invalid())?;
    let second = u8::try_from(second).map_err(|_| invalid())?;
    Ok(Time::from_hms(hour, minute, second)?)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn packed_dates_round_trip() {
        // `dataDate` is read as an integer and written back as one, so the
        // unpacking has to survive the round trip exactly.
        let date = from_yyyymmdd(20_170_301).expect("a valid date");
        assert_eq!(
            date,
            Date::from_calendar_date(2017, Month::March, 1).expect("a valid date")
        );
        assert_eq!(to_yyyymmdd(date), 20_170_301);
    }

    #[test]
    fn packed_times_round_trip_and_drop_seconds() {
        // `dataTime` has no room for seconds — packing one back must not
        // smear it into the minutes.
        let noon = from_hhmm(1200).expect("a valid time");
        assert_eq!(noon, Time::from_hms(12, 0, 0).expect("a valid time"));
        assert_eq!(
            to_hhmm(Time::from_hms(12, 0, 30).expect("a valid time")),
            1200
        );
        assert_eq!(
            from_hhmm(30).expect("a valid time"),
            Time::from_hms(0, 30, 0).expect("a valid time")
        );
    }

    #[test]
    fn impossible_times_are_refused() {
        // Unlike dates, times have no normalising round trip in the library:
        // an hour of 99 is a broken message, not a different instant.
        assert_eq!(
            from_hhmm(9900).expect_err("hour out of range").code(),
            Some(Code::InvalidKeyValue)
        );
    }
}
