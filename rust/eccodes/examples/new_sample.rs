//! Rust equivalent of new_sample.c (simplified)
//! How to create a GRIB from sample and set various keys
use eccodes::{Message, Result};

fn main() -> Result<()> {
    let outfile = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: new_sample <grib_out>");
        std::process::exit(1);
    });

    // Try to create from nonexistent sample - should fail
    assert!(Message::from_sample("nonexistentsample").is_err());

    let mut h = Message::from_sample("GRIB2")?;

    h.set("centre", &98_i64)?;
    h.set("subCentre", &0_i64)?;
    h.set("tablesVersion", &4_i64)?;
    h.set("year", &2007_i64)?;
    h.set("month", &3_i64)?;
    h.set("day", &23_i64)?;
    h.set("hour", &12_i64)?;
    h.set("minute", &0_i64)?;
    h.set("second", &0_i64)?;
    h.set("Ni", &16_i64)?;
    h.set("Nj", &31_i64)?;

    let values = vec![1.0_f64; 496];
    h.set("values", values.as_slice())?;

    h.write_to_file(&outfile)?;
    println!("Written to {outfile}");
    Ok(())
}
