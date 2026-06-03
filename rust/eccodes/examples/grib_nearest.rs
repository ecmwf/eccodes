//! Rust equivalent of grib_nearest.c (simplified)
//! How to find nearest grid points
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let filename = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_nearest <grib_file>");
        std::process::exit(1);
    });

    let msg = MessageIterator::from_grib_file(&filename)?.next().unwrap()?;
    let mut nearest = msg.nearest()?;
    let lat = 40.0;
    let lon = 15.0;

    let points = nearest.find(lat, lon)?;
    println!("Searching for nearest to lat={lat} lon={lon}");
    println!("Idx\tlat\tlon\tdist\tval");
    for p in &points {
        println!("{}\t{:.2}\t{:.2}\t{:.4}\t{:.4}", p.index, p.lat, p.lon, p.distance_km, p.value);
    }
    Ok(())
}
