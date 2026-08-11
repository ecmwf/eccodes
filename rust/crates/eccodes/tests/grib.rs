//! Integration tests ported from `examples/C` — scenarios exercising
//! several low-level calls in sequence, run against the in-repo sample
//! messages (`samples/*.tmpl`) so no downloaded test data is needed.

use std::collections::HashSet;
use std::path::{Path, PathBuf};

use eccodes::kind::{Grib, GribMulti};
use eccodes::{
    Context, GeoFlags, Handle, Index, KeyFlags, MessageReader, NearestFlags, count_in_file,
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
    assert_eq!(usize::try_from(ni * nj).expect("Ni*Nj is non-negative"), n);
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

/// Port of `examples/C/grib_iterator.c`, with the bitmap built in-test
/// the way `grib_set_bitmap.c` does — the sample has none, and the
/// missing-value branch of the C example is the part worth covering.
#[test]
fn geo_iterator_grid_and_missing() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let mut handle = MessageReader::grib(path)?
        .next()
        .expect("GRIB2.tmpl is not empty")?;
    assert_eq!(handle.get::<i64>("bitmapPresent")?, 0);

    let n = handle.size("values")?;
    handle.set("bitsPerValue", 24_i64)?;
    handle.set(
        "values",
        (0..n)
            .map(|i| i as f64 * 0.5)
            .collect::<Vec<_>>()
            .as_slice(),
    )?;

    // Full pass: as many points as the grid declares, positions in
    // range, values identical to the values key (same coded data,
    // same scanning order on this regular_ll grid).
    let decoded: Vec<f64> = handle.get("values")?;
    let points: Vec<_> = handle.geo_iter(GeoFlags::empty())?.collect();
    assert_eq!(
        points.len(),
        usize::try_from(handle.get::<i64>("numberOfDataPoints")?)
            .expect("numberOfDataPoints is non-negative")
    );
    for p in &points {
        assert!((-90.0..=90.0).contains(&p.lat), "lat {}", p.lat);
        assert!((0.0..=360.0).contains(&p.lon), "lon {}", p.lon);
    }
    let iter_values: Vec<f64> = points.iter().map(|p| p.value).collect();
    assert_eq!(iter_values, decoded);

    // NO_VALUES skips decoding: same geometry, values pinned to 0.
    let fast: Vec<_> = handle.geo_iter(GeoFlags::NO_VALUES)?.collect();
    assert_eq!(fast.len(), points.len());
    assert!(fast.iter().all(|p| p.value == 0.0));
    assert_eq!(fast[0].lat, points[0].lat);

    // reset() restarts from the first point. Scoped: the iterator
    // borrows the handle, which is mutated again below.
    {
        let mut iter = handle.geo_iter(GeoFlags::empty())?;
        let first = iter.next().expect("grid has a first point");
        iter.next().expect("grid has a second point");
        iter.reset()?;
        assert_eq!(iter.next().expect("first point after reset"), first);
    }

    // grib_set_bitmap.c: declare a missing value, enable the bitmap and
    // re-encode with two points missing; the iterator must yield the
    // missing value exactly at those points.
    const MISSING: f64 = 1.0e36;
    handle.set("missingValue", MISSING)?;
    handle.set("bitmapPresent", 1_i64)?;
    let mut holed: Vec<f64> = (0..n).map(|i| i as f64 * 0.5).collect();
    holed[0] = MISSING;
    holed[n / 2] = MISSING;
    handle.set("values", holed.as_slice())?;

    assert_eq!(handle.get::<i64>("bitmapPresent")?, 1);
    let missing: Vec<usize> = handle
        .geo_iter(GeoFlags::empty())?
        .enumerate()
        .filter(|(_, p)| p.value == MISSING)
        .map(|(i, _)| i)
        .collect();
    assert_eq!(missing, [0, n / 2]);
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

    let mut handle = MessageReader::grib(path)?
        .next()
        .expect("GRIB2.tmpl is not empty")?;

    // Known values: value at grid index i is i * 0.5.
    let n = handle.size("values")?;
    handle.set("bitsPerValue", 24_i64)?;
    handle.set(
        "values",
        (0..n)
            .map(|i| i as f64 * 0.5)
            .collect::<Vec<_>>()
            .as_slice(),
    )?;
    let decoded: Vec<f64> = handle.get("values")?;

    // A mid-grid point, slightly perturbed, must come back as the
    // nearest of the four candidates.
    let target = handle
        .geo_iter(GeoFlags::empty())?
        .nth(n / 3)
        .expect("mid-grid point exists");

    let mut nearest = handle.nearest()?;
    let found = nearest.find(target.lat + 0.01, target.lon + 0.01, NearestFlags::empty())?;

    for p in &found {
        assert!(p.index < n, "index {} out of range", p.index);
        assert_eq!(p.value, decoded[p.index], "value/index mismatch");
    }
    let best = found
        .iter()
        .min_by(|a, b| a.distance_km.total_cmp(&b.distance_km))
        .expect("four candidates");
    assert_eq!(best.index, n / 3, "perturbed query must snap back");
    assert!(
        best.distance_km < 5.0,
        "0.01 deg is ~1 km, got {} km",
        best.distance_km
    );

    // Same query with the C example's mode flags must reuse state and
    // agree.
    let again = nearest.find(
        target.lat + 0.01,
        target.lon + 0.01,
        NearestFlags::SAME_GRID | NearestFlags::SAME_POINT,
    )?;
    assert_eq!(again, found);

    // grib_nearest_multiple.c: exact grid coordinates snap to exactly
    // those points with (near) zero distance.
    let queries: Vec<_> = handle.geo_iter(GeoFlags::empty())?.take(2).collect();
    let lats: Vec<f64> = queries.iter().map(|p| p.lat).collect();
    let lons: Vec<f64> = queries.iter().map(|p| p.lon).collect();
    let multi = handle.find_nearest_multiple(&lats, &lons, false)?;
    assert_eq!(multi.len(), 2);
    for (i, p) in multi.iter().enumerate() {
        assert_eq!(p.index, i);
        assert_eq!(p.value, decoded[i]);
        assert!(p.distance_km < 1e-3, "exact hit, got {} km", p.distance_km);
    }

    // Mismatched query arrays must be rejected, not UB.
    assert!(
        handle
            .find_nearest_multiple(&lats, &lons[..1], false)
            .is_err()
    );
    Ok(())
}

