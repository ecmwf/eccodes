//! Tests for advanced features (requires `features = ["advanced"]`).

#![cfg(feature = "advanced")]

use eccodes::{Message, MessageIterator, Result};
use std::path::PathBuf;

fn test_data_dir() -> PathBuf {
    std::env::var("ECCODES_TEST_DATA_DIR")
        .map(PathBuf::from)
        .unwrap_or_else(|_| PathBuf::from("/home/joobog/build/eccodes/data"))
}

fn grib2_file() -> String {
    test_data_dir()
        .join("constant_field.grib2")
        .to_string_lossy()
        .into_owned()
}

// ============================================================================
// Geo iterator
// ============================================================================

#[test]
fn test_geo_iterator() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let num_points: i64 = msg.get("numberOfPoints").unwrap();
    let mut count = 0;
    for point in msg.geo_iterator().unwrap() {
        assert!(point.lat >= -90.0 && point.lat <= 90.0);
        assert!(point.lon >= -360.0 && point.lon <= 360.0);
        count += 1;
    }
    assert_eq!(count, num_points as usize);
}

#[test]
fn test_geo_iterator_reset() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let mut iter = msg.geo_iterator().unwrap();

    // Read first point
    let first = iter.next().unwrap();

    // Consume rest
    while iter.next().is_some() {}

    // Reset and read again
    iter.reset().unwrap();
    let first_again = iter.next().unwrap();

    assert!((first.lat - first_again.lat).abs() < 1e-10);
    assert!((first.lon - first_again.lon).abs() < 1e-10);
}

#[test]
fn test_get_data() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let num_points: i64 = msg.get("numberOfPoints").unwrap();
    let data = msg.get_data().unwrap();
    assert_eq!(data.len(), num_points as usize);
    for point in &data {
        assert!(point.lat >= -90.0 && point.lat <= 90.0);
    }
}

// ============================================================================
// Nearest neighbour
// ============================================================================

#[test]
fn test_nearest() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let mut nearest = msg.nearest().unwrap();
    let points = nearest.find(51.5, -0.1).unwrap();
    assert_eq!(points.len(), 4);
    for p in &points {
        assert!(p.lat >= -90.0 && p.lat <= 90.0);
        assert!(p.distance_km >= 0.0);
    }
}

// ============================================================================
// Index
// ============================================================================

#[test]
fn test_index_from_file() {
    use eccodes::index::Index;

    let mut index = Index::from_file(&grib2_file(), &["shortName"]).unwrap();
    let names: Vec<String> = index.get("shortName").unwrap();
    assert!(!names.is_empty());

    // Select and read
    index.select("shortName", &names[0]).unwrap();
    let msg = index.next_message().unwrap();
    assert!(msg.is_some());
    let msg = msg.unwrap();
    let name: String = msg.get("shortName").unwrap();
    assert_eq!(name, names[0]);

    // No more messages
    let msg2 = index.next_message().unwrap();
    assert!(msg2.is_none());
}

#[test]
fn test_index_get_size() {
    use eccodes::index::Index;

    let index = Index::from_file(&grib2_file(), &["level"]).unwrap();
    let size = index.get_size("level").unwrap();
    assert!(size >= 1);
}

#[test]
fn test_index_get_long() {
    use eccodes::index::Index;

    let index = Index::from_file(&grib2_file(), &["level"]).unwrap();
    let levels: Vec<i64> = index.get("level").unwrap();
    assert!(!levels.is_empty());
}

#[test]
fn test_index_new_and_add_file() {
    use eccodes::index::Index;

    let mut index = Index::new(&["shortName"]).unwrap();
    index.add_file(&grib2_file()).unwrap();
    let size = index.get_size("shortName").unwrap();
    assert!(size >= 1);
}

// ============================================================================
// Multi handle
// ============================================================================

#[test]
fn test_multi_handle() {
    use eccodes::multi_handle::MultiHandle;

    let tmp = std::env::temp_dir().join("eccodes_rust_test_multi_handle.grib2");
    let tmp_path = tmp.to_string_lossy().into_owned();
    std::fs::remove_file(&tmp_path).ok();

    let mut mh = MultiHandle::new().unwrap();

    for i in 0..2 {
        let mut msg = Message::from_grib_sample("GRIB2").unwrap();
        msg.set("centre", &((i + 1) * 10_i64)).unwrap();
        mh.append(&mut msg, 4).unwrap(); // section 4 and onwards
    }

    mh.write(&tmp_path).unwrap();

    let count = eccodes::count_messages(&tmp_path).unwrap();
    assert!(count >= 1); // multi-field messages may count differently

    std::fs::remove_file(&tmp_path).ok();
}

