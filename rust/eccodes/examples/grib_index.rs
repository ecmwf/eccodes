//! Rust equivalent of grib_index.c
//! How to create and use an index to access GRIB messages
use eccodes::index::Index;
use eccodes::Result;

fn main() -> Result<()> {
    let inputfile = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: grib_index <grib_file>");
        std::process::exit(1);
    });

    println!("indexing...");
    let mut index = Index::new(&["shortName", "level", "step"])?;
    index.add_file(&inputfile)?;
    println!("end indexing...");

    let steps: Vec<i64> = index.get("step")?;
    println!("stepSize={}", steps.len());
    for s in &steps { print!("{s} "); }
    println!();

    let levels: Vec<i64> = index.get("level")?;
    println!("levelSize={}", levels.len());
    for l in &levels { print!("{l} "); }
    println!();

    let short_names: Vec<String> = index.get("shortName")?;
    println!("shortNameSize={}", short_names.len());
    for s in &short_names { print!("{s} "); }
    println!();

    let mut count = 0;
    for sn in &short_names {
        index.select("shortName", sn)?;
        for l in &levels {
            index.select("level", l)?;
            for s in &steps {
                index.select("step", s)?;
                while let Some(msg) = index.next_message()? {
                    count += 1;
                    let name: String = msg.get("shortName")?;
                    let level: i64 = msg.get("level")?;
                    let step: i64 = msg.get("step")?;
                    println!("shortName={name} level={level} step={step}");
                }
            }
        }
    }
    println!("  {count} messages selected");
    Ok(())
}
