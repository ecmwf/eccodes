//! Rust equivalent of grib_iterator.c
//! How to use an iterator on lat/lon/values
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let filename = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_iterator <grib_file>");
        std::process::exit(1);
    });

    for msg in MessageIterator::from_grib_file(&filename)? {
        let msg = msg?;
        let bitmap_present: i64 = msg.get("bitmapPresent")?;
        let missing_value = 1.0e36_f64;
        if bitmap_present != 0 {
            // Not modifying missingValue here since we use the iterator
        }

        for (n, point) in msg.geo_iterator()?.enumerate() {
            print!("- {n} - lat={:.6} lon={:.6} value=", point.lat, point.lon);
            if bitmap_present != 0 && point.value == missing_value {
                println!("missing");
            } else {
                println!("{:.6}", point.value);
            }
        }
    }
    Ok(())
}
