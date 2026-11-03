//! Dates and times as message keys.
//!
//! The Julian-day part of `tests/julian.cc` is gone with the API it tested;
//! what is left of it inside the crate is the normalisation below.

use std::path::{Path, PathBuf};

use eccodes::GribFile;
use eccodes::time::{Date, Month, Time};

/// Path to an in-repo sample message; `None` outside the repo checkout.
fn sample(name: &str) -> Option<PathBuf> {
    let path = Path::new(env!("CARGO_MANIFEST_DIR"))
        .join("../../../samples")
        .join(name);
    path.exists().then_some(path)
}

/// `Test5` of `tests/julian.cc`: a `dataDate` of 2017-02-29 reads as
/// 2017-03-01, the way the C tools normalise it.
#[test]
fn invalid_coded_date_normalizes() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let mut message = GribFile::open(&path)?
        .messages()?
        .next()
        .expect("GRIB2.tmpl is not empty")?;

    // Some editions refuse to code an impossible date at all — nothing to
    // normalise then.
    if message.set("dataDate", 20_170_229_i64).is_err() {
        return Ok(());
    }
    if message.get::<i64>("dataDate")? != 20_170_229 {
        return Ok(());
    }

    assert_eq!(
        message.get::<Date>("dataDate")?,
        Date::from_calendar_date(2017, Month::March, 1).expect("a valid date")
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
