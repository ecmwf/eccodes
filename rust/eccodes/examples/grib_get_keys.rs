//! Rust equivalent of grib_get_keys.c
//! How to get values of different types from a GRIB message
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let filename = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_get_keys <grib_file>");
        std::process::exit(1);
    });

    for message in MessageIterator::from_grib_file(&filename)? {
        let message = message?;
        let lat_first: f64 = message.get("latitudeOfFirstGridPointInDegrees")?;
        let lon_first: f64 = message.get("longitudeOfFirstGridPointInDegrees")?;
        let lat_last: f64 = message.get("latitudeOfLastGridPointInDegrees")?;
        let lon_last: f64 = message.get("longitudeOfLastGridPointInDegrees")?;
        let di: f64 = message.get("jDirectionIncrementInDegrees")?;
        let dj: f64 = message.get("iDirectionIncrementInDegrees")?;
        let num_points: i64 = message.get("numberOfPoints")?;
        let avg: f64 = message.get("average")?;
        let packing: String = message.get("packingType")?;

        println!("latitudeOfFirstGridPointInDegrees={lat_first}");
        println!("longitudeOfFirstGridPointInDegrees={lon_first}");
        println!("latitudeOfLastGridPointInDegrees={lat_last}");
        println!("longitudeOfLastGridPointInDegrees={lon_last}");
        println!("jDirectionIncrementInDegrees={di}");
        println!("iDirectionIncrementInDegrees={dj}");
        println!("numberOfPoints={num_points}");
        println!("packingType={packing}");
        println!("average={avg}");
    }
    Ok(())
}