/// Download `name` from the ECMWF test-data server into a cache under
/// the target dir, once. `None` (with a note on stderr) when the network
/// is not cooperating — callers skip, like the CMake suite's optional
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
        Ok(r) => r,
        Err(e) => {
            eprintln!("skipping: cannot fetch {url}: {e}");
            return None;
        }
    };
    let bytes = match response.body_mut().read_to_vec() {
        Ok(b) => b,
        Err(e) => {
            eprintln!("skipping: cannot read {url}: {e}");
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
            let mut handle = MessageReader::grib(&path)?
                .next()
                .expect("GRIB2.tmpl is not empty")?;
            handle.set("shortName", short_name)?;
            handle.set("step", step)?;
            handle.write_to(&mut file)?;
        }
    }

    // grib_index.c builds an empty index and adds the file to it.
    let mut index = Index::new(&["shortName", "step"])?;
    index.add_file(&file_path)?;

    // Distinct values come back in ascending order.
    let short_names = index.values_string("shortName")?;
    let steps = index.values_long("step")?;
    assert_eq!(short_names, ["2t", "msl"]);
    assert_eq!(steps, [0, 6]);
    assert_eq!(index.size("shortName")?, 2);

    // The C example's nested select loops, counting matches per
    // combination and checking each matched message really has the
    // selected key values.
    let mut counts = Vec::new();
    for short_name in &short_names {
        for &step in &steps {
            index.select_string("shortName", short_name)?;
            index.select_long("step", step)?;
            let mut count = 0;
            while let Some(handle) = index.next_handle()? {
                assert_eq!(&handle.get::<String>("shortName")?, short_name);
                assert_eq!(handle.get::<i64>("step")?, step);
                count += 1;
            }
            counts.push(count);
        }
    }
    // Combination order (2t,0), (2t,6), (msl,0), (msl,6) — the last
    // was never written.
    assert_eq!(counts, [1, 1, 1, 0]);

    // Save/reload the index and select through the reloaded copy.
    let idx_path = Path::new(env!("CARGO_TARGET_TMPDIR")).join("index_input.idx");
    index.write(&idx_path)?;
    let mut reread = Index::read(&idx_path)?;
    reread.select_string("shortName", "2t")?;
    reread.select_long("step", 6)?;
    let matched = reread.next_handle()?.expect("(2t, 6) was written");
    assert_eq!(matched.get::<i64>("step")?, 6);
    assert!(reread.next_handle()?.is_none());
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

    let mut index = Index::new(&["shortName", "level", "number", "step"])?;
    index.add_file(&path)?;

    let short_names = index.values_string("shortName")?;
    let levels = index.values_long("level")?;
    let numbers = index.values_long("number")?;
    let steps = index.values_long("step")?;

    let mut selected = 0;
    for short_name in &short_names {
        for &level in &levels {
            for &number in &numbers {
                for &step in &steps {
                    index.select_string("shortName", short_name)?;
                    index.select_long("level", level)?;
                    index.select_long("number", number)?;
                    index.select_long("step", step)?;
                    while index.next_handle()?.is_some() {
                        selected += 1;
                    }
                }
            }
        }
    }
    assert_eq!(selected, 43, "grib_index.sh: '43 messages selected'");
    Ok(())
}

