//! Rust equivalent of grib_multi.c
//! How to decode multi-field GRIB2 messages
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let filename = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_multi <grib_file>");
        std::process::exit(1);
    });

    // Turn on multi-field support
    eccodes::grib_multi_support_on();

    let count = eccodes::count_messages(&filename)?;
    println!("count_messages counted {count} messages");

    let mut msg_count = 0;
    for msg in MessageIterator::from_grib_file(&filename)? {
        let msg = msg?;
        msg_count += 1;
        let discipline: i64 = msg.get("discipline")?;
        let pcat: i64 = msg.get("parameterCategory")?;
        let pnum: i64 = msg.get("parameterNumber")?;
        println!("discipline={discipline} parameterCategory={pcat} parameterNumber={pnum}");
        if discipline == 0 && pcat == 2 {
            if pnum == 2 { println!("-------- u -------"); }
            if pnum == 3 { println!("-------- v -------"); }
        }
    }
    println!("codes_handle_new_from_file counted {msg_count} messages");
    Ok(())
}
