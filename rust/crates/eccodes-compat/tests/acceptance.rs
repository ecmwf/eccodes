//! Acceptance suite: the `ScaleWeather` eccodes 0.15 tests, run against this
//! compatibility layer. Test bodies and golden values are ported from
//! <https://github.com/ScaleWeather/eccodes> (Apache-2.0); tests poking that
//! crate's private fields are replaced with behavioural equivalents.

// The ported tests keep the original drop choreography, even where this
// layer's types have no Drop of their own.
#![allow(clippy::drop_non_drop)]

use std::path::{Path, PathBuf};
use std::sync::{Arc, Barrier};

use anyhow::{Context, Result};
use eccodes_compat::{
    CodesError, CodesFile, DynamicKeyType, FallibleIterator, KeyRead, KeyWrite, KeysIteratorFlags,
    ProductKind,
};
use float_cmp::assert_approx_eq;

const ICELAND: &str = "./data/iceland.grib";
const ICELAND_SURFACE: &str = "./data/iceland-surface.grib";
const ICELAND_LEVELS: &str = "./data/iceland-levels.grib";

fn scratch_path(name: &str) -> PathBuf {
    std::env::temp_dir().join(format!("eccodes-compat-{name}-{}", std::process::id()))
}

// ---- CodesFile construction ----

#[test]
fn file_constructor() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    assert!(handle.ref_message_iter().next()?.is_some());
    Ok(())
}

#[test]
fn memory_constructor() -> Result<()> {
    let buf = std::fs::read(ICELAND)?;
    let mut handle = CodesFile::new_from_memory(buf, ProductKind::GRIB)?;
    assert!(handle.ref_message_iter().next()?.is_some());
    Ok(())
}

#[test]
fn codes_handle_drop_file() -> Result<()> {
    let handle = CodesFile::new_from_file(Path::new(ICELAND_SURFACE), ProductKind::GRIB)?;
    drop(handle);
    Ok(())
}

#[test]
fn codes_handle_drop_mem() -> Result<()> {
    let buf = std::fs::read(ICELAND)?;
    let handle = CodesFile::new_from_memory(buf, ProductKind::GRIB)?;
    drop(handle);
    Ok(())
}

#[test]
fn multiple_drops() -> Result<()> {
    {
        let mut handle = CodesFile::new_from_file(Path::new(ICELAND_SURFACE), ProductKind::GRIB)?;

        let ref_msg = handle.ref_message_iter().next()?.context("no message")?;
        let mut clone_msg = ref_msg.try_clone()?;
        drop(ref_msg);
        let _oth_ref = handle.ref_message_iter().next()?.context("no message")?;

        let nrst = clone_msg.codes_nearest()?;
        drop(nrst);
        let _kiter = clone_msg.default_keys_iterator()?;
    }
    Ok(())
}

// ---- message iterators ----

#[test]
fn iterator_lifetimes() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND_LEVELS), ProductKind::GRIB)?;

    // Each ref_message_iter() call continues the same underlying cursor.
    let msg1 = handle.ref_message_iter().next()?.context("no message")?;
    let key1 = msg1.read_key_dynamic("typeOfLevel")?;
    drop(msg1);

    let msg2 = handle.ref_message_iter().next()?.context("no message")?;
    let key2 = msg2.read_key_dynamic("typeOfLevel")?;
    drop(msg2);

    let msg3 = handle.ref_message_iter().next()?.context("no message")?;
    let key3 = msg3.read_key_dynamic("typeOfLevel")?;
    drop(msg3);

    assert_eq!(key1, DynamicKeyType::Str("isobaricInhPa".to_string()));
    assert_eq!(key2, DynamicKeyType::Str("isobaricInhPa".to_string()));
    assert_eq!(key3, DynamicKeyType::Str("isobaricInhPa".to_string()));

    Ok(())
}

#[test]
fn message_lifetime_safety() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND_LEVELS), ProductKind::GRIB)?;
    let msg2;
    let msg4;

    {
        let mut mgen = handle.ref_message_iter();

        let msg1 = mgen.next()?.context("no message")?;
        drop(msg1);
        msg2 = mgen.next()?.context("no message")?;
        let msg3 = mgen.next()?.context("no message")?;
        drop(msg3);
        msg4 = mgen.next()?.context("no message")?;
        let msg5 = mgen.next()?.context("no message")?;
        drop(msg5);
    }

    let key2 = msg2.read_key_dynamic("typeOfLevel")?;
    let key4 = msg4.read_key_dynamic("typeOfLevel")?;

    assert_eq!(key2, DynamicKeyType::Str("isobaricInhPa".to_string()));
    assert_eq!(key4, DynamicKeyType::Str("isobaricInhPa".to_string()));

    Ok(())
}

