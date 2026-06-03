//! Rust equivalent of grib_precision.c
//! How to control decimal precision when packing
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let infile = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_precision <grib_in>");
        std::process::exit(1);
    });

    let mut msg = MessageIterator::from_grib_file(&infile)?.next().unwrap()?;

    let bpv1: i64 = msg.get("bitsPerValue")?;
    let values1: Vec<f64> = msg.get("values")?;

    // Change decimal precision to 2
    msg.set("changeDecimalPrecision", &2_i64)?;

    let bpv2: i64 = msg.get("bitsPerValue")?;
    let values2: Vec<f64> = msg.get("values")?;

    println!("bitsPerValue before={bpv1} after={bpv2}");

    let mut max_abs_err = 0.0_f64;
    let mut max_rel_err = 0.0_f64;
    let (mut minv, mut maxv) = (values2[0], values2[0]);
    for i in 0..values1.len() {
        let a = (values2[i] - values1[i]).abs();
        if values2[i] > maxv { maxv = values2[i]; }
        if values2[i] < minv { minv = values2[i]; }
        if values2[i] != 0.0 {
            let r = ((values2[i] - values1[i]) / values2[i]).abs();
            if r > max_rel_err { max_rel_err = r; }
        }
        if a > max_abs_err { max_abs_err = a; }
    }
    println!("max absolute error = {max_abs_err}");
    println!("max relative error = {max_rel_err}");
    println!("min value = {minv}");
    println!("max value = {maxv}");
    Ok(())
}
