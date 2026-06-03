//! Rust equivalent of grib_multi_write.c
//! How to encode GRIB2 multi-field messages
use eccodes::{MessageIterator, Result};
use eccodes::multi_handle::MultiHandle;

fn main() -> Result<()> {
    let infile = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_multi_write <grib2_in> <grib_out>");
        std::process::exit(1);
    });
    let outfile = std::env::args().nth(2).unwrap_or_else(|| {
        eprintln!("Usage: grib_multi_write <grib2_in> <grib_out>");
        std::process::exit(1);
    });

    let mut msg = MessageIterator::from_grib_file(&infile)?.next().unwrap()?;
    let edition: i64 = msg.get("edition")?;
    if edition != 2 {
        eprintln!("ERROR: Input grib must be edition 2 for multi-fields");
        std::process::exit(1);
    }

    let mut mh = MultiHandle::new()?;
    let start_section = 4;

    for step in (12..=120).step_by(12) {
        msg.set("step", &(step as i64))?;
        mh.append(&mut msg, start_section)?;
    }

    mh.write(&outfile)?;
    println!("Written multi-field message to {outfile}");
    Ok(())
}
