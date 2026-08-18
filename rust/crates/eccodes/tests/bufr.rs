//! BUFR integration tests ported from `examples/C`, run against the
//! in-repo sample messages (`samples/*.tmpl`).
//!
//! The dedicated BUFR keys iterator (`codes_bufr_keys_iterator_*`) is
//! not wrapped, so key enumeration is not covered here.

use std::path::{Path, PathBuf};

use eccodes::{AnyFile, BufrFile, BufrMessage, Kind, missing};

/// Path to an in-repo sample message; `None` outside the repo checkout.
fn sample(name: &str) -> Option<PathBuf> {
    let path = Path::new(env!("CARGO_MANIFEST_DIR"))
        .join("../../../samples")
        .join(name);
    path.exists().then_some(path)
}

/// A BUFR message carrying the WMO date sequence (301011 expands to
/// year, month, day), packed and ready to encode.
fn dated_message(year: i64, month: Option<i64>, day: Option<i64>) -> eccodes::Result<BufrMessage> {
    let mut message = BufrMessage::from_sample("BUFR4")?;
    message.set("numberOfSubsets", 1_i64)?;
    message.set("unexpandedDescriptors", &[301_011_i64][..])?;
    message.set("year", year)?;
    message.set("month", month)?;
    message.set("day", day)?;
    message.set("pack", true)?;
    Ok(message)
}

/// Port of `examples/C/bufr_read_header.c`: section-1 keys are readable
/// without unpacking, and an untagged message refuses to become a GRIB
/// one.
#[test]
fn header_without_unpack() -> eccodes::Result<()> {
    let Some(path) = sample("BUFR4.tmpl") else {
        return Ok(());
    };

    assert_eq!(BufrFile::open(&path)?.count()?, 1);

    let mut messages = BufrFile::open(&path)?.messages()?;
    let message = messages.next().expect("BUFR4.tmpl is not empty")?;
    assert!(messages.next().is_none());
    assert_eq!(message.kind()?, Kind::Bufr);

    // Header keys, no unpack needed.
    assert_eq!(message.get::<i64>("edition")?, 4);
    assert!(message.get::<i64>("numberOfSubsets")? >= 1);
    let date = message.get::<String>("typicalDate")?;
    assert_eq!(date.len(), 8, "YYYYMMDD, got {date}");
    assert!(date.bytes().all(|byte| byte.is_ascii_digit()));

    // Read untagged, the same message reports BUFR and refuses to be
    // re-tagged as GRIB — handing itself back usable, and saying what it
    // actually is. (A `BufrMessage` cannot even be asked: the conversion
    // exists only on `Message<Any>`.)
    let untagged = AnyFile::open(&path)?
        .messages()?
        .next()
        .expect("BUFR4.tmpl is not empty")?;
    let wrong = untagged
        .try_into_grib()
        .expect_err("a BUFR message is not GRIB");
    assert_eq!(wrong.expected(), Kind::Grib);
    assert_eq!(wrong.actual(), Kind::Bufr);
    assert_eq!(wrong.to_string(), "expected a GRIB message, found BUFR");
    assert_eq!(wrong.into_message().get::<i64>("edition")?, 4);
    Ok(())
}

/// BUFR encode/decode round trip in the spirit of the
/// `bufr_dump_encode_C` flow: fresh message from the `BUFR4` sample,
/// date descriptors set, values packed, serialized, decoded again.
#[test]
fn encode_decode_round_trip() -> eccodes::Result<()> {
    // Resolves through the samples path baked into the build; no file
    // path involved.
    let message = dated_message(2026, Some(8), Some(11))?;

    let bytes = message.to_vec()?;
    let mut reread = BufrMessage::from_bytes(&bytes)?;
    assert_eq!(reread.kind()?, Kind::Bufr);

    // Data keys only exist after unpacking.
    assert!(reread.get::<i64>("year").is_err());
    reread.unpack()?;
    assert_eq!(reread.get::<i64>("year")?, 2026);
    assert_eq!(reread.get::<i64>("month")?, 8);
    assert_eq!(reread.get::<i64>("day")?, 11);
    assert_eq!(reread.get::<Vec<i64>>("unexpandedDescriptors")?, [301_011]);
    Ok(())
}

/// Port of the `codes_bufr_copy_data` flow from
/// `examples/C/bufr_copy_data.c`, minus the local-table bitmap: the data
/// section of an unpacked source lands in a freshly built sibling with
/// the same descriptors.
///
/// The old wrapper checked at run time that both messages were BUFR;
/// `copy_data_from` takes a `&BufrMessage`, so passing anything else no
/// longer compiles.
#[test]
fn copy_data_between_messages() -> eccodes::Result<()> {
    let mut source = dated_message(2026, Some(8), Some(11))?;
    source.unpack()?;

    let mut destination = BufrMessage::from_sample("BUFR4")?;
    destination.set("numberOfSubsets", 1_i64)?;
    destination.set("unexpandedDescriptors", &[301_011_i64][..])?;
    destination.copy_data_from(&source)?;
    destination.set("pack", true)?;

    // Round trip the destination to prove the copied values encode.
    let mut reread = BufrMessage::from_bytes(&destination.to_vec()?)?;
    reread.unpack()?;
    assert_eq!(reread.get::<i64>("year")?, 2026);
    assert_eq!(reread.get::<i64>("day")?, 11);
    Ok(())
}

/// Port of `examples/C/bufr_missing.c`, `bufr_expanded.c` and
/// `bufr_attributes.c`: values never set encode as the missing
/// sentinel, descriptor expansion is visible after unpack, and key
/// attributes resolve through the `->` syntax.
#[test]
fn missing_expansion_and_attributes() -> eccodes::Result<()> {
    // Only the year — month and day stay unset and must encode as
    // missing (bufr_missing.c reads such a key from its fixture).
    let message = dated_message(2026, None, None)?;

    let mut reread = BufrMessage::from_bytes(&message.to_vec()?)?;
    reread.unpack()?;

    assert_eq!(reread.get::<i64>("year")?, 2026);
    // Read raw, a missing value is the library's sentinel; read as an
    // Option, it is simply absent.
    assert!(missing::is_long(reread.get::<i64>("month")?));
    assert_eq!(reread.get::<Option<i64>>("month")?, None);
    assert_eq!(reread.get::<Option<i64>>("day")?, None);

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
    let source = BufrMessage::from_sample("BUFR4")?;
    let mut clone = source.try_clone()?;
    clone.set("bufrHeaderCentre", 80_i64)?;

    assert_eq!(clone.get::<i64>("bufrHeaderCentre")?, 80);
    assert_ne!(
        source.get::<i64>("bufrHeaderCentre")?,
        clone.get::<i64>("bufrHeaderCentre")?,
        "clone mutation leaked into the original"
    );
    Ok(())
}
