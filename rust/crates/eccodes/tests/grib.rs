//! Integration tests ported from `examples/C` — scenarios exercising
//! several calls in sequence, run against the in-repo sample messages
//! (`samples/*.tmpl`) so no downloaded test data is needed.

// Test data: loop indices cast to f64 stay far below 2^52, lengths cast
// to i64 far below 2^63, and float assertions compare bit-for-bit copies.
#![allow(
    clippy::cast_precision_loss,
    clippy::cast_possible_wrap,
    clippy::float_cmp
)]

use std::collections::HashSet;
use std::path::{Path, PathBuf};

use eccodes::{
    AnyFile, BufrFile, Code, GribFile, GribMessage, GribMultiField, Index, Kind, LatLon, Message,
    Messages, Reuse,
};

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

/// The first message of a sample file.
fn first_message(path: &Path) -> eccodes::Result<GribMessage> {
    GribFile::open(path)?
        .messages()?
        .next()
        .expect("the sample holds a message")
}

/// A ramp `i * 0.5` over the message's grid, packed at 24 bits so the
/// quantization error stays far below the tolerances used below.
fn set_ramp(message: &mut GribMessage) -> eccodes::Result<Vec<f64>> {
    let count = message.key_len("values")?;
    let ramp: Vec<f64> = (0..count).map(|i| i as f64 * 0.5).collect();
    message.set("bitsPerValue", 24_i64)?;
    message.set_values(&ramp)?;
    Ok(ramp)
}

/// Port of `examples/C/grib_keys_iterator.c`: for every message in the
/// file, walk the `ls` namespace once per name and read every yielded key
/// as a string.
#[test]
fn keys_query_ls_namespace() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let file = GribFile::open(&path)?;
    let mut messages = 0;
    for message in &file {
        let message = message?;
        messages += 1;

        let mut names = HashSet::new();
        for name in message.keys().namespace("ls").skip_duplicates() {
            let name = name?;
            assert!(names.insert(name.clone()), "duplicate key {name}");
            // The C example CODES_CHECKs a codes_get_string on every
            // yielded name.
            let value: String = message.get(&name)?;
            assert!(!value.is_empty(), "empty value for key {name}");
        }

        for expected in ["edition", "centre", "gridType", "packingType"] {
            assert!(
                names.contains(expected),
                "'ls' namespace missing {expected}"
            );
        }
        assert_eq!(message.get::<i64>("edition")?, 2);
    }
    assert_eq!(messages, 1, "GRIB2.tmpl holds exactly one message");
    // The file is a path, not a spent stream: it reads again, and counts.
    assert_eq!(file.count()?, 1);
    assert_eq!(file.into_iter().count(), 1);
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

    let mut message = first_message(&path)?;

    // grib_set_keys.c: centre as long, shortName as string.
    message.set("centre", 80_i64)?;
    assert_eq!(message.get::<i64>("centre")?, 80);
    // The C example also reads centre back as a string.
    assert!(!message.get::<String>("centre")?.is_empty());

    message.set("shortName", "2t")?;
    assert_eq!(message.get::<String>("shortName")?, "2t");
    // The shortName concept resolves through the definitions to the
    // parameter keys.
    assert_eq!(message.get::<i64>("paramId")?, 167);

    // grib_get_keys.c: grid geometry as longs/doubles, consistent with
    // the values array size.
    let count = message.key_len("values")?;
    let ni = message.get::<i64>("Ni")?;
    let nj = message.get::<i64>("Nj")?;
    assert_eq!(
        usize::try_from(ni * nj).expect("Ni*Nj is non-negative"),
        count
    );
    let lat0 = message.get::<f64>("latitudeOfFirstGridPointInDegrees")?;
    assert!((-90.0..=90.0).contains(&lat0));

    let ramp = set_ramp(&mut message)?;
    let decoded = message.values()?;
    assert_eq!(decoded.len(), count);
    for (i, (decoded, ramp)) in decoded.iter().zip(&ramp).enumerate() {
        assert!(
            (decoded - ramp).abs() < 1e-3,
            "value {i}: {decoded} vs {ramp}"
        );
    }
    // Element access and the float path read the same coded data.
    assert_eq!(message.element::<f64>("values", 3)?, decoded[3]);
    assert_eq!(
        message.elements::<f64>("values", &[0, 3])?,
        [decoded[0], decoded[3]]
    );
    assert_eq!(message.get::<Vec<f32>>("values")?.len(), count);

    // grib_set_keys.c ends with codes_get_message + fwrite; re-decoding
    // the copy must reproduce the message exactly.
    let bytes = message.to_vec()?;
    let reread = GribMessage::from_bytes(&bytes)?;
    assert_eq!(reread.get::<i64>("centre")?, 80);
    assert_eq!(reread.get::<String>("shortName")?, "2t");
    assert_eq!(reread.values()?, decoded);

    // A message tagged with the wrong product refuses its own bytes.
    let as_bufr = eccodes::BufrMessage::from_bytes(&bytes);
    assert_eq!(
        as_bufr.expect_err("GRIB bytes are not BUFR").code(),
        Some(Code::InvalidMessage)
    );
    Ok(())
}