/// Port of `examples/C/grib_multi_write.c` (build a multi-field GRIB2
/// message by appending from section 4 per step) and `grib_multi.c`
/// (read it back field by field with multi-field support on).
#[test]
fn multi_field_write_and_read() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let mut src = MessageReader::grib(&path)?
        .next()
        .expect("GRIB2.tmpl is not empty")?;
    assert_eq!(src.get::<i64>("edition")?, 2, "multi-field needs GRIB2");

    // grib_multi_write.c: one field per step, sections 4-8 repeated.
    let steps: Vec<i64> = (12..=120).step_by(12).collect();
    let mut multi = Handle::<GribMulti>::new()?;
    for &step in &steps {
        src.set("step", step)?;
        multi.append(&src, 4)?;
    }

    let file_path = Path::new(env!("CARGO_TARGET_TMPDIR")).join("multi_step.grib2");
    {
        let mut file = std::fs::File::create(&file_path)?;
        multi.write_to(&mut file)?;
    }

    // Handle::<GribMulti>::new() enabled multi-field support globally
    // (side effect of codes_grib_multi_handle_new); turn it off to test
    // the plain reading mode: one message, keyed by the first field.
    Context::grib_multi_support(false);
    let plain: Vec<_> = MessageReader::grib(&file_path)?.collect::<Result<_, _>>()?;
    assert_eq!(plain.len(), 1);
    assert_eq!(plain[0].get::<i64>("step")?, steps[0]);

    // grib_multi.c: with support on, every field decodes as its own
    // message and the counting function agrees.
    Context::grib_multi_support(true);
    let read_steps: Vec<i64> = MessageReader::grib(&file_path)?
        .map(|h| h.and_then(|h| h.get::<i64>("step")))
        .collect::<Result<_, _>>()?;
    let counted = count_in_file(&file_path)?;
    Context::grib_multi_support(false);

    assert_eq!(read_steps, steps);
    assert_eq!(counted, steps.len());
    Ok(())
}