#[test]
fn iterator_fn() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND_SURFACE), ProductKind::GRIB)?;

    while let Some(msg) = handle.ref_message_iter().next()? {
        let key = msg.read_key_dynamic("shortName")?;
        assert!(matches!(key, DynamicKeyType::Str(_)));
    }
    Ok(())
}

#[test]
fn iterator_collected() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND_SURFACE), ProductKind::GRIB)?;

    let mut collected = vec![];
    while let Some(msg) = handle.ref_message_iter().next()? {
        collected.push(msg.try_clone()?);
    }

    for msg in collected {
        let key = msg.read_key_dynamic("name")?;
        assert!(matches!(key, DynamicKeyType::Str(_)));
    }
    Ok(())
}

#[test]
fn iterator_beyond_none() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND_SURFACE), ProductKind::GRIB)?;
    let mut mgen = handle.ref_message_iter();

    for _ in 0..5 {
        assert!(mgen.next()?.is_some());
    }
    for _ in 0..4 {
        assert!(mgen.next()?.is_none());
    }
    Ok(())
}

#[test]
fn iterator_filter() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;

    let mut level = vec![];
    while let Some(msg) = handle.ref_message_iter().next()? {
        if msg.read_key_dynamic("shortName")? == DynamicKeyType::Str("msl".to_string())
            && msg.read_key_dynamic("typeOfLevel")? == DynamicKeyType::Str("surface".to_string())
        {
            level.push(msg.try_clone()?);
        }
    }
    let level = level.first().context("no matching message")?;

    assert_eq!(
        level.read_key_dynamic("shortName")?,
        DynamicKeyType::Str("msl".into())
    );

    let nearest_gridpoints = level.codes_nearest()?.find_nearest(64.13, -21.89)?;
    assert_approx_eq!(f64, nearest_gridpoints[3].value, 100_557.937_5);
    assert_approx_eq!(f64, nearest_gridpoints[3].distance, 14.358_879_960_775_498);

    Ok(())
}

// ---- key reading ----

#[test]
fn static_reading() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let msg = handle.ref_message_iter().next()?.context("no message")?;

    assert_eq!(KeyRead::<i64>::read_key(&msg, "dataDate")?, 20_210_601);
    assert_approx_eq!(
        f32,
        KeyRead::<f32>::read_key(&msg, "jDirectionIncrementInDegrees")?,
        0.25
    );
    assert_approx_eq!(
        f64,
        KeyRead::<f64>::read_key(&msg, "jDirectionIncrementInDegrees")?,
        0.25
    );
    assert_eq!(
        KeyRead::<String>::read_key(&msg, "name")?,
        "Mean sea level pressure"
    );
    assert_eq!(
        KeyRead::<Vec<i64>>::read_key(&msg, "numberOfPointsAlongAParallel")?,
        vec![49]
    );
    assert_eq!(
        KeyRead::<Vec<f32>>::read_key(&msg, "jDirectionIncrementInDegrees")?,
        vec![0.25]
    );
    assert_eq!(
        KeyRead::<Vec<f64>>::read_key(&msg, "jDirectionIncrementInDegrees")?,
        vec![0.25]
    );

    Ok(())
}

#[test]
fn key_reader() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let msg = handle.ref_message_iter().next()?.context("no message")?;

    assert!(matches!(
        msg.read_key_dynamic("name")?,
        DynamicKeyType::Str(_)
    ));
    assert!(matches!(
        msg.read_key_dynamic("jDirectionIncrementInDegrees")?,
        DynamicKeyType::Float(_)
    ));
    assert!(matches!(
        msg.read_key_dynamic("numberOfPointsAlongAParallel")?,
        DynamicKeyType::Int(_)
    ));
    assert!(matches!(
        msg.read_key_dynamic("values")?,
        DynamicKeyType::FloatArray(_)
    ));

    Ok(())
}

#[test]
fn era5_keys_dynamic() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let mut msg = handle.ref_message_iter().next()?.context("no message")?;

    let mut key_names = vec![];
    let mut kiter = msg.default_keys_iterator()?;
    while let Some(key_name) = kiter.next()? {
        assert!(!key_name.is_empty());
        key_names.push(key_name);
    }
    drop(kiter);

    for key_name in &key_names {
        assert!(
            msg.read_key_dynamic(key_name).is_ok(),
            "failed reading {key_name}"
        );
    }
    Ok(())
}

#[test]
fn missing_key() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let msg = handle.ref_message_iter().next()?.context("no message")?;

    assert!(msg.read_key_dynamic("doesNotExist").is_err());
    Ok(())
}

