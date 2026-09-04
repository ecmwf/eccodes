//! Port of `tests/julian.cc` — Julian <-> calendar conversions.
//!
//! Test names, values, loop bounds and epsilons match the C test
//! (`Test0` .. `Test5`) one to one.

use eccodes::{date_to_julian, datetime_to_julian, julian_to_date, julian_to_datetime};

/// `DBL_EQUAL` from the C test: relative comparison at 1e-12.
fn dbl_equal(a: f64, b: f64) -> bool {
    (a - b).abs() <= 1e-12 * a.abs()
}

/// `TestDateTime` from the C test: a datetime survives the round trip
/// through its Julian date, and the rounded Julian day survives the
/// round trip through the `YYYYMMDD` form.
fn round_trip(dt: (i64, i64, i64, i64, i64, i64)) -> eccodes::Result<()> {
    let (year, month, day, hour, minute, second) = dt;
    let jd = datetime_to_julian(year, month, day, hour, minute, second)?;
    assert_eq!(
        julian_to_datetime(jd)?,
        dt,
        "datetime -> julian {jd} -> datetime"
    );

    #[allow(clippy::cast_possible_truncation)]
    let jday = (jd + 0.5) as i64; // round up, as in the C test
    let date = julian_to_date(jday);
    assert_eq!(date_to_julian(date), jday, "julian day -> {date} -> back");
    Ok(())
}

/// `Test0` + `Test1`: the two golden datetime -> Julian pairs.
#[test]
fn golden_pairs() -> eccodes::Result<()> {
    let jd = datetime_to_julian(1801, 1, 30, 18, 26, 24)?;
    assert!(dbl_equal(jd, 2_378_891.268_333), "Test0: got {jd}");
    assert_eq!(julian_to_datetime(jd)?, (1801, 1, 30, 18, 26, 24));

    let jd = datetime_to_julian(1957, 10, 4, 19, 26, 24)?;
    assert!(dbl_equal(jd, 2_436_116.31), "Test1: got {jd}");
    assert_eq!(julian_to_datetime(jd)?, (1957, 10, 4, 19, 26, 24));
    Ok(())
}

/// `Test2`: specific Julian values survive julian -> datetime -> julian.
/// (The C test also runs the `YYYYMMDD` leg here but only prints it —
/// it cannot round-trip for the BC date at `jd = 0.0` — so it is not
/// asserted, matching the C behavior.)
#[test]
fn specific_julian_values() -> eccodes::Result<()> {
    let jds = [
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
        0.0,
    ];
    for jd in jds {
        let (year, month, day, hour, minute, second) = julian_to_datetime(jd)?;
        let back = datetime_to_julian(year, month, day, hour, minute, second)?;
        assert!(
            dbl_equal(back, jd),
            "julian {jd} -> {year}-{month}-{day} {hour}:{minute}:{second} -> {back}"
        );
    }
    Ok(())
}

/// `Test3`: the full sweep of modern datetimes, C loop bounds verbatim
/// (years 1950..=2015 by 5, days 1..=28, hours by 2, minutes by 3,
/// seconds by 5 — ~13.5M round trips).
#[test]
fn modern_sweep() -> eccodes::Result<()> {
    for year in (1950..=2015).step_by(5) {
        for month in 1..=12 {
            for day in 1..=28 {
                for hour in (1..=23).step_by(2) {
                    for minute in (1..=59).step_by(3) {
                        for second in (1..=59).step_by(5) {
                            round_trip((year, month, day, hour, minute, second))?;
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
        (1582, 9, 1, 1, 0, 0),
        (1582, 9, 2, 1, 0, 0),
        (1582, 9, 3, 1, 0, 0),
        (1582, 9, 4, 1, 0, 0),
        (1582, 9, 4, 16, 0, 0),
        (1582, 10, 1, 1, 0, 0),
        (1582, 10, 2, 1, 0, 0),
        (1582, 10, 3, 1, 0, 0),
        (1582, 10, 4, 1, 0, 0),
        (1582, 10, 15, 1, 0, 0),
        (1582, 11, 1, 1, 0, 0),
        (1582, 11, 2, 1, 0, 0),
        (1582, 11, 3, 1, 0, 0),
        (1582, 11, 4, 1, 0, 0),
        (1582, 11, 5, 1, 0, 0),
        (1582, 11, 5, 15, 0, 0),
    ];
    for dt in cases {
        round_trip(dt)?;
    }
    Ok(())
}

/// `Test5`: an invalid date (2017-02-29) must NOT survive the round
/// trip unchanged. The C test only asserts inequality; here we pin the
/// normalized result (2017-03-01) so the assertion cannot pass vacuously.
#[test]
fn invalid_date_normalizes() -> eccodes::Result<()> {
    let jd = datetime_to_julian(2017, 2, 29, 0, 0, 0)?;
    assert_eq!(
        julian_to_datetime(jd)?,
        (2017, 3, 1, 0, 0, 0),
        "2017-02-29 should normalize to 2017-03-01"
    );
    Ok(())
}
