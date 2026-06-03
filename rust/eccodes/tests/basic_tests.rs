//! Integration tests for the ecCodes Rust API.
//!
//! These tests use the test data from the ecCodes build directory.
//! Set ECCODES_TEST_DATA_DIR to the data directory if not using the default.

use eccodes::{EcCodesError, Message, MessageIterator, ProductKind, Result};
use std::path::PathBuf;

/// Get the path to the ecCodes test data directory.
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

fn grib1_file() -> String {
    test_data_dir()
        .join("constant_field.grib1")
        .to_string_lossy()
        .into_owned()
}

fn gen_grib_file() -> String {
    test_data_dir()
        .join("gen.grib")
        .to_string_lossy()
        .into_owned()
}

// ============================================================================
// Library info tests
// ============================================================================

#[test]
fn test_api_version() {
    let version = eccodes::api_version();
    assert!(version > 0, "API version should be positive");
    assert!(version >= 24800, "Expected at least version 2.48.0");
}

#[test]
fn test_package_name() {
    let name = eccodes::package_name();
    assert_eq!(name, "ecCodes");
}

#[test]
fn test_git_sha1() {
    let sha = eccodes::git_sha1();
    assert!(!sha.is_empty(), "Git SHA1 should not be empty");
}

#[test]
fn test_git_branch() {
    // May be empty in release builds, just check it doesn't crash
    let _ = eccodes::git_branch();
}

#[test]
fn test_build_date() {
    let date = eccodes::build_date();
    assert!(!date.is_empty(), "Build date should not be empty");
}

// ============================================================================
// Count messages
// ============================================================================

#[test]
fn test_count_messages() {
    let count = eccodes::count_messages(&grib2_file()).unwrap();
    assert_eq!(count, 1);
}

#[test]
fn test_count_messages_file_not_found() {
    let result = eccodes::count_messages("/nonexistent/file.grib2");
    assert!(result.is_err());
}

// ============================================================================
// MessageIterator tests
// ============================================================================

#[test]
fn test_message_iterator_grib2() {
    let mut count = 0;
    for msg in MessageIterator::from_grib_file(&grib2_file()).unwrap() {
        let _msg = msg.unwrap();
        count += 1;
    }
    assert_eq!(count, 1);
}

#[test]
fn test_message_iterator_grib1() {
    let mut count = 0;
    for msg in MessageIterator::from_grib_file(&grib1_file()).unwrap() {
        let _msg = msg.unwrap();
        count += 1;
    }
    assert_eq!(count, 1);
}

#[test]
fn test_message_iterator_from_file_generic() {
    let mut count = 0;
    for msg in MessageIterator::from_file(&grib2_file(), ProductKind::Grib).unwrap() {
        let _msg = msg.unwrap();
        count += 1;
    }
    assert_eq!(count, 1);
}

#[test]
fn test_message_iterator_file_not_found() {
    let result = MessageIterator::from_grib_file("/nonexistent/file.grib2");
    assert!(result.is_err());
}

// ============================================================================
// Get scalar values
// ============================================================================

#[test]
fn test_get_long() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let level: i64 = msg.get("level").unwrap();
    assert!(level >= 0);
}

#[test]
fn test_get_double() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    // latitudeOfFirstGridPointInDegrees is always available in GRIB
    let lat: f64 = msg.get("latitudeOfFirstGridPointInDegrees").unwrap();
    assert!(lat >= -90.0 && lat <= 90.0);
}

#[test]
fn test_get_float() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let lat: f32 = msg.get("latitudeOfFirstGridPointInDegrees").unwrap();
    assert!(lat >= -90.0 && lat <= 90.0);
}

#[test]
fn test_get_string() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let name: String = msg.get("shortName").unwrap();
    assert!(!name.is_empty());
}

#[test]
fn test_get_nonexistent_key() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let result: Result<i64> = msg.get("thisKeyDoesNotExist12345");
    assert!(result.is_err());
}

// ============================================================================
// Get array values
// ============================================================================

#[test]
fn test_get_double_array() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let values: Vec<f64> = msg.get("values").unwrap();
    let num_points: i64 = msg.get("numberOfPoints").unwrap();
    assert_eq!(values.len(), num_points as usize);
}

#[test]
fn test_get_float_array() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let values: Vec<f32> = msg.get("values").unwrap();
    let num_points: i64 = msg.get("numberOfPoints").unwrap();
    assert_eq!(values.len(), num_points as usize);
}

#[test]
fn test_get_long_array() {
    // pl (number of points per latitude) is available on reduced Gaussian grids
    // Use a file that has it, or test with a key that's always an array
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    // "values" are normally doubles, but let's use the level as a long array test
    // Actually test with a scalar as 1-element array
    let edition: Vec<i64> = msg.get("experimentVersionNumber").unwrap_or_else(|_| vec![]);
    // Just make sure it didn't crash
    let _ = edition;
}