#[test]
fn incorrect_key_type() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let msg = handle.ref_message_iter().next()?.context("no message")?;

    let read: Result<f64, CodesError> = msg.read_key("shortName");
    assert!(matches!(read, Err(CodesError::WrongRequestedKeyType)));
    Ok(())
}

#[test]
fn challenging_keys() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let msg = handle.ref_message_iter().next()?.context("no message")?;

    // key of bytes type
    let _ = msg.read_key_dynamic("section1Padding")?;
    // missing nul-byte termination
    let _ = msg.read_key_dynamic("experimentVersionNumber")?;
    // differing name on different platforms
    let _ = msg
        .read_key_dynamic("zero")
        .or_else(|_| msg.read_key_dynamic("zeros"))?;

    Ok(())
}

// ---- cloning ----

#[test]
fn check_clone_safety() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND_LEVELS), ProductKind::GRIB)?;

    let msg1 = handle.ref_message_iter().next()?.context("no message")?;
    let key1 = msg1.read_key_dynamic("typeOfLevel")?;

    let msg_clone = msg1.try_clone()?;
    drop(msg1);
    drop(handle);
    let key1_clone = msg_clone.read_key_dynamic("typeOfLevel")?;
    assert_eq!(key1, key1_clone);

    Ok(())
}

#[test]
fn message_clone_independent() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let mut mgen = handle.ref_message_iter();
    let msg = mgen.next()?.context("no message")?.try_clone()?;
    let _ = mgen.next()?;

    drop(handle);

    let _ = msg.read_key_dynamic("dataDate")?;
    let _ = msg.read_key_dynamic("jDirectionIncrementInDegrees")?;
    let _ = msg.read_key_dynamic("values")?;
    let _ = msg.read_key_dynamic("name")?;
    let _ = msg.read_key_dynamic("section1Padding")?;
    let _ = msg.read_key_dynamic("experimentVersionNumber")?;

    Ok(())
}

#[test]
fn message_clone_drop() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let msg_ref = handle.ref_message_iter().next()?.context("no message")?;
    let msg_clone = msg_ref.try_clone()?;

    drop(msg_ref);
    drop(handle);
    drop(msg_clone);

    Ok(())
}

// ---- writing ----

#[test]
fn write_message_ref() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let msg = handle.ref_message_iter().next()?.context("no message")?;

    let out_path = scratch_path("write.grib");
    msg.write_to_file(&out_path, false)?;
    std::fs::remove_file(&out_path)?;
    Ok(())
}

#[test]
fn append_message() -> Result<()> {
    let out_path = scratch_path("append.grib");

    let mut handle = CodesFile::new_from_file(Path::new(ICELAND_SURFACE), ProductKind::GRIB)?;
    let msg = handle.ref_message_iter().next()?.context("no message")?;
    msg.write_to_file(&out_path, false)?;

    let mut handle = CodesFile::new_from_file(Path::new(ICELAND_LEVELS), ProductKind::GRIB)?;
    let msg = handle.ref_message_iter().next()?.context("no message")?;
    msg.write_to_file(&out_path, true)?;

    // Both appended messages must read back.
    let mut handle = CodesFile::new_from_file(&out_path, ProductKind::GRIB)?;
    let mut mgen = handle.ref_message_iter();
    assert!(mgen.next()?.is_some());
    assert!(mgen.next()?.is_some());
    assert!(mgen.next()?.is_none());
    drop(mgen);
    drop(handle);

    std::fs::remove_file(&out_path)?;
    Ok(())
}

#[test]
fn write_key() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let msg = handle.ref_message_iter().next()?.context("no message")?;

    let old_key = msg.read_key_dynamic("centre")?;

    let mut cloned = msg.try_clone()?;
    cloned.write_key_unchecked("centre", "cnmc")?;

    let read_key = cloned.read_key_dynamic("centre")?;
    assert_ne!(old_key, read_key);
    assert_eq!(read_key, DynamicKeyType::Str("cnmc".into()));

    Ok(())
}

#[test]
fn write_key_types() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let mut msg = handle
        .ref_message_iter()
        .next()?
        .context("no message")?
        .try_clone()?;

    let mut values_array: Vec<f64> = msg.read_key("values")?;
    values_array[0] = 0.0;

    msg.write_key_unchecked("centre", "cnmc")?; // str
    msg.write_key_unchecked("day", 3)?; // int
    msg.write_key_unchecked("latitudeOfFirstGridPointInDegrees", 7.0)?; // float
    msg.write_key_unchecked("values", values_array.as_slice())?; // float array

    Ok(())
}