/// Port of `examples/C/grib_iterator.c`, with the bitmap built in-test
/// the way `grib_set_bitmap.c` does — the sample has none, and the
/// missing-value branch of the C example is the part worth covering.
#[test]
fn grid_iteration_and_missing_values() -> eccodes::Result<()> {
    const MISSING: f64 = 1.0e36;

    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let mut message = first_message(&path)?;
    assert!(!message.get::<bool>("bitmapPresent")?);

    let count = message.key_len("values")?;
    set_ramp(&mut message)?;

    // Full pass: as many points as the grid declares, positions in
    // range, values identical to the values key (same coded data,
    // same scanning order on this regular_ll grid).
    let decoded = message.values()?;
    let points: Vec<_> = message.grid_points()?.collect();
    assert_eq!(
        points.len(),
        usize::try_from(message.get::<i64>("numberOfDataPoints")?)
            .expect("numberOfDataPoints is non-negative")
    );
    for point in &points {
        assert!((-90.0..=90.0).contains(&point.position.lat));
        assert!((0.0..=360.0).contains(&point.position.lon));
    }
    let from_grid: Vec<f64> = points.iter().map(|point| point.value).collect();
    assert_eq!(from_grid, decoded);

    // The positions-only walk skips decoding the data section: same
    // geometry, and no value field to misread as data.
    let positions: Vec<LatLon> = message.grid_positions()?.collect();
    assert_eq!(positions.len(), points.len());
    assert_eq!(positions[0], points[0].position);

    // reset() restarts from the first point. Scoped: the iterator
    // borrows the message, which is mutated again below.
    {
        let mut grid = message.grid_points()?;
        let first = grid.next().expect("grid has a first point");
        grid.next().expect("grid has a second point");
        grid.reset()?;
        assert_eq!(grid.next().expect("first point after reset"), first);
    }

    // grib_set_bitmap.c: declare a missing value, enable the bitmap and
    // re-encode with two points missing; the grid must yield the missing
    // value exactly at those points.
    message.set("missingValue", MISSING)?;
    message.set("bitmapPresent", true)?;
    let mut holed: Vec<f64> = (0..count).map(|i| i as f64 * 0.5).collect();
    holed[0] = MISSING;
    holed[count / 2] = MISSING;
    message.set_values(&holed)?;

    assert!(message.get::<bool>("bitmapPresent")?);
    let missing: Vec<usize> = message
        .grid_points()?
        .enumerate()
        .filter(|(_, point)| point.value == MISSING)
        .map(|(i, _)| i)
        .collect();
    assert_eq!(missing, [0, count / 2]);
    Ok(())
}