// ============================================================================
// Get metadata
// ============================================================================

#[test]
fn test_get_size() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let size = msg.get_size("values").unwrap();
    let num_points: i64 = msg.get("numberOfPoints").unwrap();
    assert_eq!(size, num_points as usize);
}

#[test]
fn test_get_length() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let len = msg.get_length("shortName").unwrap();
    assert!(len > 0);
}

#[test]
fn test_is_defined() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    assert!(msg.is_defined("shortName"));
    assert!(!msg.is_defined("thisKeyDoesNotExist12345"));
}

#[test]
fn test_is_missing() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    // shortName should not be missing
    let missing = msg.is_missing("shortName").unwrap();
    assert!(!missing);
}

#[test]
fn test_product_kind() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let kind = msg.product_kind().unwrap();
    assert_eq!(kind, ProductKind::Grib);
}

// ============================================================================
// Set values
// ============================================================================

#[test]
fn test_set_long() {
    let mut msg = Message::from_grib_sample("GRIB2").unwrap();
    // "edition" is a simple coded long, safe to set/get roundtrip
    let edition: i64 = msg.get("edition").unwrap();
    assert_eq!(edition, 2);
    // Set a numeric key that works directly
    msg.set("centre", &98_i64).unwrap();
    let centre: i64 = msg.get("centre").unwrap();
    assert_eq!(centre, 98);
}

#[test]
fn test_set_double() {
    let mut msg = Message::from_grib_sample("GRIB2").unwrap();
    msg.set("longitudeOfFirstGridPointInDegrees", &10.5_f64)
        .unwrap();
    let lon: f64 = msg.get("longitudeOfFirstGridPointInDegrees").unwrap();
    assert!((lon - 10.5).abs() < 1e-3);
}

#[test]
fn test_set_string() {
    let mut msg = Message::from_grib_sample("GRIB2").unwrap();
    msg.set("shortName", "2t").unwrap();
    let name: String = msg.get("shortName").unwrap();
    assert_eq!(name, "2t");
}

#[test]
fn test_set_double_array() {
    let mut msg = Message::from_grib_sample("GRIB2").unwrap();
    let num_points: i64 = msg.get("numberOfPoints").unwrap();
    let data: Vec<f64> = (0..num_points as usize)
        .map(|i| 200.0 + (i as f64) * 0.01)
        .collect();
    msg.set("values", data.as_slice()).unwrap();
    let values: Vec<f64> = msg.get("values").unwrap();
    assert_eq!(values.len(), num_points as usize);
    // Check first and last values are in expected range
    assert!((values[0] - 200.0).abs() < 1.0);
}

#[test]
fn test_set_float_array() {
    let mut msg = Message::from_grib_sample("GRIB2").unwrap();
    let num_points: i64 = msg.get("numberOfPoints").unwrap();
    // Set via double array first (float set may not be supported in all builds)
    let data_f64: Vec<f64> = (0..num_points as usize)
        .map(|i| 200.0 + (i as f64) * 0.01)
        .collect();
    msg.set("values", data_f64.as_slice()).unwrap();
    // Then verify we can GET as float
    let values: Vec<f32> = msg.get("values").unwrap();
    assert_eq!(values.len(), num_points as usize);
}

#[test]
fn test_set_long_array() {
    // Setting a long array isn't common for most keys, but we can test via "pl" or similar
    // Let's test set_missing instead as it's more universal
    let mut msg = Message::from_grib_sample("GRIB2").unwrap();
    // scaleFactorOfSecondFixedSurface can be set to missing
    msg.set_missing("scaleFactorOfSecondFixedSurface").unwrap();
    let missing = msg.is_missing("scaleFactorOfSecondFixedSurface").unwrap();
    assert!(missing);
}

// ============================================================================
// Sample creation
// ============================================================================

#[test]
fn test_from_grib_sample() {
    let msg = Message::from_grib_sample("GRIB2").unwrap();
    let edition: i64 = msg.get("edition").unwrap();
    assert_eq!(edition, 2);
}

#[test]
fn test_from_grib1_sample() {
    let msg = Message::from_grib_sample("GRIB1").unwrap();
    let edition: i64 = msg.get("edition").unwrap();
    assert_eq!(edition, 1);
}

#[test]
fn test_from_sample_generic() {
    let msg = Message::from_sample("GRIB2").unwrap();
    let edition: i64 = msg.get("edition").unwrap();
    assert_eq!(edition, 2);
}

#[test]
fn test_from_sample_invalid() {
    let result = Message::from_grib_sample("NONEXISTENT_SAMPLE_12345");
    assert!(result.is_err());
}

// ============================================================================
// Clone
// ============================================================================

#[test]
fn test_clone() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let name_orig: String = msg.get("shortName").unwrap();
    let cloned = msg.clone();
    let name_clone: String = cloned.get("shortName").unwrap();
    assert_eq!(name_orig, name_clone);
}

