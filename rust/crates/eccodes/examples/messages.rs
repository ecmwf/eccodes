use std::io::BufReader;
use std::path::Path;

use eccodes::time::Date;
use eccodes::{GribFile, GribMessage};

fn write_fields(path: &Path) -> eccodes::Result<()> {
    let mut out = std::fs::File::create(path)?;
    for (name, step) in [("2t", 0_i64), ("2t", 6), ("msl", 0)] {
        let mut message = GribMessage::from_sample("GRIB2")?;
        message.set("shortName", name)?;
        message.set("step", step)?;
        message.write_to(&mut out)?;
    }
    Ok(())
}

fn main() -> eccodes::Result<()> {
    let path = std::env::temp_dir().join("eccodes-messages.grib2");
    write_fields(&path)?;

    // [read]
    let file = GribFile::open(&path)?;
    println!("{} messages", file.count()?);
    for message in &file {
        let message = message?;
        println!(
            "{} {}",
            message.get::<String>("shortName")?,
            message.get::<i64>("step")?
        );
    }

    let reader = BufReader::new(std::fs::File::open(&path)?);
    for message in GribFile::messages_from(reader) {
        println!("{} bytes", message?.byte_len()?);
    }
    // [end read]

    // [keys]
    let message = GribMessage::from_sample("GRIB2")?;
    let edition: i64 = message.get("edition")?;
    let name: String = message.get("shortName")?;
    let level: Option<i64> = message.get("level")?;
    let date: Date = message.get("dataDate")?;
    println!("edition {edition}, {name}, level {level:?}, {date}");

    let values = message.key("values");
    println!(
        "{} {:?} {}",
        values.exists(),
        values.value_type()?,
        values.len()?
    );

    for key in message.keys().namespace("ls") {
        let key = key?;
        let value: String = message.get(&key)?;
        println!("{key} = {value}");
    }
    // [end keys]
    Ok(())
}
