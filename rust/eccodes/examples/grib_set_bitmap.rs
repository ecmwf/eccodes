//! Rust equivalent of grib_set_bitmap.c
//! How to set a bitmap in a GRIB message
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let infile = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_set_bitmap <grib_in> <grib_out>");
        std::process::exit(1);
    });
    let outfile = std::env::args().nth(2).unwrap_or_else(|| {
        eprintln!("Usage: grib_set_bitmap <grib_in> <grib_out>");
        std::process::exit(1);
    });

    let mut msg = MessageIterator::from_grib_file(&infile)?.next().unwrap()?;
    let missing = 1.0e36_f64;
    msg.set("missingValue", &missing)?;

    let mut values: Vec<f64> = msg.get("values")?;
    msg.set("bitmapPresent", &1_i64)?;

    // Set first 10 values to missing
    let num_missing = 10;
    for v in values.iter_mut().take(num_missing) {
        *v = missing;
    }
    msg.set("values", values.as_slice())?;

    let ndp: i64 = msg.get("numberOfDataPoints")?;
    let ncv: i64 = msg.get("numberOfCodedValues")?;
    let nm: i64 = msg.get("numberOfMissing")?;
    assert_eq!(ndp as usize, values.len());
    assert_eq!(ncv, ndp - nm);
    assert_eq!(nm, num_missing as i64);
    println!("numberOfDataPoints={ndp} numberOfCodedValues={ncv} numberOfMissing={nm}");

    msg.write_to_file(&outfile)?;
    println!("Written to {outfile}");
    Ok(())
}
