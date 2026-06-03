//! Rust equivalent of bufr_keys_iterator.c
//! How to iterate over keys in a BUFR message
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let filename = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: bufr_keys_iterator <bufr_file>");
        std::process::exit(1);
    });

    for (cnt, msg) in MessageIterator::from_bufr_file(&filename)?.enumerate() {
        let mut msg = msg?;
        println!("message: {cnt}");
        msg.set("unpack", &1_i64)?;

        for key_name in msg.keys() {
            let size = msg.get_size(&key_name).unwrap_or(0);
            if size <= 1 {
                let val: String = msg.get(&key_name).unwrap_or_default();
                println!("  {key_name}={val}");
            } else {
                println!("  {key_name}=(array of {size})");
            }
        }
    }
    Ok(())
}