/// Port of `examples/C/grib_nearest.c` + `grib_nearest_multiple.c` (the
/// fieldset part is not wrapped). Query points are taken from the grid
/// itself, so nothing here depends on the sample's geometry.
#[test]
fn nearest_point_search() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let mut message = first_message(&path)?;
    // Known values: value at grid index i is i * 0.5.
    let count = message.key_len("values")?;
    set_ramp(&mut message)?;
    let decoded = message.values()?;

    // A mid-grid point, slightly perturbed, must come back as the
    // nearest of the four candidates.
    let target = message
        .grid_points()?
        .nth(count / 3)
        .expect("mid-grid point exists")
        .position;
    let query = LatLon::new(target.lat + 0.01, target.lon + 0.01);

    let mut nearest = message.nearest()?;
    let found = nearest.find(query)?;

    for point in &found {
        assert!(point.index < count, "index {} out of range", point.index);
        assert_eq!(point.value, decoded[point.index], "value/index mismatch");
    }
    let best = found
        .iter()
        .min_by(|a, b| a.distance_km.total_cmp(&b.distance_km))
        .expect("four candidates");
    assert_eq!(best.index, count / 3, "perturbed query must snap back");
    assert!(
        best.distance_km < 5.0,
        "0.01 deg is ~1 km, got {} km",
        best.distance_km
    );

    // Same query, telling the library what has not changed: it must
    // reuse its state and agree.
    let again = nearest.find_reusing(query, Reuse::SAME_GRID | Reuse::SAME_POINT)?;
    assert_eq!(again, found);

    // grib_nearest_multiple.c: exact grid coordinates snap to exactly
    // those points with (near) zero distance. The old API took two
    // parallel arrays whose lengths could disagree; one slice of points
    // cannot.
    let queries: Vec<LatLon> = message.grid_positions()?.take(2).collect();
    let each = nearest.find_each(&queries)?;
    assert_eq!(each.len(), 2);
    for (i, point) in each.iter().enumerate() {
        assert_eq!(point.index, i);
        assert_eq!(point.value, decoded[i]);
        assert!(
            point.distance_km < 1e-3,
            "exact hit, got {} km",
            point.distance_km
        );
    }
    Ok(())
}

/// Download `name` from the ECMWF test-data server into a cache under
/// the target dir, once. `None` (with a note on stderr) when the network
/// is not cooperating — callers skip, like the `CMake` suite's optional
/// downloaded fixtures.
fn fetch(name: &str) -> Option<PathBuf> {
    let dir = Path::new(env!("CARGO_TARGET_TMPDIR")).join("eccodes-test-data");
    let path = dir.join(name);
    if path.exists() {
        return Some(path);
    }
    std::fs::create_dir_all(&dir).ok()?;
    let url = format!("https://get.ecmwf.int/repository/test-data/eccodes/data/{name}");
    let mut response = match ureq::get(&url).call() {
        Ok(response) => response,
        Err(err) => {
            eprintln!("skipping: cannot fetch {url}: {err}");
            return None;
        }
    };
    let bytes = match response.body_mut().read_to_vec() {
        Ok(bytes) => bytes,
        Err(err) => {
            eprintln!("skipping: cannot read {url}: {err}");
            return None;
        }
    };
    // Parallel tests may race on the cache entry; the rename makes the
    // final write atomic.
    let tmp = dir.join(format!("{name}.{}.part", std::process::id()));
    std::fs::write(&tmp, bytes).ok()?;
    std::fs::rename(&tmp, &path).ok()?;
    Some(path)
}

