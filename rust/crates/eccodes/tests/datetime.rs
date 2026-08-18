//! Port of `tests/julian.cc` — Julian <-> calendar conversions.
//!
//! Test values, loop bounds and epsilons match the C test (`Test0` ..
//! `Test5`) one to one; the dates themselves are [`time`] types, as they
//! are everywhere else in this crate.

use std::path::{Path, PathBuf};

use eccodes::time::{Date, Month, PrimitiveDateTime, Time};
use eccodes::{GribFile, JulianDay};

/// `DBL_EQUAL` from the C test: relative comparison at 1e-12.
fn dbl_equal(a: f64, b: f64) -> bool {
    (a - b).abs() <= 1e-12 * a.abs()
}

/// Path to an in-repo sample message; `None` outside the repo checkout.
fn sample(name: &str) -> Option<PathBuf> {
    let path = Path::new(env!("CARGO_MANIFEST_DIR"))
        .join("../../../samples")
        .join(name);
    path.exists().then_some(path)
}

/// A date-time from its parts, for the C test's tables.
fn datetime(year: i32, month: u8, day: u8, hour: u8, minute: u8, second: u8) -> PrimitiveDateTime {
    let month = Month::try_from(month).expect("month in range");
    PrimitiveDateTime::new(
        Date::from_calendar_date(year, month, day).expect("a date the test declares valid"),
        Time::from_hms(hour, minute, second).expect("a time the test declares valid"),
    )
}

/// `TestDateTime` from the C test: a date-time survives the round trip
/// through its Julian day.
fn round_trip(when: PrimitiveDateTime) -> eccodes::Result<()> {
    let julian = JulianDay::from_datetime(when)?;
    assert_eq!(julian.to_datetime()?, when, "{when} -> {julian} -> back");
    Ok(())
}

/// `Test0` + `Test1`: the two golden date-time -> Julian pairs.
#[test]
fn golden_pairs() -> eccodes::Result<()> {
    let when = datetime(1801, 1, 30, 18, 26, 24);
    let julian = JulianDay::from_datetime(when)?;
    assert!(
        dbl_equal(julian.0, 2_378_891.268_333),
        "Test0: got {julian}"
    );
    assert_eq!(julian.to_datetime()?, when);

    let when = datetime(1957, 10, 4, 19, 26, 24);
    let julian = JulianDay::from_datetime(when)?;
    assert!(dbl_equal(julian.0, 2_436_116.31), "Test1: got {julian}");
    assert_eq!(julian.to_datetime()?, when);
    Ok(())
}

/// `Test2`: specific Julian values survive julian -> datetime -> julian.
#[test]
fn specific_julian_values() -> eccodes::Result<()> {
    let julians = [
        2_451_545.0,
        2_446_822.5,
        2_446_966.0,
        2_447_187.5,
        2_447_332.0,
        2_415_020.5,
        2_305_447.5,
        2_305_812.5,
        2_205_512.5,
        2_026_871.8,
        // 4713 BC, the Julian epoch itself. The C test runs it through the
        // same round trip and only *prints* its YYYYMMDD leg, which cannot
        // hold a BC date — that leg is not wrapped here at all.
        0.0,
    ];
    for julian in julians.map(JulianDay) {
        let when = julian.to_datetime()?;
        let back = JulianDay::from_datetime(when)?;
        assert!(dbl_equal(back.0, julian.0), "{julian} -> {when} -> {back}");
    }
    Ok(())
}

/// `Test3`: the full sweep of modern date-times, C loop bounds verbatim
/// (years 1950..=2015 by 5, days 1..=28, hours by 2, minutes by 3,
/// seconds by 5 — ~13.5M round trips).
#[test]
fn modern_sweep() -> eccodes::Result<()> {
    for year in (1950..=2015).step_by(5) {
        for month in 1..=12_u8 {
            for day in 1..=28_u8 {
                for hour in (1..=23_u8).step_by(2) {
                    for minute in (1..=59_u8).step_by(3) {
                        for second in (1..=59_u8).step_by(5) {
                            round_trip(datetime(year, month, day, hour, minute, second))?;
                        }
                    }
                }
            }
        }
    }
    Ok(())
}

