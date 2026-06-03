//! Rust equivalent of grib_set_pv.c
//! How to set pv (vertical coordinate parameters)
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let infile = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_set_pv <grib_in> <grib_out>");
        std::process::exit(1);
    });
    let outfile = std::env::args().nth(2).unwrap_or_else(|| {
        eprintln!("Usage: grib_set_pv <grib_in> <grib_out>");
        std::process::exit(1);
    });

    let mut msg = MessageIterator::from_grib_file(&infile)?.next().unwrap()?;
    let pv = [1.0_f64, 2.0, 3.0, 4.0];

    msg.set("PVPresent", &1_i64)?;
    msg.set("pv", &pv[..])?;

    let nv: i64 = msg.get("NV")?;
    assert_eq!(nv, pv.len() as i64);
    println!("NV={nv}");

    msg.write_to_file(&outfile)?;
    println!("Written to {outfile}");
    Ok(())
}