/// Port of `examples/C/grib_index.c`, hermetic variant: the input file
/// is written in-test, so the distinct-value lists and per-selection
/// counts are known exactly — including a combination with zero
/// matches, which the C harness cannot assert.
#[test]
fn index_select_and_count() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    // Three of the four (shortName, step) combinations; the fourth must
    // select zero messages.
    let combos = [("2t", 0_i64), ("2t", 6), ("msl", 0)];
    let file_path = Path::new(env!("CARGO_TARGET_TMPDIR")).join("index_input.grib2");
    {
        let mut file = std::fs::File::create(&file_path)?;
        for (short_name, step) in combos {
            let mut message = first_message(&path)?;
            message.set("shortName", short_name)?;
            message.set("step", step)?;
            message.write_to(&mut file)?;
        }
    }

    // grib_index.c builds an empty index and adds the file to it.
    let mut index = Index::new(["shortName", "step"])?;
    index.add_file(&file_path)?;

    // Distinct values come back in ascending order.
    let short_names = index.values::<String>("shortName")?;
    let steps = index.values::<i64>("step")?;
    assert_eq!(short_names, ["2t", "msl"]);
    assert_eq!(steps, [0, 6]);
    assert_eq!(index.value_count("shortName")?, 2);

    // The C example's nested select loops, counting matches per
    // combination and checking each matched message really has the
    // selected key values.
    let mut counts = Vec::new();
    for short_name in &short_names {
        for &step in &steps {
            index
                .select("shortName", short_name)?
                .select("step", step)?;
            let mut count = 0;
            for message in index.messages() {
                let message = message?;
                assert_eq!(&message.get::<String>("shortName")?, short_name);
                assert_eq!(message.get::<i64>("step")?, step);
                count += 1;
            }
            counts.push(count);
        }
    }
    // Combination order (2t,0), (2t,6), (msl,0), (msl,6) — the last
    // was never written.
    assert_eq!(counts, [1, 1, 1, 0]);

    // Save/reload the index and select through the reloaded copy.
    let index_path = Path::new(env!("CARGO_TARGET_TMPDIR")).join("index_input.idx");
    index.save(&index_path)?;
    let mut reread = Index::open(&index_path)?;
    reread.select("shortName", "2t")?.select("step", 6_i64)?;
    let mut matched = reread.messages();
    let message = matched.next().expect("(2t, 6) was written")?;
    assert_eq!(message.get::<i64>("step")?, 6);
    assert!(matched.next().is_none());

    // A file knows how to index itself.
    let from_file = AnyFile::open(&file_path)?.index(["shortName"])?;
    assert_eq!(from_file.value_count("shortName")?, 2);
    Ok(())
}

/// Port of `examples/C/grib_index.c` on its real fixture when the
/// network allows: the harness greps for "43 messages selected" on
/// `tigge_cf_ecmwf.grib2`.
#[test]
fn index_tigge_golden_count() -> eccodes::Result<()> {
    let Some(path) = fetch("tigge_cf_ecmwf.grib2") else {
        return Ok(());
    };

    let mut index = Index::new(["shortName", "level", "number", "step"])?;
    index.add_file(&path)?;

    let short_names = index.values::<String>("shortName")?;
    let levels = index.values::<i64>("level")?;
    let numbers = index.values::<i64>("number")?;
    let steps = index.values::<i64>("step")?;

    let mut selected = 0;
    for short_name in &short_names {
        for &level in &levels {
            for &number in &numbers {
                for &step in &steps {
                    index
                        .select("shortName", short_name)?
                        .select("level", level)?
                        .select("number", number)?
                        .select("step", step)?;
                    selected += index.messages().count();
                }
            }
        }
    }
    assert_eq!(selected, 43, "grib_index.sh: '43 messages selected'");
    Ok(())
}

/// A multi-field GRIB2 message with one field per step, written to `name` in
/// the target dir — the input `grib_multi.c` reads back field by field.
fn write_multi_field(sample: &Path, steps: &[i64], name: &str) -> eccodes::Result<PathBuf> {
    let mut source = first_message(sample)?;
    let mut multi = GribMultiField::new()?;
    for &step in steps {
        source.set("step", step)?;
        multi.push(&source)?;
    }
    let path = Path::new(env!("CARGO_TARGET_TMPDIR")).join(name);
    std::fs::write(&path, multi.to_vec()?)?;
    Ok(path)
}

/// Port of `examples/C/grib_multi_write.c` (build a multi-field GRIB2
/// message by appending from section 4 per step) and `grib_multi.c`
/// (read it back field by field).
#[test]
fn multi_field_write_and_read() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };
    assert_eq!(
        first_message(&path)?.get::<i64>("edition")?,
        2,
        "multi-field needs GRIB2"
    );

    // grib_multi_write.c: one field per step, sections 4-8 repeated.
    let steps: Vec<i64> = (12..=120).step_by(12).collect();
    let file_path = write_multi_field(&path, &steps, "multi_step.grib2")?;

    // The file frames all of that as one message, and a plain read sees
    // exactly that: one message, keyed by its first field.
    let plain: Vec<_> = GribFile::open(&file_path)?
        .messages()?
        .collect::<eccodes::Result<_>>()?;
    assert_eq!(plain.len(), 1);
    assert_eq!(plain[0].get::<i64>("step")?, steps[0]);
    assert_eq!(GribFile::open(&file_path)?.count()?, 1);
    assert_eq!(AnyFile::open(&file_path)?.count()?, 1);

    // grib_multi.c: asked for fields, the same file reads as one message per
    // field — and counting agrees with iterating, which is the whole point of
    // counting.
    let fields = GribFile::open(&file_path)?.multi_field(true);
    let read_steps: Vec<i64> = fields
        .messages()?
        .map(|message| message.and_then(|message| message.get::<i64>("step")))
        .collect::<eccodes::Result<_>>()?;
    assert_eq!(read_steps, steps);
    assert_eq!(fields.count()?, steps.len());

    // Offsets are where messages start, so they report the framing either
    // way: one message, whatever the reader was asked for.
    assert_eq!(fields.offsets()?.len(), 1);
    assert_eq!(GribFile::open(&file_path)?.offsets()?.len(), 1);
    Ok(())
}

