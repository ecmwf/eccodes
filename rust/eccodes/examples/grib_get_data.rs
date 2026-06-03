//! Rust equivalent of grib_get_data.c
//! How to get lat/lon/values from a GRIB message
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let filename = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_get_data <grib_file>");
        std::process::exit(1);
    });

    let msg = MessageIterator::from_grib_file(&filename)?.next().unwrap()?;
    let missing = 1.0e36_f64;

    let data = msg.get_data()?;
    for point in &data {
        if point.value != missing {
            println!("{:.6} {:.6} {:.6}", point.lat, point.lon, point.value);
        }
    }
    Ok(())
}
