use eccodes::time::{Date, Month};
use eccodes::{GribMessage, missing};

fn main() -> eccodes::Result<()> {
    let path = std::env::temp_dir().join("eccodes-write.grib2");

    // [write]
    let mut message = GribMessage::from_sample("GRIB2")?;
    message.set("centre", "ecmf")?;
    message.set("shortName", "2t")?;
    message.set(
        "dataDate",
        Date::from_calendar_date(2026, Month::January, 1)?,
    )?;
    message.set("step", 6_i64)?;
    let count = message.key("values").len()?;
    message.set_values(&vec![273.15; count])?;
    message.write_to(std::fs::File::create(&path)?)?;
    // [end write]

    // [bytes]
    let bytes = message.to_vec()?;
    let copy = GribMessage::from_bytes(&bytes)?;
    println!(
        "{} {} bytes",
        copy.get::<String>("shortName")?,
        copy.byte_len()?
    );

    let mut later = message.try_clone()?;
    later.set("step", 12_i64)?;
    // [end bytes]

    // [missing]
    let mut values = message.values()?;
    values[0] = missing::F64;
    message.set("missingValue", missing::F64)?;
    message.set("bitmapPresent", true)?;
    message.set_values(&values)?;

    let present = message
        .values()?
        .iter()
        .filter(|value| !missing::is_f64(**value))
        .count();
    println!("{present} of {count} points carry data");
    // [end missing]
    Ok(())
}
