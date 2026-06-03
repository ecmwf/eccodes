//! Rust equivalent of bufr_read_header.c
//! How to read header keys from BUFR messages
use eccodes::{MessageIterator, Result};

fn main() -> Result<()> {
    let filename = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: bufr_read_header <bufr_file>");
        std::process::exit(1);
    });

    for (cnt, msg) in MessageIterator::from_bufr_file(&filename)?.enumerate() {
        let msg = msg?;
        println!("message: {cnt}");

        let data_cat: i64 = msg.get("dataCategory")?;
        println!("  dataCategory: {data_cat}");
        let data_sub: i64 = msg.get("dataSubCategory")?;
        println!("  dataSubCategory: {data_sub}");
        let typical_date: i64 = msg.get("typicalDate")?;
        println!("  typicalDate: {typical_date}");
        let centre: i64 = msg.get("bufrHeaderCentre")?;
        println!("  bufrHeaderCentre: {centre}");
        let sub_centre: i64 = msg.get("bufrHeaderSubCentre")?;
        println!("  bufrHeaderSubCentre: {sub_centre}");
        let master_tv: i64 = msg.get("masterTablesVersionNumber")?;
        println!("  masterTablesVersionNumber: {master_tv}");
        let local_tv: i64 = msg.get("localTablesVersionNumber")?;
        println!("  localTablesVersionNumber: {local_tv}");
        let num_subsets: i64 = msg.get("numberOfSubsets")?;
        println!("  numberOfSubsets: {num_subsets}");
    }
    Ok(())
}
