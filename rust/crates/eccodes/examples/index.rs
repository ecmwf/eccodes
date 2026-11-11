use std::path::Path;

use eccodes::{GribMessage, Index};

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
    let path = std::env::temp_dir().join("eccodes-index.grib2");
    write_fields(&path)?;

    // [index]
    let mut index = Index::from_file(&path, ["shortName", "step"])?;
    println!(
        "{:?} {:?}",
        index.values::<String>("shortName")?,
        index.values::<i64>("step")?
    );

    index.select("shortName", "2t")?.select("step", 6_i64)?;
    for message in index.messages() {
        let message = message?;
        println!(
            "{} {}",
            message.get::<String>("shortName")?,
            message.get::<i64>("step")?
        );
    }
    // [end index]
    Ok(())
}
