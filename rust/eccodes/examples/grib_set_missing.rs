//! Rust equivalent of grib_set_missing.c
//! How to set a key as missing
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let infile = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_set_missing <grib_in> <grib_out>");
        std::process::exit(1);
    });
    let outfile = std::env::args().nth(2).unwrap_or_else(|| {
        eprintln!("Usage: grib_set_missing <grib_in> <grib_out>");
        std::process::exit(1);
    });

    let mut msg = MessageIterator::from_grib_file(&infile)?
        .next().unwrap()?;

    msg.set("typeOfFirstFixedSurface", "sfc")?;
    msg.set_missing("scaleFactorOfFirstFixedSurface")?;
    msg.set_missing("scaledValueOfFirstFixedSurface")?;

    let ni: i64 = msg.get("Ni")?;
    let is_missing = msg.is_missing("Ni")?;
    println!("Ni={ni}, is_missing={is_missing}");

    msg.write_to_file(&outfile)?;
    println!("Written to {outfile}");
    Ok(())
}
