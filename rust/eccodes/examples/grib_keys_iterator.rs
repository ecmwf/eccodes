//! Rust equivalent of grib_keys_iterator.c
//! How to iterate over all keys in a GRIB message
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let filename = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_keys_iterator <grib_file>");
        std::process::exit(1);
    });

    let namespace = "ls";

    for (i, msg) in MessageIterator::from_grib_file(&filename)?.enumerate() {
        let msg = msg?;
        println!("-- GRIB N. {} --", i + 1);
        for key_name in msg.keys_in_namespace(namespace) {
            let value: String = msg.get(&key_name).unwrap_or_default();
            println!("{key_name} = {value}");
        }
    }
    Ok(())
}