/// Multi-field decoding belongs to the reader that asked for it: the C
/// library keeps it in a switch on its global context, which a writer sets
/// for itself and a fields read needs on only while it decodes.
#[test]
fn multi_field_belongs_to_its_reader() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };
    let steps = [12_i64, 24, 36];
    let multi_path = write_multi_field(&path, &steps, "multi_reader.grib2")?;

    // `codes_grib_multi_handle_new` switches the C library into multi-field
    // mode for itself. Left that way, every GRIB count in the process fails.
    let writer = GribMultiField::new()?;
    assert_eq!(GribFile::open(&path)?.count()?, 1);
    assert_eq!(GribFile::open(&multi_path)?.count()?, 1);
    drop(writer);

    // A fields reader stopped between two fields keeps its place, and the
    // readers around it see the file framed as it is on disk.
    let fields = GribFile::open(&multi_path)?.multi_field(true);
    let mut reading = fields.messages()?;
    let first = reading.next().expect("a first field")?;
    assert_eq!(first.get::<i64>("step")?, steps[0]);

    assert_eq!(GribFile::open(&multi_path)?.count()?, 1);
    assert_eq!(GribFile::open(&multi_path)?.messages()?.count(), 1);

    let rest: Vec<i64> = reading
        .map(|message| message.and_then(|message| message.get::<i64>("step")))
        .collect::<eccodes::Result<_>>()?;
    assert_eq!(rest, &steps[1..]);
    Ok(())
}

/// A fields reader that stops between two fields leaves the rest of the
/// message with the C library, filed under the stream it was reading. That
/// stream is about to close, and its address is about to be handed to
/// somebody else — who must still read their own file (GRIB-249).
#[test]
fn an_abandoned_fields_reader_leaves_nothing_behind() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };
    let abandoned_path = write_multi_field(&path, &[12, 24, 36], "multi_abandoned.grib2")?;
    let next_steps = [100_i64, 200, 300];
    let next_path = write_multi_field(&path, &next_steps, "multi_next.grib2")?;

    // Repeated, because whether the new stream lands on the freed one's
    // address is the C runtime's business, not ours.
    for _ in 0..8 {
        let mut giving_up = GribFile::open(&abandoned_path)?
            .multi_field(true)
            .messages()?;
        let first = giving_up.next().expect("a first field")?;
        assert_eq!(first.get::<i64>("step")?, 12);
        drop(giving_up);

        let read: Vec<i64> = GribFile::open(&next_path)?
            .multi_field(true)
            .messages()?
            .map(|message| message.and_then(|message| message.get::<i64>("step")))
            .collect::<eccodes::Result<_>>()?;
        assert_eq!(
            read, next_steps,
            "a fresh reader read someone else's fields"
        );
    }
    Ok(())
}

