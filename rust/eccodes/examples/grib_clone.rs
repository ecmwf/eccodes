//! Rust equivalent of grib_clone.c
//! How to clone a GRIB message and write modified copies
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let infile = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_clone <grib_in> <grib_out>");
        std::process::exit(1);
    });
    let outfile = std::env::args().nth(2).unwrap_or_else(|| {
        eprintln!("Usage: grib_clone <grib_in> <grib_out>");
        std::process::exit(1);
    });

    for source in MessageIterator::from_grib_file(&infile)? {
        let source = source?;
        for i in 0..3 {
            let mut clone = source.clone();
            clone.set("day", &(20_i64 + i))?;
            if i == 0 {
                clone.write_to_file(&outfile)?;
            } else {
                clone.append_to_file(&outfile)?;
            }
        }
    }
    println!("Written 3 cloned messages to {outfile}");
    Ok(())
}
