use eccodes::{Code, GribFile, GribMessage, Library, LogLevel, Logging};

fn main() -> eccodes::Result<()> {
    // [logging]
    Library::set_logging(Logging::Rust);

    Library::set_log_handler(|level, message| {
        if level >= LogLevel::Warning {
            eprintln!("eccodes: {message}");
        }
    });
    // [end logging]

    let message = GribMessage::from_sample("GRIB2")?;

    // [errors]
    match message.get::<i64>("noSuchKey") {
        Ok(value) => println!("{value}"),
        Err(e) if e.code() == Some(Code::NotFound) => println!("{:?}: {e}", e.key()),
        Err(e) => return Err(e),
    }

    match GribFile::open("/nonexistent/data.grib2") {
        Ok(file) => println!("{}", file.count()?),
        Err(e) => println!("{:?}: {e}", e.path()),
    }
    // [end errors]
    Ok(())
}
