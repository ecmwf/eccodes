//! Rust equivalent of get_product_kind.c
//! How to determine the product kind of messages in a file
use eccodes::{MessageIterator, ProductKind, Result};

fn main() -> Result<()> {
    let filename = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: get_product_kind <file>");
        std::process::exit(1);
    });

    for (i, msg) in MessageIterator::from_file(&filename, ProductKind::Any)?.enumerate() {
        let msg = msg?;
        println!("message: {i}");
        let kind: String = msg.get("kindOfProduct")?;
        println!("  product: {kind}");
    }
    Ok(())
}