/// The same, under contention: reading one file field by field must not
/// change what another thread reads or counts.
///
/// While multi-field support was a process-wide switch, this is what broke —
/// a plain `count` overlapping a fields read came back
/// [`Code::NotImplemented`], because the C counter refuses a GRIB file while
/// the switch is on.
#[test]
fn fields_do_not_disturb_other_threads() -> eccodes::Result<()> {
    const ROUNDS: usize = 40;

    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };
    let steps = [12_i64, 24, 36];
    let multi_path = write_multi_field(&path, &steps, "multi_threads.grib2")?;

    std::thread::scope(|threads| {
        for _ in 0..4 {
            threads.spawn(|| {
                for _ in 0..ROUNDS {
                    let file = GribFile::open(&path).expect("the sample opens");
                    assert_eq!(file.count().expect("a plain count is never refused"), 1);
                    assert_eq!(file.messages().expect("the sample reads").count(), 1);
                }
            });
        }
        threads.spawn(|| {
            let fields = GribFile::open(&multi_path)
                .expect("the multi-field file opens")
                .multi_field(true);
            for _ in 0..ROUNDS {
                let read: Vec<i64> = fields
                    .messages()
                    .expect("the multi-field file reads")
                    .map(|message| message.and_then(|message| message.get::<i64>("step")))
                    .collect::<eccodes::Result<_>>()
                    .expect("every field decodes");
                assert_eq!(read, steps);
            }
        });
    });
    Ok(())
}

/// Port of `examples/C/grib_clone.c`, `grib_copy_keys.c` and
/// `grib_copy_message.c`: clones diverge independently, namespaces copy
/// between messages, and the borrowed message view re-decodes.
#[test]
fn clone_copy_and_message_bytes() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let source = {
        let mut source = first_message(&path)?;
        source.set("centre", 80_i64)?;
        source.set("shortName", "2t")?;
        // A non-trivial data section, so the headers-only size
        // comparison below is meaningful (the template's is ~empty).
        set_ramp(&mut source)?;
        source
    };

    // grib_clone.c: mutating the clone must not touch the original.
    let mut clone = source.try_clone()?;
    clone.set("shortName", "msl")?;
    assert_eq!(clone.get::<String>("shortName")?, "msl");
    assert_eq!(source.get::<String>("shortName")?, "2t");

    // Headers-only clone keeps the metadata but drops the data section.
    let headers = source.try_clone_headers_only()?;
    assert_eq!(headers.get::<i64>("centre")?, 80);
    assert!(headers.byte_len()? < source.byte_len()?);

    // grib_copy_keys.c: pull the `ls` namespace of `source` into a fresh
    // message with different values.
    let mut destination = first_message(&path)?;
    assert_ne!(destination.get::<i64>("centre")?, 80);
    destination.copy_namespace_from(&source, "ls")?;
    assert_eq!(destination.get::<i64>("centre")?, 80);
    assert_eq!(destination.get::<String>("shortName")?, "2t");

    // grib_copy_message.c: the borrowed message view re-decodes into an
    // equal message.
    let view = source.as_bytes()?;
    let reread = Message::from_bytes(view)?;
    assert_eq!(reread.get::<String>("shortName")?, "2t");
    assert_eq!(reread.byte_len()?, source.byte_len()?);
    // Untagged, it still knows what it holds — and re-tags on request.
    assert_eq!(reread.kind()?, Kind::Grib);
    assert!(reread.try_into_grib().is_ok());
    Ok(())
}

/// Port of `examples/C/grib_get_data.c`, `grib_set_missing.c`,
/// `grib_set_pv.c` and `grib_precision.c`: the flat lat/lon/values
/// accessor, missing surface keys, vertical-coordinate arrays and
/// decimal-precision re-encoding.
#[test]
fn data_points_missing_pv_precision() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let mut message = first_message(&path)?;
    let count = message.key_len("values")?;
    set_ramp(&mut message)?;

    // grib_get_data.c: the bulk accessor agrees with the grid iterator.
    let data = message.data_points()?;
    let points: Vec<_> = message.grid_points()?.collect();
    assert_eq!(data.len(), count);
    assert_eq!(data, points);

    // grib_set_missing.c: surface-level scale keys can be set missing
    // and report it — as a predicate, or by reading them as Option.
    message.set("typeOfFirstFixedSurface", "sfc")?;
    message.set_missing("scaleFactorOfFirstFixedSurface")?;
    message.set("scaledValueOfFirstFixedSurface", None::<i64>)?;
    assert!(message.is_missing("scaleFactorOfFirstFixedSurface")?);
    assert_eq!(
        message.get::<Option<i64>>("scaledValueOfFirstFixedSurface")?,
        None
    );
    // A key that is not there at all reads the same way.
    assert_eq!(message.get::<Option<i64>>("thisKeyDoesNotExist")?, None);
    assert!(!message.contains_key("thisKeyDoesNotExist"));

    // grib_set_pv.c: vertical-coordinate array round trips through its
    // own key.
    let pv = [1.0_f64, 2.0, 3.0, 4.0];
    message.set("PVPresent", true)?;
    message.set("pv", &pv[..])?;
    assert_eq!(message.get::<Vec<f64>>("pv")?, pv);
    assert_eq!(message.get::<i64>("NV")?, pv.len() as i64);

    // grib_precision.c: forcing 2 decimal places re-encodes the field
    // exactly at that precision (the ramp has 1 decimal, so values
    // survive unchanged).
    message.set("setDecimalPrecision", 2_i64)?;
    let rounded = message.values()?;
    for (i, (rounded, point)) in rounded.iter().zip(&points).enumerate() {
        assert!(
            (rounded - point.value).abs() < 1e-9,
            "value {i}: {rounded} vs {}",
            point.value
        );
    }
    Ok(())
}

