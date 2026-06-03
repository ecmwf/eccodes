//! Rust equivalent of grib_set_keys.c
//! How to set key values in GRIB messages
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let infile = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_set_keys <grib_in> <grib_out>");
        std::process::exit(1);
    });
    let outfile = std::env::args().nth(2).unwrap_or_else(|| {
        eprintln!("Usage: grib_set_keys <grib_in> <grib_out>");
        std::process::exit(1);
    });

    let mut msg = MessageIterator::from_grib_file(&infile)?
        .next().unwrap()?;

    // Set centre as a long
    msg.set("centre", &80_i64)?;

    // Set shortName as a string
    msg.set("shortName", "fis")?;

    // Get centre back as long and string
    let centre_long: i64 = msg.get("centre")?;
    println!("centre long value={centre_long}");

    let centre_str: String = msg.get("centre")?;
    println!("centre string value={centre_str}");

    msg.write_to_file(&outfile)?;
    println!("Written to {outfile}");
    Ok(())
}