#[test]
fn edit_keys_and_save() -> Result<()> {
    let out_path = scratch_path("edit.grib");

    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let msg = handle.ref_message_iter().next()?.context("no message")?;

    let old_key = msg.read_key_dynamic("centre")?;

    let mut cloned = msg.try_clone()?;
    cloned.write_key_unchecked("centre", "cnmc")?;
    cloned.write_to_file(&out_path, false)?;

    let mut handle = CodesFile::new_from_file(&out_path, ProductKind::GRIB)?;
    let msg = handle.ref_message_iter().next()?.context("no message")?;
    let read_key = msg.read_key_dynamic("centre")?;

    assert_ne!(old_key, read_key);
    assert_eq!(read_key, DynamicKeyType::Str("cnmc".into()));
    drop(msg);
    drop(handle);

    std::fs::remove_file(&out_path)?;
    Ok(())
}

// ---- keys iterator ----

#[test]
fn keys_iterator_parameters() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let mut msg = handle.ref_message_iter().next()?.context("no message")?;

    let flags = [
        KeysIteratorFlags::AllKeys,
        KeysIteratorFlags::SkipOptional,
        KeysIteratorFlags::SkipReadOnly,
        KeysIteratorFlags::SkipDuplicates,
    ];
    let mut kiter = msg.new_keys_iterator(&flags, "geography")?;

    assert!(kiter.next()?.is_some());
    while let Some(key_name) = kiter.next()? {
        assert!(!key_name.is_empty());
    }
    Ok(())
}

#[test]
fn invalid_namespace() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let mut msg = handle.ref_message_iter().next()?.context("no message")?;

    let mut kiter = msg.new_keys_iterator(&[KeysIteratorFlags::AllKeys], "blabla")?;
    while let Some(key_name) = kiter.next()? {
        assert!(!key_name.is_empty());
    }
    Ok(())
}

#[test]
fn keys_iterator_destructor() -> Result<()> {
    let mut handle = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let mut msg = handle.ref_message_iter().next()?.context("no message")?;

    let _kiter = msg.default_keys_iterator()?;
    Ok(())
}

// ---- nearest ----

#[test]
fn find_nearest() -> Result<()> {
    let mut handle1 = CodesFile::new_from_file(Path::new(ICELAND), ProductKind::GRIB)?;
    let msg1 = handle1.ref_message_iter().next()?.context("no message")?;
    let out1 = msg1.codes_nearest()?.find_nearest(64.13, -21.89)?;

    let mut handle2 = CodesFile::new_from_file(Path::new(ICELAND_SURFACE), ProductKind::GRIB)?;
    let msg2 = handle2.ref_message_iter().next()?.context("no message")?;
    let out2 = msg2.codes_nearest()?.find_nearest(64.13, -21.89)?;

    assert!(out1[0].value > 10000.0);
    assert_eq!(out2[3].index, 551);
    assert!((out1[1].lat - 64.0).abs() < f64::EPSILON);
    assert!((out2[2].lon - -21.75).abs() < f64::EPSILON);
    assert!(out1[0].distance > 15.0);

    Ok(())
}

// ---- threading ----

#[test]
fn thread_safety_message_wise() -> Result<()> {
    let handle = CodesFile::new_from_file(Path::new(ICELAND_LEVELS), ProductKind::GRIB)?;
    let mut mgen = handle.arc_message_iter();

    let barrier = Arc::new(Barrier::new(10));
    let mut threads = vec![];

    for _ in 0..10 {
        let msg = mgen.next()?.context("no more messages")?;
        let barrier = barrier.clone();

        threads.push(std::thread::spawn(move || {
            for _ in 0..10 {
                barrier.wait();
                for _ in 0..100 {
                    let _ = msg
                        .read_key_dynamic("shortName")
                        .expect("read_key_dynamic failed in thread");
                }
            }
        }));
    }

    for thread in threads {
        thread.join().expect("reader thread panicked");
    }
    Ok(())
}

#[test]
fn thread_safety_within_message() -> Result<()> {
    let handle = CodesFile::new_from_file(Path::new(ICELAND_LEVELS), ProductKind::GRIB)?;
    let mut mgen = handle.arc_message_iter();
    let msg = Arc::new(mgen.next()?.context("no more messages")?);

    let barrier = Arc::new(Barrier::new(10));
    let mut threads = vec![];

    for _ in 0..10 {
        let msg = msg.clone();
        let barrier = barrier.clone();

        threads.push(std::thread::spawn(move || {
            for _ in 0..10 {
                barrier.wait();
                for _ in 0..100 {
                    let _ = msg
                        .read_key_dynamic("shortName")
                        .expect("read_key_dynamic failed in thread");
                }
            }
        }));
    }

    for thread in threads {
        thread.join().expect("reader thread panicked");
    }
    Ok(())
}