/// `Test4`: dates around the Julian/Gregorian calendar transition of
/// October 1582 (the nonexistent 1582-10-05 .. 1582-10-14 are skipped,
/// as in the C test).
#[test]
fn gregorian_transition() -> eccodes::Result<()> {
    let cases = [
        (1582, 9, 1, 1),
        (1582, 9, 2, 1),
        (1582, 9, 3, 1),
        (1582, 9, 4, 1),
        (1582, 9, 4, 16),
        (1582, 10, 1, 1),
        (1582, 10, 2, 1),
        (1582, 10, 3, 1),
        (1582, 10, 4, 1),
        (1582, 10, 15, 1),
        (1582, 11, 1, 1),
        (1582, 11, 2, 1),
        (1582, 11, 3, 1),
        (1582, 11, 4, 1),
        (1582, 11, 5, 1),
        (1582, 11, 5, 15),
    ];
    for (year, month, day, hour) in cases {
        round_trip(datetime(year, month, day, hour, 0, 0))?;
    }
    Ok(())
}

/// The library's Julian day and [`time`]'s differ by a fixed offset for
/// modern dates.
///
/// Both are consulted by this crate — the C library converts instants,
/// `time` converts whole days — so a drift between them would silently
/// shift dates. The offset is derived from one reference date rather
/// than hard-coded, so the test pins the *agreement*, not a constant.
#[test]
fn julian_day_agrees_with_the_time_crate() -> eccodes::Result<()> {
    let offset_at = |when: PrimitiveDateTime| -> eccodes::Result<i64> {
        let julian = JulianDay::from_datetime(when)?;
        // Midnight sits half a day before the Julian day boundary, the
        // rounding the C test applies too.
        #[allow(clippy::cast_possible_truncation)]
        let day_number = (julian.0 + 0.5) as i64;
        Ok(day_number - i64::from(when.date().to_julian_day()))
    };

    let reference = offset_at(datetime(2000, 1, 1, 0, 0, 0))?;
    for year in (1583..=2100).step_by(7) {
        for (month, day) in [(1_u8, 1_u8), (2, 28), (7, 15), (12, 31)] {
            let when = datetime(year, month, day, 0, 0, 0);
            assert_eq!(
                offset_at(when)?,
                reference,
                "{when}: eccodes and time disagree about the Julian day"
            );
        }
    }
    Ok(())
}

/// `Test5`: an invalid date must not survive a round trip unchanged.
///
/// A `time::Date` cannot hold 2017-02-29 at all, so the case now arises
/// where it actually reaches users: a message whose `dataDate` codes a
/// date that does not exist. Reading it as a `Date` normalises the way
/// the C tools do, to 2017-03-01.
#[test]
fn invalid_coded_date_normalizes() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let mut message = GribFile::open(&path)?
        .messages()?
        .next()
        .expect("GRIB2.tmpl is not empty")?;

    // Some editions refuse to code an impossible date at all, which is a
    // fine outcome — there is then nothing to normalise.
    if message.set("dataDate", 20_170_229_i64).is_err() {
        return Ok(());
    }
    if message.get::<i64>("dataDate")? != 20_170_229 {
        return Ok(());
    }

    assert_eq!(
        message.get::<Date>("dataDate")?,
        Date::from_calendar_date(2017, Month::March, 1).expect("a valid date"),
        "2017-02-29 should normalize to 2017-03-01"
    );
    Ok(())
}

/// Dates and times read and write through their packed key forms.
#[test]
fn date_and_time_keys_round_trip() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let mut message = GribFile::open(&path)?
        .messages()?
        .next()
        .expect("GRIB2.tmpl is not empty")?;

    let date = Date::from_calendar_date(2026, Month::August, 18).expect("a valid date");
    let noon = Time::from_hms(12, 0, 0).expect("a valid time");
    message.set("dataDate", date)?;
    message.set("dataTime", noon)?;

    assert_eq!(message.get::<Date>("dataDate")?, date);
    assert_eq!(message.get::<Time>("dataTime")?, noon);
    // The packed forms are what the message actually holds.
    assert_eq!(message.get::<i64>("dataDate")?, 20_260_818);
    assert_eq!(message.get::<i64>("dataTime")?, 1200);
    Ok(())
}
