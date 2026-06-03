//! Rust equivalent of bufr_missing.c
//! How to handle missing values in BUFR messages
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let filename = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: bufr_missing <bufr_file>");
        std::process::exit(1);
    });

    for (cnt, msg) in MessageIterator::from_bufr_file(&filename)?.enumerate() {
        let mut msg = msg?;
        println!("message: {cnt}");

        msg.set("unpack", &1_i64)?;

        let val: f64 = msg.get("relativeHumidity")?;
        if val == eccodes::MISSING_DOUBLE {
            println!("   --> value missing");
        } else {
            println!("   --> value present: {val}");
        }

        let is_missing = msg.is_missing("relativeHumidity")?;
        if is_missing {
            println!("   relativeHumidity is confirmed missing");
        }

        msg.set_missing("blockNumber")?;
    }
    Ok(())
}