// ---- ndarray ----

#[cfg(feature = "ndarray")]
mod ndarray_tests {
    use super::*;

    #[test]
    fn to_ndarray_f32() -> Result<()> {
        let mut handle = CodesFile::new_from_file(Path::new(ICELAND_SURFACE), ProductKind::GRIB)?;

        while let Some(msg) = handle.ref_message_iter().next()? {
            if msg.read_key_dynamic("shortName")? == DynamicKeyType::Str("2d".to_string()) {
                let ndarray = msg.to_ndarray::<f32>()?;
                // values from xarray
                assert_approx_eq!(f32, ndarray[[0, 0]], 276.37793, epsilon = 0.000_1);
                assert_approx_eq!(f32, ndarray[[0, 48]], 276.65723, epsilon = 0.000_1);
                assert_approx_eq!(f32, ndarray[[16, 0]], 277.91113, epsilon = 0.000_1);
                assert_approx_eq!(f32, ndarray[[16, 48]], 280.34277, epsilon = 0.000_1);
                assert_approx_eq!(f32, ndarray[[5, 5]], 276.03418, epsilon = 0.000_1);
                assert_approx_eq!(f32, ndarray[[10, 10]], 277.59082, epsilon = 0.000_1);
                assert_approx_eq!(f32, ndarray[[15, 15]], 277.68652, epsilon = 0.000_1);
                assert_approx_eq!(f32, ndarray[[8, 37]], 273.2744, epsilon = 0.000_1);
                return Ok(());
            }
        }
        anyhow::bail!("no 2d message found");
    }

    #[test]
    fn to_ndarray_f64() -> Result<()> {
        let mut handle = CodesFile::new_from_file(Path::new(ICELAND_SURFACE), ProductKind::GRIB)?;

        while let Some(msg) = handle.ref_message_iter().next()? {
            if msg.read_key_dynamic("shortName")? == DynamicKeyType::Str("2d".to_string()) {
                let ndarray = msg.to_ndarray::<f64>()?;
                // values from xarray
                assert_approx_eq!(f64, ndarray[[0, 0]], 276.37793, epsilon = 0.000_1);
                assert_approx_eq!(f64, ndarray[[0, 48]], 276.65723, epsilon = 0.000_1);
                assert_approx_eq!(f64, ndarray[[16, 0]], 277.91113, epsilon = 0.000_1);
                assert_approx_eq!(f64, ndarray[[16, 48]], 280.34277, epsilon = 0.000_1);
                assert_approx_eq!(f64, ndarray[[5, 5]], 276.03418, epsilon = 0.000_1);
                assert_approx_eq!(f64, ndarray[[10, 10]], 277.59082, epsilon = 0.000_1);
                assert_approx_eq!(f64, ndarray[[15, 15]], 277.68652, epsilon = 0.000_1);
                assert_approx_eq!(f64, ndarray[[8, 37]], 273.2744, epsilon = 0.000_1);
                return Ok(());
            }
        }
        anyhow::bail!("no 2d message found");
    }

    #[test]
    fn lons_lats_values() -> Result<()> {
        let mut handle = CodesFile::new_from_file(Path::new(ICELAND_SURFACE), ProductKind::GRIB)?;

        while let Some(msg) = handle.ref_message_iter().next()? {
            if msg.read_key_dynamic("shortName")? == DynamicKeyType::Str("2d".to_string()) {
                let rmsg = msg.to_lons_lats_values()?;
                let (vals, lons, lats) = (rmsg.values, rmsg.longitudes, rmsg.latitudes);

                // values from cfgrib
                assert_approx_eq!(f64, vals[[0, 0]], 276.37793, epsilon = 0.000_1);
                assert_approx_eq!(f64, vals[[16, 48]], 280.34277, epsilon = 0.000_1);
                assert_approx_eq!(f64, vals[[8, 37]], 273.2744, epsilon = 0.000_1);

                assert_approx_eq!(f64, lons[[0, 0]], -25.0);
                assert_approx_eq!(f64, lons[[16, 48]], -13.0);
                assert_approx_eq!(f64, lons[[8, 37]], -15.75);

                assert_approx_eq!(f64, lats[[0, 0]], 67.0);
                assert_approx_eq!(f64, lats[[16, 48]], 63.0);
                assert_approx_eq!(f64, lats[[8, 37]], 65.0);
                return Ok(());
            }
        }
        anyhow::bail!("no 2d message found");
    }
}