/// Port of `examples/C/grib_clone.c`, `grib_copy_keys.c` and
/// `grib_copy_message.c`: clones diverge independently, namespaces copy
/// between handles, and the zero-copy message view re-decodes.
#[test]
fn clone_copy_and_message_view() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let src = {
        let mut src = MessageReader::grib(&path)?
            .next()
            .expect("GRIB2.tmpl is not empty")?;
        src.set("centre", 80_i64)?;
        src.set("shortName", "2t")?;
        // A non-trivial data section, so the headers-only size
        // comparison below is meaningful (the template's is ~empty).
        let n = src.size("values")?;
        src.set("bitsPerValue", 24_i64)?;
        src.set(
            "values",
            (0..n)
                .map(|i| i as f64 * 0.5)
                .collect::<Vec<_>>()
                .as_slice(),
        )?;
        src
    };

    // grib_clone.c: mutating the clone must not touch the original.
    let mut clone = src.try_clone()?;
    clone.set("shortName", "msl")?;
    assert_eq!(clone.get::<String>("shortName")?, "msl");
    assert_eq!(src.get::<String>("shortName")?, "2t");

    // Headers-only clone keeps the metadata but drops the data section.
    let headers = src.try_clone_headers_only()?;
    assert_eq!(headers.get::<i64>("centre")?, 80);
    assert!(headers.message_size()? < src.message_size()?);

    // grib_copy_keys.c: pull the `ls` namespace of `src` into a fresh
    // message with different values.
    let mut dst = MessageReader::grib(&path)?
        .next()
        .expect("GRIB2.tmpl is not empty")?;
    assert_ne!(dst.get::<i64>("centre")?, 80);
    dst.copy_namespace(&src, "ls")?;
    assert_eq!(dst.get::<i64>("centre")?, 80);
    assert_eq!(dst.get::<String>("shortName")?, "2t");

    // grib_copy_message.c: the borrowed message view re-decodes into an
    // equal message.
    let view = src.message_data()?;
    let reread = Handle::from_message(view)?;
    assert_eq!(reread.get::<String>("shortName")?, "2t");
    assert_eq!(reread.message_size()?, src.message_size()?);
    Ok(())
}

/// Port of `examples/C/grib_get_data.c`, `grib_set_missing.c`,
/// `grib_set_pv.c` and `grib_precision.c`: the flat lat/lon/values
/// accessor, missing surface keys, vertical-coordinate arrays and
/// decimal-precision re-encoding.
#[test]
fn data_access_missing_pv_precision() -> eccodes::Result<()> {
    let Some(path) = sample("GRIB2.tmpl") else {
        return Ok(());
    };

    let mut handle = MessageReader::grib(&path)?
        .next()
        .expect("GRIB2.tmpl is not empty")?;
    let n = handle.size("values")?;
    handle.set("bitsPerValue", 24_i64)?;
    handle.set(
        "values",
        (0..n)
            .map(|i| i as f64 * 0.5)
            .collect::<Vec<_>>()
            .as_slice(),
    )?;

    // grib_get_data.c: the flat accessor agrees with the geo iterator.
    let (lats, lons, values) = handle.grib_get_data()?;
    assert_eq!(lats.len(), n);
    assert_eq!(lons.len(), n);
    let points: Vec<_> = handle.geo_iter(GeoFlags::empty())?.collect();
    assert_eq!(lats[0], points[0].lat);
    assert_eq!(lons[0], points[0].lon);
    assert_eq!(values, points.iter().map(|p| p.value).collect::<Vec<_>>());

    // grib_set_missing.c: surface-level scale keys can be set missing
    // and report it.
    handle.set("typeOfFirstFixedSurface", "sfc")?;
    handle.set_missing("scaleFactorOfFirstFixedSurface")?;
    handle.set_missing("scaledValueOfFirstFixedSurface")?;
    assert!(handle.is_missing("scaleFactorOfFirstFixedSurface")?);
    assert!(handle.is_missing("scaledValueOfFirstFixedSurface")?);

    // grib_set_pv.c: vertical-coordinate array round trips through its
    // own key.
    let pv = [1.0_f64, 2.0, 3.0, 4.0];
    handle.set("PVPresent", 1_i64)?;
    handle.set("pv", &pv[..])?;
    assert_eq!(handle.get::<Vec<f64>>("pv")?, pv);
    assert_eq!(handle.get::<i64>("NV")?, pv.len() as i64);

    // grib_precision.c: forcing 2 decimal places re-encodes the field
    // exactly at that precision (the ramp has 1 decimal, so values
    // survive unchanged).
    handle.set("setDecimalPrecision", 2_i64)?;
    let rounded: Vec<f64> = handle.get("values")?;
    for (i, (r, p)) in rounded.iter().zip(&points).enumerate() {
        assert!((r - p.value).abs() < 1e-9, "value {i}: {r} vs {}", p.value);
    }
    Ok(())
}
