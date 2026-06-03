//! Rust equivalent of grib_set_data.c
//! How to set data values in a GRIB message
use eccodes::{Message, Result};

fn main() -> Result<()> {
    let outfile = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_set_data <grib_out>");
        std::process::exit(1);
    });

    println!("Using samples path: {}", eccodes::samples_path());

    let mut h = Message::from_grib_sample("regular_ll_pl_grib1")?;

    let values_len = h.get_size("values")?;
    let mut values = Vec::with_capacity(values_len);
    let mut d = 10e-8_f64;
    let mut e = d;
    let mut count = 1;
    for _ in 0..values_len {
        if count > 100 {
            e *= 10.0;
            count = 1;
        }
        values.push(d);
        d += e;
        count += 1;
    }

    h.set("bitsPerValue", &16_i64)?;
    h.set("values", values.as_slice())?;
    h.write_to_file(&outfile)?;
    println!("Written to {outfile}");
    Ok(())
}
