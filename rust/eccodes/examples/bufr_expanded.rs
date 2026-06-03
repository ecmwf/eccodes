//! Rust equivalent of bufr_expanded.c
//! How to read all expanded data values from BUFR
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let filename = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: bufr_expanded <bufr_file>");
        std::process::exit(1);
    });

    for (cnt, msg) in MessageIterator::from_bufr_file(&filename)?.enumerate() {
        let msg = msg?;
        let values_len = msg.get_size("numericValues")?;
        println!("message {cnt}: number of expanded values: {values_len}");

        let values: Vec<f64> = msg.get("numericValues")?;
        for v in &values {
            println!("  {v:.10e}");
        }
    }
    Ok(())
}