/// Port of `tests/grib_codedValues_as_bytes.cc`: the packed data
/// section moves between messages as raw bytes, and `set_forced`
/// re-packs values through the read-only `codedValues` key.
#[test]
fn coded_values_bytes_and_forced_write() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let mut source = first_message(&path)?;
    let count = set_ramp(&mut source)?.len();
    let decoded = source.values()?;

    // The coded data as raw bytes: 24 bits per value, within section 7.
    let bytes: Vec<u8> = source.get("codedValues")?;
    assert!(bytes.len() >= count * 3, "{} < {}", bytes.len(), count * 3);
    let section7 =
        usize::try_from(source.get::<i64>("section7Length")?).expect("section length fits");
    assert!(bytes.len() <= section7);

    // Overwrite a clone's field through the forced path (codedValues is
    // read-only for the plain set), then transplant the original bytes
    // back. The reversed ramp has the same min/max, so the packing
    // parameters (reference value, scale, width) stay identical and the
    // raw bytes remain compatible — all zeros would collapse to a 0-bit
    // constant field.
    let mut clone = source.try_clone()?;
    let reversed: Vec<f64> = decoded.iter().rev().copied().collect();
    clone.set_forced("codedValues", &reversed)?;
    assert_eq!(clone.values()?, reversed);

    // Writing it without the forced path is refused, not silently ignored.
    assert!(clone.set("codedValues", reversed.as_slice()).is_err());

    clone.set("codedValues", bytes.as_slice())?;
    assert_eq!(clone.values()?, decoded);
    Ok(())
}

/// Port of `tests/extract_offsets.cc` + `examples/C/get_product_kind.c`:
/// per-product counting on a mixed GRIB/BUFR file, the product each
/// message holds, and the GRIB-only byte-buffer reader.
#[test]
fn mixed_products_counting() -> eccodes::Result<()> {
    let (Some(grib_path), Some(bufr_path)) = (sample("GRIB2.tmpl"), sample("BUFR4.tmpl")) else {
        return Ok(());
    };

    // GRIB + BUFR + GRIB in one file.
    let grib_bytes = std::fs::read(&grib_path)?;
    let bufr_bytes = std::fs::read(&bufr_path)?;
    let mixed_path = Path::new(env!("CARGO_TARGET_TMPDIR")).join("mixed_products.bin");
    let mut mixed = grib_bytes.clone();
    mixed.extend_from_slice(&bufr_bytes);
    mixed.extend_from_slice(&grib_bytes);
    std::fs::write(&mixed_path, &mixed)?;

    assert_eq!(AnyFile::open(&mixed_path)?.count()?, 3);
    assert_eq!(GribFile::open(&mixed_path)?.count()?, 2);
    assert_eq!(BufrFile::open(&mixed_path)?.count()?, 1);
    // Counting a product the file does not hold is zero, not an error.
    assert_eq!(BufrFile::open(&grib_path)?.count()?, 0);

    // Every message starts where the offsets say it does.
    let offsets = AnyFile::open(&mixed_path)?.offsets()?;
    assert_eq!(offsets.len(), 3);
    assert_eq!(offsets[0], 0);
    assert_eq!(offsets[1], grib_bytes.len() as u64);
    for (message, offset) in AnyFile::open(&mixed_path)?.messages()?.zip(&offsets) {
        assert_eq!(message?.file_offset()?, *offset);
    }

    // get_product_kind.c: each message reports the product it holds —
    // from its own framing, not from how the file was opened.
    let kinds: Vec<Kind> = AnyFile::open(&mixed_path)?
        .messages()?
        .map(|message| message.and_then(|message| message.kind()))
        .collect::<eccodes::Result<_>>()?;
    assert_eq!(kinds, [Kind::Grib, Kind::Bufr, Kind::Grib]);

    // The byte-buffer reader walks consecutive GRIB messages.
    let mut two_gribs = grib_bytes.clone();
    two_gribs.extend_from_slice(&grib_bytes);
    let editions: Vec<i64> = Messages::from_bytes(&two_gribs)
        .map(|message| message.and_then(|message| message.get::<i64>("edition")))
        .collect::<eccodes::Result<_>>()?;
    assert_eq!(editions, [2, 2]);
    Ok(())
}

