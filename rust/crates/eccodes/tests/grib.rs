//! Integration tests ported from `examples/C` — scenarios exercising
//! several low-level calls in sequence, run against the in-repo sample
//! messages (`samples/*.tmpl`) so no downloaded test data is needed.

use std::collections::HashSet;
use std::path::{Path, PathBuf};

use eccodes::kind::Grib;
use eccodes::{Handle, KeyFlags, MessageReader};

/// Path to an in-repo sample message.
///
/// `None` when not building from the repo checkout (e.g. a packaged
/// crate) — the test skips then, same convention as the sys-crate
/// version test.
fn sample(name: &str) -> Option<PathBuf> {
    let path = Path::new(env!("CARGO_MANIFEST_DIR"))
        .join("../../../samples")
        .join(name);
    path.exists().then_some(path)
}

/// Port of `examples/C/grib_keys_iterator.c`: for every message in the
/// file, walk the `ls` namespace with `ALL_KEYS | SKIP_DUPLICATES` and
/// read every yielded key as a string.
#[test]
fn keys_iterator_ls_namespace() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let mut messages = 0;
    for handle in MessageReader::grib(path)? {
        let handle = handle?;
        messages += 1;

        let mut names = HashSet::new();
        let keys =
            handle.keys_in_namespace("ls", KeyFlags::ALL_KEYS | KeyFlags::SKIP_DUPLICATES)?;
        for name in keys {
            let name = name?;
            assert!(names.insert(name.clone()), "duplicate key {name}");
            // The C example CODES_CHECKs a codes_get_string on every
            // yielded name.
            let value: String = handle.get(&name)?;
            assert!(!value.is_empty(), "empty value for key {name}");
        }

        for expected in ["edition", "centre", "gridType", "packingType"] {
            assert!(
                names.contains(expected),
                "'ls' namespace missing {expected}"
            );
        }
        assert_eq!(handle.get::<i64>("edition")?, 2);
    }
    assert_eq!(messages, 1, "GRIB2.tmpl holds exactly one message");
    Ok(())
}

/// Port of `examples/C/grib_set_keys.c` + `grib_get_keys.c`, extended
/// into a full round trip: set keys of every type, serialize the coded
/// message, decode it again and check everything survived.
#[test]
fn set_get_round_trip() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let mut handle = MessageReader::grib(path)?
        .next()
        .expect("GRIB2.tmpl is not empty")?;

    // grib_set_keys.c: centre as long, shortName as string.
    handle.set("centre", 80_i64)?;
    assert_eq!(handle.get::<i64>("centre")?, 80);
    // The C example also reads centre back as a string.
    assert!(!handle.get::<String>("centre")?.is_empty());

    handle.set("shortName", "2t")?;
    assert_eq!(handle.get::<String>("shortName")?, "2t");
    // The shortName concept resolves through the definitions to the
    // parameter keys.
    assert_eq!(handle.get::<i64>("paramId")?, 167);

    // grib_get_keys.c: grid geometry as longs/doubles, consistent with
    // the values array size.
    let n = handle.size("values")?;
    let ni = handle.get::<i64>("Ni")?;
    let nj = handle.get::<i64>("Nj")?;
    assert_eq!(usize::try_from(ni * nj).unwrap(), n);
    let lat0 = handle.get::<f64>("latitudeOfFirstGridPointInDegrees")?;
    assert!((-90.0..=90.0).contains(&lat0));

    // Set a non-constant field. 24 packed bits keep the quantization
    // error of the ramp far below the tolerance below.
    handle.set("bitsPerValue", 24_i64)?;
    let ramp: Vec<f64> = (0..n).map(|i| i as f64 * 0.5).collect();
    handle.set("values", ramp.as_slice())?;

    let decoded: Vec<f64> = handle.get("values")?;
    assert_eq!(decoded.len(), n);
    for (i, (d, r)) in decoded.iter().zip(&ramp).enumerate() {
        assert!((d - r).abs() < 1e-3, "value {i}: {d} vs {r}");
    }
    // Element access and the float path read the same coded data.
    assert_eq!(handle.get_double_element("values", 3)?, decoded[3]);
    assert_eq!(handle.get::<Vec<f32>>("values")?.len(), n);

    // grib_set_keys.c ends with codes_get_message + fwrite; re-decoding
    // the copy must reproduce the message exactly.
    let bytes = handle.message_copy()?;
    let reread = Handle::<Grib>::from_bytes(&bytes)?;
    assert_eq!(reread.get::<i64>("centre")?, 80);
    assert_eq!(reread.get::<String>("shortName")?, "2t");
    assert_eq!(reread.get::<Vec<f64>>("values")?, decoded);
    Ok(())
}
