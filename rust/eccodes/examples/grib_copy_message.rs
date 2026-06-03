//! Rust equivalent of grib_copy_message.c
//! How to copy a GRIB message via raw bytes
use eccodes::{Message, MessageIterator, Result};

fn main() -> Result<()> {
    let infile = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_copy_message <grib_in> <grib_out>");
        std::process::exit(1);
    });
    let outfile = std::env::args().nth(2).unwrap_or_else(|| {
        eprintln!("Usage: grib_copy_message <grib_in> <grib_out>");
        std::process::exit(1);
    });

    for source in MessageIterator::from_grib_file(&infile)? {
        let source = source?;
        let bytes = source.to_bytes()?;

        let mut new_msg = Message::from_bytes_copy(&bytes)?;
        new_msg.set("hour", &18_i64)?;
        new_msg.write_to_file(&outfile)?;
    }
    println!("Written to {outfile}");
    Ok(())
}
