//! Raw FFI bindings to the ecCodes C library.
//!
//! This crate provides unsafe, low-level bindings. Use the `eccodes` crate
//! for a safe, idiomatic Rust API.

#![allow(non_camel_case_types)]
#![allow(non_upper_case_globals)]

use std::os::raw::{c_char, c_double, c_float, c_int, c_long, c_uchar, c_ulong, c_void};

// ── Opaque types ─────────────────────────────────────────────────────────────

#[repr(C)]
pub struct codes_handle {
    _opaque: [u8; 0],
}

#[repr(C)]
pub struct codes_context {
    _opaque: [u8; 0],
}

#[repr(C)]
pub struct codes_keys_iterator {
    _opaque: [u8; 0],
}

#[repr(C)]
pub struct codes_iterator {
    _opaque: [u8; 0],
}

#[repr(C)]
pub struct codes_nearest {
    _opaque: [u8; 0],
}

#[repr(C)]
pub struct codes_index {
    _opaque: [u8; 0],
}

#[repr(C)]
pub struct codes_multi_handle {
    _opaque: [u8; 0],
}

#[repr(C)]
pub struct codes_bufr_header {
    _opaque: [u8; 0],
}

// ── Enums ────────────────────────────────────────────────────────────────────

#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ProductKind {
    PRODUCT_ANY = 0,
    PRODUCT_GRIB = 1,
    PRODUCT_BUFR = 2,
    PRODUCT_METAR = 3,
    PRODUCT_GTS = 4,
    PRODUCT_TAF = 5,
}

// ── Constants ────────────────────────────────────────────────────────────────

// Type constants
pub const CODES_TYPE_UNDEFINED: c_int = 0;
pub const CODES_TYPE_LONG: c_int = 1;
pub const CODES_TYPE_DOUBLE: c_int = 2;
pub const CODES_TYPE_STRING: c_int = 3;
pub const CODES_TYPE_BYTES: c_int = 4;
pub const CODES_TYPE_SECTION: c_int = 5;
pub const CODES_TYPE_LABEL: c_int = 6;
pub const CODES_TYPE_MISSING: c_int = 7;