// ============================================================================
// Context
// ============================================================================

#[test]
fn test_context_default() {
    use eccodes::context::Context;
    let _ctx = Context::get_default();
    // Just check it doesn't crash
}

// ============================================================================
// Native type / key_is_computed / get_offset
// ============================================================================

#[test]
fn test_native_type() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let t = msg.native_type("level").unwrap();
    assert_eq!(t, eccodes::KeyType::Long);

    let t2 = msg.native_type("shortName").unwrap();
    assert_eq!(t2, eccodes::KeyType::String);
}

#[test]
fn test_key_is_computed() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    // "shortName" is typically computed
    let computed = msg.key_is_computed("shortName").unwrap();
    assert!(computed);
    // "edition" is typically coded
    let coded = msg.key_is_computed("edition").unwrap();
    assert!(!coded);
}

#[test]
fn test_get_offset() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let offset = msg.get_offset("edition").unwrap();
    // Edition byte is near the beginning of the message
    assert!(offset < 100);
}

// ============================================================================
// Message size / offset
// ============================================================================

#[test]
fn test_message_size() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let size = msg.message_size().unwrap();
    assert!(size > 0);
}

#[test]
fn test_message_offset() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let offset = msg.message_offset().unwrap();
    assert_eq!(offset, 0); // First message starts at offset 0
}

// ============================================================================
// to_bytes / from_bytes_copy roundtrip
// ============================================================================

#[test]
fn test_to_bytes_from_bytes() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let bytes = msg.to_bytes().unwrap();
    assert!(!bytes.is_empty());

    // Reconstruct from bytes
    let msg2 = Message::from_bytes_copy(&bytes).unwrap();
    let name1: String = msg.get("shortName").unwrap();
    let name2: String = msg2.get("shortName").unwrap();
    assert_eq!(name1, name2);
}

// ============================================================================
// Clone headers only
// ============================================================================

#[test]
fn test_clone_headers_only() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let cloned = msg.clone_headers_only().unwrap();
    let name: String = cloned.get("shortName").unwrap();
    let name_orig: String = msg.get("shortName").unwrap();
    assert_eq!(name, name_orig);
}

// ============================================================================
// Get element
// ============================================================================

#[test]
fn test_get_element_double() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let val: f64 = msg.get_element("values", 0).unwrap();
    let values: Vec<f64> = msg.get("values").unwrap();
    assert!((val - values[0]).abs() < 1e-10);
}

#[test]
fn test_get_elements_double() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let vals: Vec<f64> = msg.get_elements("values", &[0, 1, 2]).unwrap();
    assert_eq!(vals.len(), 3);
    let all_values: Vec<f64> = msg.get("values").unwrap();
    for i in 0..3 {
        assert!((vals[i] - all_values[i]).abs() < 1e-10);
    }
}

// ============================================================================
// Utility functions
// ============================================================================

#[test]
fn test_datetime_to_julian_roundtrip() {
    use eccodes::util;
    let jd = util::datetime_to_julian(2024, 6, 15, 12, 0, 0).unwrap();
    assert!(jd > 0.0);
    let (y, m, d, h, mi, s) = util::julian_to_datetime(jd).unwrap();
    assert_eq!(y, 2024);
    assert_eq!(m, 6);
    assert_eq!(d, 15);
    assert_eq!(h, 12);
    assert_eq!(mi, 0);
    assert_eq!(s, 0);
}

#[test]
fn test_date_to_julian_roundtrip() {
    use eccodes::util;
    let jd = util::date_to_julian(20240615);
    let date = util::julian_to_date(jd);
    assert_eq!(date, 20240615);
}

// ============================================================================
// Force set
// ============================================================================

#[test]
fn test_set_force_double_array() {
    use eccodes::message::SetForceKey;

    let mut msg = Message::from_grib_sample("GRIB2").unwrap();
    let num_points: i64 = msg.get("numberOfPoints").unwrap();
    let data = vec![99.0f64; num_points as usize];
    msg.set_force("codedValues", data.as_slice()).unwrap();
    let values: Vec<f64> = msg.get("codedValues").unwrap();
    assert_eq!(values.len(), num_points as usize);
}
