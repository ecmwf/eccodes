//! BUFR integration tests ported from `examples/C`, run against the
//! in-repo sample messages (`samples/*.tmpl`).
//!
//! The dedicated BUFR keys iterator (`codes_bufr_keys_iterator_*`) is
//! not wrapped, so key enumeration is not covered here.

use std::path::{Path, PathBuf};

use eccodes::{Error, Handle, Kind, MessageReader, count_bufr_in_file, is_missing_long};

/// Path to an in-repo sample message; `None` outside the repo checkout.
fn sample(name: &str) -> Option<PathBuf> {
    let path = Path::new(env!("CARGO_MANIFEST_DIR"))
        .join("../../../samples")
        .join(name);
    path.exists().then_some(path)
}

/// Port of `examples/C/bufr_read_header.c`: section-1 keys are readable
/// without unpacking, and a BUFR handle refuses to become a GRIB one.
#[test]
fn header_without_unpack() -> eccodes::Result<()> {
    let Some(path) = sample("BUFR4.tmpl") else {
        return Ok(());
    };

    assert_eq!(count_bufr_in_file(&path)?, 1);

    let mut reader = MessageReader::bufr(&path)?;
    let handle = reader.next().expect("BUFR4.tmpl is not empty")?;
    assert!(reader.next().is_none());
    assert_eq!(handle.product_kind()?, Kind::Bufr);

    // Header keys, no unpack needed.
    assert_eq!(handle.get::<i64>("edition")?, 4);
    assert!(handle.get::<i64>("numberOfSubsets")? >= 1);
    let date = handle.get::<String>("typicalDate")?;
    assert_eq!(date.len(), 8, "YYYYMMDD, got {date}");
    assert!(date.bytes().all(|b| b.is_ascii_digit()));

    // try_into_grib must refuse and hand the message back usable.
    let Err(handle) = handle.try_into_grib() else {
        panic!("BUFR message retagged as GRIB")
    };
    assert_eq!(handle.get::<i64>("edition")?, 4);
    Ok(())
}

/// BUFR encode/decode round trip in the spirit of the
/// `bufr_dump_encode_C` flow: fresh message from the `BUFR4` sample,
/// date descriptors set, values packed, serialized, decoded again.
#[test]
fn encode_decode_round_trip() -> eccodes::Result<()> {
    // Resolves through the samples path baked into the build; no file
    // path involved.
    let mut msg = Handle::bufr_from_samples("BUFR4")?;
    msg.set("numberOfSubsets", 1_i64)?;
    // 301011 = (year, month, day), WMO master table only.
    msg.set("unexpandedDescriptors", &[301_011_i64][..])?;
    msg.set("year", 2026_i64)?;
    msg.set("month", 8_i64)?;
    msg.set("day", 11_i64)?;
    msg.set("pack", 1_i64)?;

    let bytes = msg.message_copy()?;
    let mut reread = Handle::from_message(&bytes)?;
    assert_eq!(reread.product_kind()?, Kind::Bufr);

    // Data keys only exist after unpacking.
    assert!(reread.get::<i64>("year").is_err());
    reread.set("unpack", 1_i64)?;
    assert_eq!(reread.get::<i64>("year")?, 2026);
    assert_eq!(reread.get::<i64>("month")?, 8);
    assert_eq!(reread.get::<i64>("day")?, 11);
    assert_eq!(reread.get::<Vec<i64>>("unexpandedDescriptors")?, [301_011]);
    Ok(())
}

/// Port of the `codes_bufr_copy_data` flow from
/// `examples/C/bufr_copy_data.c`, minus the local-table bitmap: the data
/// section of an unpacked source lands in a freshly built sibling with
/// the same descriptors. Also pins the wrapper's product-kind guard.
#[test]
fn copy_data_between_messages() -> eccodes::Result<()> {
    let mut src = Handle::bufr_from_samples("BUFR4")?;
    src.set("numberOfSubsets", 1_i64)?;
    src.set("unexpandedDescriptors", &[301_011_i64][..])?;
    src.set("year", 2026_i64)?;
    src.set("month", 8_i64)?;
    src.set("day", 11_i64)?;
    src.set("pack", 1_i64)?;
    src.set("unpack", 1_i64)?;

    let mut dst = Handle::bufr_from_samples("BUFR4")?;
    dst.set("numberOfSubsets", 1_i64)?;
    dst.set("unexpandedDescriptors", &[301_011_i64][..])?;
    dst.bufr_copy_data(&src)?;
    dst.set("pack", 1_i64)?;

    // Round trip the destination to prove the copied values encode.
    let bytes = dst.message_copy()?;
    let mut reread = Handle::from_message(&bytes)?;
    reread.set("unpack", 1_i64)?;
    assert_eq!(reread.get::<i64>("year")?, 2026);
    assert_eq!(reread.get::<i64>("day")?, 11);

    // The guard: copying from a non-BUFR message is refused before any
    // FFI happens.
    let grib = Handle::<eccodes::kind::Any>::from_samples("GRIB2")?;
    assert!(matches!(
        dst.bufr_copy_data(&grib),
        Err(Error::InvalidArgument)
    ));
    Ok(())
}

/// Port of `examples/C/bufr_missing.c`, `bufr_expanded.c` and
/// `bufr_attributes.c`: values never set encode as the missing
/// sentinel, descriptor expansion is visible after unpack, and key
/// attributes resolve through the `->` syntax.
#[test]
fn missing_expansion_and_attributes() -> eccodes::Result<()> {
    let mut msg = Handle::bufr_from_samples("BUFR4")?;
    msg.set("numberOfSubsets", 1_i64)?;
    msg.set("unexpandedDescriptors", &[301_011_i64][..])?;
    // Only the year — month and day stay unset and must encode as
    // missing (bufr_missing.c reads such a key from its fixture).
    msg.set("year", 2026_i64)?;
    msg.set("pack", 1_i64)?;

    let mut reread = Handle::from_message(&msg.message_copy()?)?;
    reread.set("unpack", 1_i64)?;

    assert_eq!(reread.get::<i64>("year")?, 2026);
    assert!(is_missing_long(reread.get::<i64>("month")?));
    assert!(is_missing_long(reread.get::<i64>("day")?));

    // bufr_expanded.c: 301011 is the WMO date sequence and expands to
    // exactly (004001, 004002, 004003).
    assert_eq!(
        reread.get::<Vec<i64>>("expandedDescriptors")?,
        [4001, 4002, 4003]
    );

    // bufr_attributes.c: attributes hang off data keys via `->`.
    assert_eq!(reread.get::<i64>("year->code")?, 4001);
    let units = reread.get::<String>("year->units")?;
    assert!(!units.is_empty());
    Ok(())
}

/// Port of `examples/C/bufr_clone.c`: clones of a BUFR message diverge
/// independently.
#[test]
fn clone_diverges() -> eccodes::Result<()> {
    let src = Handle::bufr_from_samples("BUFR4")?;
    let mut clone = src.try_clone()?;
    clone.set("bufrHeaderCentre", 80_i64)?;

    assert_eq!(clone.get::<i64>("bufrHeaderCentre")?, 80);
    assert_ne!(
        src.get::<i64>("bufrHeaderCentre")?,
        clone.get::<i64>("bufrHeaderCentre")?,
        "clone mutation leaked into the original"
    );
    Ok(())
}