// Error codes
pub const CODES_SUCCESS: c_int = 0;
pub const CODES_END_OF_FILE: c_int = -1;
pub const CODES_INTERNAL_ERROR: c_int = -2;
pub const CODES_BUFFER_TOO_SMALL: c_int = -3;
pub const CODES_NOT_IMPLEMENTED: c_int = -4;
pub const CODES_7777_NOT_FOUND: c_int = -5;
pub const CODES_ARRAY_TOO_SMALL: c_int = -6;
pub const CODES_FILE_NOT_FOUND: c_int = -7;
pub const CODES_CODE_NOT_FOUND_IN_TABLE: c_int = -8;
pub const CODES_WRONG_ARRAY_SIZE: c_int = -9;
pub const CODES_NOT_FOUND: c_int = -10;
pub const CODES_IO_PROBLEM: c_int = -11;
pub const CODES_INVALID_MESSAGE: c_int = -12;
pub const CODES_DECODING_ERROR: c_int = -13;
pub const CODES_ENCODING_ERROR: c_int = -14;
pub const CODES_NO_MORE_IN_SET: c_int = -15;
pub const CODES_GEOCALCULUS_PROBLEM: c_int = -16;
pub const CODES_OUT_OF_MEMORY: c_int = -17;
pub const CODES_READ_ONLY: c_int = -18;
pub const CODES_INVALID_ARGUMENT: c_int = -19;
pub const CODES_NULL_HANDLE: c_int = -20;
pub const CODES_INVALID_SECTION_NUMBER: c_int = -21;
pub const CODES_VALUE_CANNOT_BE_MISSING: c_int = -22;
pub const CODES_WRONG_LENGTH: c_int = -23;
pub const CODES_INVALID_TYPE: c_int = -24;
pub const CODES_WRONG_STEP: c_int = -25;
pub const CODES_WRONG_STEP_UNIT: c_int = -26;
pub const CODES_INVALID_FILE: c_int = -27;
pub const CODES_INVALID_GRIB: c_int = -28;
pub const CODES_INVALID_INDEX: c_int = -29;
pub const CODES_INVALID_ITERATOR: c_int = -30;
pub const CODES_INVALID_KEYS_ITERATOR: c_int = -31;
pub const CODES_INVALID_NEAREST: c_int = -32;
pub const CODES_INVALID_ORDERBY: c_int = -33;
pub const CODES_MISSING_KEY: c_int = -34;
pub const CODES_OUT_OF_AREA: c_int = -35;
pub const CODES_CONCEPT_NO_MATCH: c_int = -36;
pub const CODES_HASH_ARRAY_NO_MATCH: c_int = -37;
pub const CODES_NO_DEFINITIONS: c_int = -38;
pub const CODES_WRONG_TYPE: c_int = -39;
pub const CODES_END: c_int = -40;
pub const CODES_NO_VALUES: c_int = -41;
pub const CODES_WRONG_GRID: c_int = -42;
pub const CODES_END_OF_INDEX: c_int = -43;
pub const CODES_NULL_INDEX: c_int = -44;
pub const CODES_PREMATURE_END_OF_FILE: c_int = -45;
pub const CODES_INTERNAL_ARRAY_TOO_SMALL: c_int = -46;
pub const CODES_MESSAGE_TOO_LARGE: c_int = -47;
pub const CODES_CONSTANT_FIELD: c_int = -48;
pub const CODES_SWITCH_NO_MATCH: c_int = -49;
pub const CODES_UNDERFLOW: c_int = -50;
pub const CODES_MESSAGE_MALFORMED: c_int = -51;
pub const CODES_CORRUPTED_INDEX: c_int = -52;
pub const CODES_INVALID_BPV: c_int = -53;
pub const CODES_DIFFERENT_EDITION: c_int = -54;
pub const CODES_VALUE_DIFFERENT: c_int = -55;
pub const CODES_INVALID_KEY_VALUE: c_int = -56;
pub const CODES_STRING_TOO_SMALL: c_int = -57;
pub const CODES_WRONG_CONVERSION: c_int = -58;
pub const CODES_MISSING_BUFR_ENTRY: c_int = -59;
pub const CODES_NULL_POINTER: c_int = -60;
pub const CODES_ATTRIBUTE_CLASH: c_int = -61;
pub const CODES_TOO_MANY_ATTRIBUTES: c_int = -62;
pub const CODES_ATTRIBUTE_NOT_FOUND: c_int = -63;
pub const CODES_UNSUPPORTED_EDITION: c_int = -64;
pub const CODES_OUT_OF_RANGE: c_int = -65;
pub const CODES_WRONG_BITMAP_SIZE: c_int = -66;
pub const CODES_FUNCTIONALITY_NOT_ENABLED: c_int = -67;

// Keys iterator flags
pub const CODES_KEYS_ITERATOR_ALL_KEYS: c_ulong = 0;
pub const CODES_KEYS_ITERATOR_SKIP_READ_ONLY: c_ulong = 1 << 0;
pub const CODES_KEYS_ITERATOR_SKIP_OPTIONAL: c_ulong = 1 << 1;
pub const CODES_KEYS_ITERATOR_SKIP_EDITION_SPECIFIC: c_ulong = 1 << 2;
pub const CODES_KEYS_ITERATOR_SKIP_CODED: c_ulong = 1 << 3;
pub const CODES_KEYS_ITERATOR_SKIP_COMPUTED: c_ulong = 1 << 4;
pub const CODES_KEYS_ITERATOR_SKIP_DUPLICATES: c_ulong = 1 << 5;
pub const CODES_KEYS_ITERATOR_SKIP_FUNCTION: c_ulong = 1 << 6;
pub const CODES_KEYS_ITERATOR_DUMP_ONLY: c_ulong = 1 << 7;

// Nearest flags
pub const CODES_NEAREST_SAME_GRID: c_ulong = 1 << 0;
pub const CODES_NEAREST_SAME_DATA: c_ulong = 1 << 1;
pub const CODES_NEAREST_SAME_POINT: c_ulong = 1 << 2;

// Geoiterator flags
pub const CODES_GEOITERATOR_NO_VALUES: c_ulong = 1 << 0;

