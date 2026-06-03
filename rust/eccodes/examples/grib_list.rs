//! Rust equivalent of grib_list.c
//! How to get list/array values using keys
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let filename = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_list <grib_file>");
        std::process::exit(1);
    });

    let mut msg = MessageIterator::from_grib_file(&filename)?.next().unwrap()?;

    let nb: i64 = msg.get("numberOfContributingSpectralBands")?;
    println!("numberOfContributingSpectralBands={nb}");

    // Expand NB to 9
    msg.set("numberOfContributingSpectralBands", &9_i64)?;
    let nb: i64 = msg.get("numberOfContributingSpectralBands")?;
    println!("numberOfContributingSpectralBands={nb}");

    let count = msg.get_size("scaledValueOfCentralWaveNumber")?;
    println!("count={count}");

    let values: Vec<i64> = msg.get("scaledValueOfCentralWaveNumber")?;
    for (i, v) in values.iter().enumerate() {
        if *v == eccodes::MISSING_LONG {
            println!("scaledValueOfCentralWaveNumber {i} = MISSING");
        } else {
            println!("scaledValueOfCentralWaveNumber {i} = {v}");
        }
    }

    // Set new values
    let new_vals: Vec<i64> = (0..count as i64).map(|i| i + 1000).collect();
    msg.set("scaledValueOfCentralWaveNumber", new_vals.as_slice())?;

    let check: Vec<i64> = msg.get("scaledValueOfCentralWaveNumber")?;
    for (i, v) in check.iter().enumerate() {
        println!("Now scaledValueOfCentralWaveNumber {i} = {v}");
    }
    Ok(())
}
