//! Rust equivalent of bufr_get_keys.c
//! How to read keys from BUFR messages
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let filename = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: bufr_get_keys <bufr_file>");
        std::process::exit(1);
    });

    for (cnt, msg) in MessageIterator::from_bufr_file(&filename)?.enumerate() {
        let mut msg = msg?;
        println!("message: {cnt}");

        // Unpack data
        msg.set("unpack", &1_i64)?;

        let type_desc: String = msg.get("typicalDate").unwrap_or_default();
        println!("  typicalDate: {type_desc}");

        let lat: f64 = msg.get("latitude").unwrap_or(0.0);
        let lon: f64 = msg.get("longitude").unwrap_or(0.0);
        println!("  latitude: {lat}");
        println!("  longitude: {lon}");

        let num_subsets: i64 = msg.get("numberOfSubsets").unwrap_or(0);
        println!("  numberOfSubsets: {num_subsets}");
    }
    Ok(())
}