// Missing value sentinels (must match grib_api.h exactly)
pub const CODES_MISSING_LONG: c_long = 2147483647;
pub const CODES_MISSING_DOUBLE: c_double = -1e+100;

// libc FILE type
#[repr(C)]
pub struct FILE {
    _opaque: [u8; 0],
}

extern "C" {
    pub fn fopen(filename: *const c_char, mode: *const c_char) -> *mut FILE;
    pub fn fclose(file: *mut FILE) -> c_int;
}

// ── FFI functions ────────────────────────────────────────────────────────────

extern "C" {
    // -- Version / info --
    pub fn codes_get_api_version() -> c_long;
    pub fn codes_get_git_sha1() -> *const c_char;
    pub fn codes_get_git_branch() -> *const c_char;
    pub fn codes_get_build_date() -> *const c_char;
    pub fn codes_get_package_name() -> *const c_char;
    pub fn codes_get_error_message(code: c_int) -> *const c_char;
    pub fn codes_get_type_name(type_: c_int) -> *const c_char;

    // -- Context --
    pub fn codes_context_get_default() -> *mut codes_context;
    pub fn codes_context_delete(c: *mut codes_context);
    pub fn codes_context_set_definitions_path(c: *mut codes_context, path: *const c_char);
    pub fn codes_context_set_samples_path(c: *mut codes_context, path: *const c_char);

    // -- Counting --
    pub fn codes_count_in_file(c: *mut codes_context, f: *mut FILE, n: *mut c_int) -> c_int;
    pub fn codes_count_in_filename(
        c: *mut codes_context,
        filename: *const c_char,
        n: *mut c_int,
    ) -> c_int;

    // -- Handle creation --
    pub fn codes_handle_new_from_file(
        c: *mut codes_context,
        f: *mut FILE,
        product: ProductKind,
        error: *mut c_int,
    ) -> *mut codes_handle;

    pub fn codes_grib_handle_new_from_file(
        c: *mut codes_context,
        f: *mut FILE,
        error: *mut c_int,
    ) -> *mut codes_handle;

    pub fn codes_bufr_handle_new_from_file(
        c: *mut codes_context,
        f: *mut FILE,
        error: *mut c_int,
    ) -> *mut codes_handle;

    pub fn codes_handle_new_from_message(
        c: *mut codes_context,
        data: *const c_void,
        data_len: usize,
    ) -> *mut codes_handle;

    pub fn codes_handle_new_from_message_copy(
        c: *mut codes_context,
        data: *const c_void,
        data_len: usize,
    ) -> *mut codes_handle;

    pub fn codes_handle_new_from_samples(
        c: *mut codes_context,
        sample_name: *const c_char,
    ) -> *mut codes_handle;

    pub fn codes_grib_handle_new_from_samples(
        c: *mut codes_context,
        sample_name: *const c_char,
    ) -> *mut codes_handle;

    pub fn codes_bufr_handle_new_from_samples(
        c: *mut codes_context,
        sample_name: *const c_char,
    ) -> *mut codes_handle;

    pub fn codes_handle_clone(h: *const codes_handle) -> *mut codes_handle;
    pub fn codes_handle_clone_headers_only(h: *const codes_handle) -> *mut codes_handle;

    // -- Handle deletion --
    pub fn codes_handle_delete(h: *mut codes_handle) -> c_int;

    // -- Get scalar --
    pub fn codes_get_long(
        h: *const codes_handle,
        key: *const c_char,
        value: *mut c_long,
    ) -> c_int;

    pub fn codes_get_double(
        h: *const codes_handle,
        key: *const c_char,
        value: *mut c_double,
    ) -> c_int;

    pub fn codes_get_float(
        h: *const codes_handle,
        key: *const c_char,
        value: *mut c_float,
    ) -> c_int;

    pub fn codes_get_string(
        h: *const codes_handle,
        key: *const c_char,
        value: *mut c_char,
        length: *mut usize,
    ) -> c_int;

    pub fn codes_get_bytes(
        h: *const codes_handle,
        key: *const c_char,
        bytes: *mut c_uchar,
        length: *mut usize,
    ) -> c_int;

    // -- Get array --
    pub fn codes_get_long_array(
        h: *const codes_handle,
        key: *const c_char,
        vals: *mut c_long,
        length: *mut usize,
    ) -> c_int;

    pub fn codes_get_double_array(
        h: *const codes_handle,
        key: *const c_char,
        vals: *mut c_double,
        length: *mut usize,
    ) -> c_int;

    pub fn codes_get_float_array(
        h: *const codes_handle,
        key: *const c_char,
        vals: *mut c_float,
        length: *mut usize,
    ) -> c_int;

    pub fn codes_get_string_array(
        h: *const codes_handle,
        key: *const c_char,
        vals: *mut *mut c_char,
        length: *mut usize,
    ) -> c_int;

    // -- Get element --
    pub fn codes_get_double_element(
        h: *const codes_handle,
        key: *const c_char,
        i: c_int,
        value: *mut c_double,
    ) -> c_int;

    pub fn codes_get_float_element(
        h: *const codes_handle,
        key: *const c_char,
        i: c_int,
        value: *mut c_float,
    ) -> c_int;

    pub fn codes_get_double_elements(
        h: *const codes_handle,
        key: *const c_char,
        index_array: *const c_int,
        size: c_long,
        value: *mut c_double,
    ) -> c_int;

    pub fn codes_get_float_elements(
        h: *const codes_handle,
        key: *const c_char,
        index_array: *const c_int,
        size: c_long,
        value: *mut c_float,
    ) -> c_int;

    // -- Get metadata --
    pub fn codes_get_size(
        h: *const codes_handle,
        key: *const c_char,
        size: *mut usize,
    ) -> c_int;

    pub fn codes_get_length(
        h: *const codes_handle,
        key: *const c_char,
        length: *mut usize,
    ) -> c_int;

    pub fn codes_get_offset(
        h: *const codes_handle,
        key: *const c_char,
        offset: *mut usize,
    ) -> c_int;

    pub fn codes_get_native_type(
        h: *const codes_handle,
        name: *const c_char,
        type_: *mut c_int,
    ) -> c_int;

    pub fn codes_is_missing(
        h: *const codes_handle,
        key: *const c_char,
        err: *mut c_int,
    ) -> c_int;

    pub fn codes_is_defined(h: *const codes_handle, key: *const c_char) -> c_int;

    pub fn codes_get_product_kind(
        h: *const codes_handle,
        product_kind: *mut ProductKind,
    ) -> c_int;

    // -- Set scalar --
    pub fn codes_set_long(
        h: *mut codes_handle,
        key: *const c_char,
        val: c_long,
    ) -> c_int;

    pub fn codes_set_double(
        h: *mut codes_handle,
        key: *const c_char,
        val: c_double,
    ) -> c_int;

    pub fn codes_set_string(
        h: *mut codes_handle,
        key: *const c_char,
        value: *const c_char,
        length: *mut usize,
    ) -> c_int;

    pub fn codes_set_bytes(
        h: *mut codes_handle,
        key: *const c_char,
        bytes: *const c_uchar,
        length: *mut usize,
    ) -> c_int;

    pub fn codes_set_missing(h: *mut codes_handle, key: *const c_char) -> c_int;

    // -- Set array --
    pub fn codes_set_long_array(
        h: *mut codes_handle,
        key: *const c_char,
        vals: *const c_long,
        length: usize,
    ) -> c_int;

    pub fn codes_set_double_array(
        h: *mut codes_handle,
        key: *const c_char,
        vals: *const c_double,
        length: usize,
    ) -> c_int;

    pub fn codes_set_float_array(
        h: *mut codes_handle,
        key: *const c_char,
        vals: *const c_float,
        length: usize,
    ) -> c_int;

    pub fn codes_set_string_array(
        h: *mut codes_handle,
        key: *const c_char,
        vals: *const *const c_char,
        length: usize,
    ) -> c_int;

    // -- Set force --
    pub fn codes_set_force_double_array(
        h: *mut codes_handle,
        key: *const c_char,
        vals: *const c_double,
        length: usize,
    ) -> c_int;

    pub fn codes_set_force_float_array(
        h: *mut codes_handle,
        key: *const c_char,
        vals: *const c_float,
        length: usize,
    ) -> c_int;

    // -- Write --
    pub fn codes_write_message(
        h: *const codes_handle,
        file: *const c_char,
        mode: *const c_char,
    ) -> c_int;

    pub fn codes_get_message(
        h: *const codes_handle,
        message: *mut *const c_void,
        message_length: *mut usize,
    ) -> c_int;

    pub fn codes_get_message_copy(
        h: *const codes_handle,
        message: *mut c_void,
        message_length: *mut usize,
    ) -> c_int;

    pub fn codes_get_message_offset(
        h: *const codes_handle,
        offset: *mut i64,
    ) -> c_int;

    pub fn codes_get_message_size(
        h: *const codes_handle,
        size: *mut usize,
    ) -> c_int;

    // -- Keys iterator --
    pub fn codes_keys_iterator_new(
        h: *mut codes_handle,
        filter_flags: c_ulong,
        name_space: *const c_char,
    ) -> *mut codes_keys_iterator;

    pub fn codes_keys_iterator_next(kiter: *mut codes_keys_iterator) -> c_int;

    pub fn codes_keys_iterator_get_name(
        kiter: *const codes_keys_iterator,
    ) -> *const c_char;

    pub fn codes_keys_iterator_delete(kiter: *mut codes_keys_iterator) -> c_int;

    pub fn codes_keys_iterator_rewind(kiter: *mut codes_keys_iterator) -> c_int;

    // -- Geo iterator --
    pub fn codes_grib_iterator_new(
        h: *const codes_handle,
        flags: c_ulong,
        error: *mut c_int,
    ) -> *mut codes_iterator;

    pub fn codes_grib_iterator_next(
        i: *mut codes_iterator,
        lat: *mut c_double,
        lon: *mut c_double,
        value: *mut c_double,
    ) -> c_int;

    pub fn codes_grib_iterator_previous(
        i: *mut codes_iterator,
        lat: *mut c_double,
        lon: *mut c_double,
        value: *mut c_double,
    ) -> c_int;

    pub fn codes_grib_iterator_has_next(i: *mut codes_iterator) -> c_int;
    pub fn codes_grib_iterator_reset(i: *mut codes_iterator) -> c_int;
    pub fn codes_grib_iterator_delete(i: *mut codes_iterator) -> c_int;

    pub fn codes_grib_get_data(
        h: *const codes_handle,
        lats: *mut c_double,
        lons: *mut c_double,
        values: *mut c_double,
    ) -> c_int;

    // -- Nearest --
    pub fn codes_grib_nearest_new(
        h: *const codes_handle,
        error: *mut c_int,
    ) -> *mut codes_nearest;

    pub fn codes_grib_nearest_find(
        nearest: *mut codes_nearest,
        h: *const codes_handle,
        inlat: c_double,
        inlon: c_double,
        flags: c_ulong,
        outlats: *mut c_double,
        outlons: *mut c_double,
        values: *mut c_double,
        distances: *mut c_double,
        indexes: *mut c_int,
        len: *mut usize,
    ) -> c_int;

    pub fn codes_grib_nearest_delete(nearest: *mut codes_nearest) -> c_int;

    pub fn codes_grib_nearest_find_multiple(
        h: *const codes_handle,
        is_lsm: c_int,
        inlats: *const c_double,
        inlons: *const c_double,
        npoints: c_long,
        outlats: *mut c_double,
        outlons: *mut c_double,
        values: *mut c_double,
        distances: *mut c_double,
        indexes: *mut c_int,
    ) -> c_int;

    // -- Index --
    pub fn codes_index_new(
        c: *mut codes_context,
        keys: *const c_char,
        err: *mut c_int,
    ) -> *mut codes_index;

    pub fn codes_index_new_from_file(
        c: *mut codes_context,
        filename: *const c_char,
        keys: *const c_char,
        err: *mut c_int,
    ) -> *mut codes_index;

    pub fn codes_index_add_file(
        index: *mut codes_index,
        filename: *const c_char,
    ) -> c_int;

    pub fn codes_index_read(
        c: *mut codes_context,
        filename: *const c_char,
        err: *mut c_int,
    ) -> *mut codes_index;

    pub fn codes_index_write(
        index: *mut codes_index,
        filename: *const c_char,
    ) -> c_int;

    pub fn codes_index_delete(index: *mut codes_index);

    pub fn codes_index_get_size(
        index: *const codes_index,
        key: *const c_char,
        size: *mut usize,
    ) -> c_int;

    pub fn codes_index_get_long(
        index: *const codes_index,
        key: *const c_char,
        values: *mut c_long,
        size: *mut usize,
    ) -> c_int;

    pub fn codes_index_get_double(
        index: *const codes_index,
        key: *const c_char,
        values: *mut c_double,
        size: *mut usize,
    ) -> c_int;

    pub fn codes_index_get_string(
        index: *const codes_index,
        key: *const c_char,
        values: *mut *mut c_char,
        size: *mut usize,
    ) -> c_int;

    pub fn codes_index_select_long(
        index: *mut codes_index,
        key: *const c_char,
        value: c_long,
    ) -> c_int;

    pub fn codes_index_select_double(
        index: *mut codes_index,
        key: *const c_char,
        value: c_double,
    ) -> c_int;

    pub fn codes_index_select_string(
        index: *mut codes_index,
        key: *const c_char,
        value: *const c_char,
    ) -> c_int;

    pub fn codes_handle_new_from_index(
        index: *mut codes_index,
        err: *mut c_int,
    ) -> *mut codes_handle;

    // -- Multi handle --
    pub fn codes_grib_multi_handle_new(
        c: *mut codes_context,
    ) -> *mut codes_multi_handle;

    pub fn codes_grib_multi_handle_append(
        h: *mut codes_handle,
        start_section: c_int,
        mh: *mut codes_multi_handle,
    ) -> c_int;

    pub fn codes_grib_multi_handle_delete(
        mh: *mut codes_multi_handle,
    ) -> c_int;

    pub fn codes_grib_multi_handle_write(
        mh: *mut codes_multi_handle,
        f: *mut FILE,
    ) -> c_int;

    // -- Copy / Namespace --
    pub fn codes_copy_namespace(
        dest: *mut codes_handle,
        name: *const c_char,
        src: *mut codes_handle,
    ) -> c_int;

    // -- BUFR --
    pub fn codes_bufr_copy_data(
        hin: *mut codes_handle,
        hout: *mut codes_handle,
    ) -> c_int;

    // -- Context config --
    pub fn codes_context_set_debug(c: *mut codes_context, mode: c_int);
    pub fn codes_grib_multi_support_on(c: *mut codes_context);
    pub fn codes_grib_multi_support_off(c: *mut codes_context);

    pub fn codes_samples_path(c: *const codes_context) -> *mut c_char;

    // -- Utilities --
    pub fn codes_get_gaussian_latitudes(
        truncation: c_long,
        latitudes: *mut c_double,
    ) -> c_int;

    pub fn codes_datetime_to_julian(
        year: c_long,
        month: c_long,
        day: c_long,
        hour: c_long,
        minute: c_long,
        second: c_long,
        jd: *mut c_double,
    ) -> c_int;

    pub fn codes_julian_to_datetime(
        jd: c_double,
        year: *mut c_long,
        month: *mut c_long,
        day: *mut c_long,
        hour: *mut c_long,
        minute: *mut c_long,
        second: *mut c_long,
    ) -> c_int;

    pub fn codes_julian_to_date(jdate: c_long) -> c_long;
    pub fn codes_date_to_julian(ddate: c_long) -> c_long;

    pub fn codes_dump_content(
        h: *const codes_handle,
        out: *mut FILE,
        mode: *const c_char,
        option_flags: c_ulong,
        arg: *mut c_void,
    );

    pub fn codes_key_is_computed(
        h: *const codes_handle,
        key: *const c_char,
        err: *mut c_int,
    ) -> c_int;

    pub fn codes_extract_offsets_malloc(
        c: *mut codes_context,
        filename: *const c_char,
        product: ProductKind,
        offsets: *mut *mut i64,
        num_messages: *mut c_int,
        strict_mode: c_int,
    ) -> c_int;
}
