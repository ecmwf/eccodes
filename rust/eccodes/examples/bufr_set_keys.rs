//! Rust equivalent of bufr_set_keys.c
//! How to set key values in BUFR messages
use eccodes::{Message, Result};

fn main() -> Result<()> {
    let outfile = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: bufr_set_keys <bufr_out>");
        std::process::exit(1);
    });

    let mut msg = Message::from_bufr_sample("BUFR4")?;

    msg.set("unpack", &1_i64)?;
    msg.set("typicalYear", &2015_i64)?;
    msg.set("typicalMonth", &8_i64)?;
    msg.set("typicalDay", &21_i64)?;

    msg.set("pack", &1_i64)?;

    msg.write_to_file(&outfile)?;
    println!("Written to {outfile}");
    Ok(())
}
