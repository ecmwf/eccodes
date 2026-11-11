use eccodes::{GribFile, GribMessage, GribMultiField};

fn main() -> eccodes::Result<()> {
    let path = std::env::temp_dir().join("eccodes-multi.grib2");

    // [multi]
    let mut multi = GribMultiField::new()?;
    for step in [0_i64, 6, 12] {
        let mut field = GribMessage::from_sample("GRIB2")?;
        field.set("step", step)?;
        multi.push(&field)?;
    }
    multi.write_to(std::fs::File::create(&path)?)?;

    println!("{} message", GribFile::open(&path)?.count()?);

    let fields = GribFile::open(&path)?.multi_field(true);
    println!("{} fields", fields.count()?);
    for field in &fields {
        println!("step {}", field?.get::<i64>("step")?);
    }
    // [end multi]
    Ok(())
}
