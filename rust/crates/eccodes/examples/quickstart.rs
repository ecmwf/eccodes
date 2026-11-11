use eccodes::{GribFile, GribMessage};

// [main]
fn main() -> eccodes::Result<()> {
    let mut message = GribMessage::from_sample("GRIB2")?;
    message.set("shortName", "2t")?;
    message.set("step", 6_i64)?;
    let count = message.key("values").len()?;
    message.set_values(&vec![273.15; count])?;

    let path = std::env::temp_dir().join("eccodes-quickstart.grib2");
    message.write_to(std::fs::File::create(&path)?)?;

    let file = GribFile::open(&path)?;
    println!("{}: {} message(s)", file.path().display(), file.count()?);
    for message in &file {
        let message = message?;
        let name: String = message.get("shortName")?;
        let step: i64 = message.get("step")?;
        println!("{name} at step {step}: {} values", message.values()?.len());
    }
    Ok(())
}
// [end main]