/// `count` reports zero for a file that holds no message of its product —
/// the C call reports that as an error, and the crate maps it. This pins the
/// other half: a file that is broken rather than empty must still fail.
#[test]
fn counting_tells_an_empty_file_from_a_broken_one() -> eccodes::Result<()> {
    let (Some(grib_path), Some(bufr_path)) = (sample("GRIB2.tmpl"), sample("BUFR4.tmpl")) else {
        return Ok(());
    };
    let bytes = std::fs::read(&grib_path)?;
    let dir = Path::new(env!("CARGO_TARGET_TMPDIR"));

    // No GRIB message in sight: zero, not an error.
    assert_eq!(GribFile::open(&bufr_path)?.count()?, 0);
    assert!(GribFile::open(&bufr_path)?.offsets()?.is_empty());
    let empty_path = dir.join("empty.grib2");
    std::fs::write(&empty_path, b"")?;
    assert_eq!(GribFile::open(&empty_path)?.count()?, 0);

    // Half a message is not "no messages": the file claims a length it does
    // not have, and that must not read as an empty file.
    let truncated_path = dir.join("truncated.grib2");
    std::fs::write(&truncated_path, &bytes[..bytes.len() / 2])?;
    let truncated = GribFile::open(&truncated_path)?;
    let err = truncated
        .count()
        .expect_err("a truncated message is not an empty file");
    assert_eq!(err.code(), Some(Code::PrematureEndOfFile));
    assert_eq!(err.path(), Some(truncated_path.as_path()));
    assert_eq!(
        truncated
            .offsets()
            .expect_err("offsets fail where the count does")
            .code(),
        Some(Code::PrematureEndOfFile)
    );

    // Nor is a message whose 7777 terminator has been overwritten.
    let corrupt_path = dir.join("corrupt.grib2");
    let mut corrupt = bytes;
    let tail = corrupt.len() - 4;
    corrupt[tail..].copy_from_slice(b"XXXX");
    std::fs::write(&corrupt_path, &corrupt)?;
    assert_eq!(
        GribFile::open(&corrupt_path)?
            .count()
            .expect_err("a message that never ends is not an empty file")
            .code(),
        Some(Code::WrongLength)
    );
    Ok(())
}

/// Errors name the key or the file they happened on, and keep the code
/// callers match against.
#[test]
fn errors_carry_their_context() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let message = first_message(&path)?;
    let err = message
        .get::<i64>("noSuchKey")
        .expect_err("the key does not exist");
    assert_eq!(err.code(), Some(Code::NotFound));
    assert_eq!(err.key(), Some("noSuchKey"));
    assert!(err.to_string().contains("noSuchKey"), "{err}");

    let missing_file =
        GribFile::open("/nonexistent/eccodes-test.grib2").expect_err("the file does not exist");
    assert_eq!(
        missing_file.path(),
        Some(Path::new("/nonexistent/eccodes-test.grib2"))
    );
    assert!(missing_file.io_error().is_some());
    Ok(())
}