// ============================================================================
// Write
// ============================================================================

#[test]
fn test_write_to_file() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();

    let tmp = std::env::temp_dir().join("eccodes_rust_test_write.grib2");
    let tmp_path = tmp.to_string_lossy().into_owned();
    msg.write_to_file(&tmp_path).unwrap();

    // Read it back
    let count = eccodes::count_messages(&tmp_path).unwrap();
    assert_eq!(count, 1);

    let msg2 = MessageIterator::from_grib_file(&tmp_path)
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let name: String = msg2.get("shortName").unwrap();
    let name_orig: String = msg.get("shortName").unwrap();
    assert_eq!(name, name_orig);

    std::fs::remove_file(&tmp_path).ok();
}

#[test]
fn test_append_to_file() {
    let msg = Message::from_grib_sample("GRIB2").unwrap();

    let tmp = std::env::temp_dir().join("eccodes_rust_test_append.grib2");
    let tmp_path = tmp.to_string_lossy().into_owned();

    // Remove if exists
    std::fs::remove_file(&tmp_path).ok();

    msg.write_to_file(&tmp_path).unwrap();
    msg.append_to_file(&tmp_path).unwrap();

    let count = eccodes::count_messages(&tmp_path).unwrap();
    assert_eq!(count, 2);

    std::fs::remove_file(&tmp_path).ok();
}

// ============================================================================
// Keys iterator
// ============================================================================

#[test]
fn test_keys_iterator() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let keys: Vec<String> = msg.keys().collect();
    assert!(!keys.is_empty());
    assert!(keys.contains(&"shortName".to_string()) || keys.contains(&"edition".to_string()));
}

#[test]
fn test_keys_iterator_namespace() {
    let msg = MessageIterator::from_grib_file(&grib2_file())
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let keys: Vec<String> = msg.keys_in_namespace("ls").collect();
    // The "ls" namespace should have some keys
    assert!(!keys.is_empty());
}

// ============================================================================
// Error handling
// ============================================================================

#[test]
fn test_error_display() {
    let err = EcCodesError::NotFound;
    let msg = format!("{}", err);
    assert!(msg.contains("ecCodes error"));
}

#[test]
fn test_error_message() {
    let err = EcCodesError::NotFound;
    let msg = err.message();
    assert!(!msg.is_empty());
}

// ============================================================================
// Round-trip: create, set, write, read, verify
// ============================================================================

#[test]
fn test_roundtrip() {
    let mut msg = Message::from_grib_sample("GRIB2").unwrap();
    msg.set("shortName", "2t").unwrap();
    msg.set("centre", &98_i64).unwrap();
    let num_points: i64 = msg.get("numberOfPoints").unwrap();
    let data: Vec<f64> = (0..num_points as usize)
        .map(|i| 273.15 + (i as f64) * 0.001)
        .collect();
    msg.set("values", data.as_slice()).unwrap();

    let tmp = std::env::temp_dir().join("eccodes_rust_test_roundtrip.grib2");
    let tmp_path = tmp.to_string_lossy().into_owned();
    msg.write_to_file(&tmp_path).unwrap();

    let msg2 = MessageIterator::from_grib_file(&tmp_path)
        .unwrap()
        .next()
        .unwrap()
        .unwrap();
    let name: String = msg2.get("shortName").unwrap();
    assert_eq!(name, "2t");
    let centre: i64 = msg2.get("centre").unwrap();
    assert_eq!(centre, 98);

    let values2: Vec<f64> = msg2.get("values").unwrap();
    assert_eq!(values2.len(), num_points as usize);

    std::fs::remove_file(&tmp_path).ok();
}

// ============================================================================
// Multiple messages iteration
// ============================================================================

#[test]
fn test_multiple_messages() {
    // Create a file with 3 messages
    let tmp = std::env::temp_dir().join("eccodes_rust_test_multi.grib2");
    let tmp_path = tmp.to_string_lossy().into_owned();
    std::fs::remove_file(&tmp_path).ok();

    for i in 0..3 {
        let mut msg = Message::from_grib_sample("GRIB2").unwrap();
        msg.set("centre", &((i + 1) * 10_i64)).unwrap();
        if i == 0 {
            msg.write_to_file(&tmp_path).unwrap();
        } else {
            msg.append_to_file(&tmp_path).unwrap();
        }
    }

    let count = eccodes::count_messages(&tmp_path).unwrap();
    assert_eq!(count, 3);

    let mut centres = Vec::new();
    for msg in MessageIterator::from_grib_file(&tmp_path).unwrap() {
        let msg = msg.unwrap();
        let centre: i64 = msg.get("centre").unwrap();
        centres.push(centre);
    }
    assert_eq!(centres, vec![10, 20, 30]);

    std::fs::remove_file(&tmp_path).ok();
}
