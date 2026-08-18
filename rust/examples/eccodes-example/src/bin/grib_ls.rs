//! A small `grib_ls`, written against the high-level `eccodes` crate.
//!
//! ```text
//! cargo run --bin grib_ls -- ../../samples/GRIB2.tmpl
//! ```
//!
//! Lists what each message holds, and looks up the value nearest a point.

use std::path::PathBuf;
use std::process::ExitCode;

use eccodes::time::{Date, Time};
use eccodes::{GribFile, LatLon, Library};

/// The point to sample each field at, for want of a command line option.
const SAMPLE_AT: LatLon = LatLon::new(51.5, -0.1);

fn main() -> ExitCode {
    let Some(path) = std::env::args_os().nth(1).map(PathBuf::from) else {
        eprintln!("usage: grib_ls <file.grib2>");
        return ExitCode::FAILURE;
    };

    match list(path) {
        Ok(()) => ExitCode::SUCCESS,
        // The error already says which key or file it was about.
        Err(err) => {
            eprintln!("grib_ls: {err}");
            ExitCode::FAILURE
        }
    }
}

fn list(path: PathBuf) -> eccodes::Result<()> {
    println!("eccodes {}", Library::version());

    let file = GribFile::open(path)?;
    println!("{}: {} messages", file.path().display(), file.count()?);

    for (index, message) in file.messages()?.enumerate() {
        let message = message?;

        // The `ls` namespace is what grib_ls prints by default.
        let mut summary = Vec::new();
        for key in message.keys().namespace("ls").skip_duplicates() {
            let key = key?;
            summary.push(format!("{key}={}", message.get::<String>(&key)?));
        }
        println!("\n#{} {}", index + 1, summary.join(" "));

        // Dates and times decode from their packed key forms.
        let date: Date = message.get("dataDate")?;
        let time: Time = message.get("dataTime")?;
        println!("   valid at {date} {time}");

        // A field with no values at all is a legitimate message, so the
        // extremes are optional rather than an error.
        let values = message.values()?;
        match extremes(&values) {
            Some((min, max)) => println!("   {} values, {min} .. {max}", values.len()),
            None => println!("   no values"),
        }

        // Nearest-point lookup, on the message's own geometry.
        let mut nearest = message.nearest()?;
        if let Some(closest) = nearest
            .find(SAMPLE_AT)?
            .into_iter()
            .min_by(|a, b| a.distance_km.total_cmp(&b.distance_km))
        {
            println!(
                "   at {:.2}, {:.2}: {} ({:.1} km away)",
                closest.position.lat, closest.position.lon, closest.value, closest.distance_km
            );
        }
    }
    Ok(())
}

/// The smallest and largest value, ignoring the missing ones.
fn extremes(values: &[f64]) -> Option<(f64, f64)> {
    let present = values
        .iter()
        .copied()
        .filter(|value| !eccodes::missing::is_double(*value));
    present.fold(None, |extremes, value| {
        Some(extremes.map_or((value, value), |(min, max): (f64, f64)| {
            (min.min(value), max.max(value))
        }))
    })
}
