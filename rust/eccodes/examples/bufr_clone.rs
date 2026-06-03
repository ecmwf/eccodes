//! Rust equivalent of bufr_clone.c
//! How to clone BUFR messages
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let infile = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: bufr_clone <bufr_in> <bufr_out>");
        std::process::exit(1);
    });
    let outfile = std::env::args().nth(2).unwrap_or_else(|| {
        eprintln!("Usage: bufr_clone <bufr_in> <bufr_out>");
        std::process::exit(1);
    });

    let mut first = true;
    for source in MessageIterator::from_bufr_file(&infile)? {
        let source = source?;
        for i in 0..3_i64 {
            let mut cloned = source.clone();
            cloned.set("typicalDay", &(20 + i))?;
            if first {
                cloned.write_to_file(&outfile)?;
                first = false;
            } else {
                cloned.append_to_file(&outfile)?;
            }
        }
    }
    println!("Written to {outfile}");
    Ok(())
}
