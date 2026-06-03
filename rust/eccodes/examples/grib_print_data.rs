//! Rust equivalent of grib_print_data.c
//! Print all data values from a GRIB message
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let filename = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_print_data <grib_file>");
        std::process::exit(1);
    });

    let msg = MessageIterator::from_grib_file(&filename)?
        .next().unwrap()?;

    let values: Vec<f64> = msg.get("values")?;
    for (i, v) in values.iter().enumerate() {
        println!("{i}  {v:.10e}");
    }

    let max: f64 = msg.get("max")?;
    let min: f64 = msg.get("min")?;
    let avg: f64 = msg.get("average")?;
    println!("{} values found in {filename}", values.len());
    println!("max={max:.10e} min={min:.10e} average={avg:.10e}");
    Ok(())
}
