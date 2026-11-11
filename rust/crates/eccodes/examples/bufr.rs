use eccodes::BufrMessage;

fn main() -> eccodes::Result<()> {
    // [encode]
    let mut message = BufrMessage::from_sample("BUFR4")?;
    message.set("numberOfSubsets", 1_i64)?;
    message.set("unexpandedDescriptors", &[301_011_i64][..])?;
    message.set("year", 2026_i64)?;
    message.set("month", 8_i64)?;
    message.set("day", 11_i64)?;
    message.set("pack", true)?;
    let bytes = message.to_vec()?;
    // [end encode]

    // [decode]
    let mut reread = BufrMessage::from_bytes(&bytes)?;
    println!("edition {}", reread.get::<i64>("edition")?);

    reread.unpack()?;
    println!(
        "{}-{}-{}",
        reread.get::<i64>("year")?,
        reread.get::<i64>("month")?,
        reread.get::<i64>("day")?
    );
    println!("{:?}", reread.get::<Vec<i64>>("expandedDescriptors")?);
    println!("{}", reread.get::<String>("year->units")?);
    // [end decode]
    Ok(())
}
