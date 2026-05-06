// Stub bindings for documentation purposes.
//
// These bindings are used when building documentation on docs.rs
// where the actual C library is not available.

use std::os::raw::{c_char, c_double, c_int, c_long, c_ulong, c_void};

// Type definitions
pub type codes_context = c_void;
pub type codes_handle = c_void;
pub type codes_index = c_void;
pub type codes_keys_iterator = c_void;
pub type codes_bufr_keys_iterator = c_void;
pub type codes_values_iterator = c_void;
pub type codes_nearest = c_void;
pub type grib_context = c_void;
pub type grib_handle = c_void;
pub type grib_index = c_void;
pub type grib_keys_iterator = c_void;
pub type grib_nearest = c_void;
pub type grib_iterator = c_void;
pub type grib_multi_handle = c_void;
pub type FILE = c_void;

// Return codes
pub const CODES_SUCCESS: c_int = 0;
pub const CODES_END_OF_FILE: c_int = -1;
pub const CODES_INTERNAL_ERROR: c_int = -2;
pub const CODES_BUFFER_TOO_SMALL: c_int = -3;
pub const CODES_NOT_IMPLEMENTED: c_int = -4;
pub const CODES_NOT_FOUND: c_int = -10;
pub const CODES_END_OF_INDEX: c_int = -11;
pub const CODES_NULL_POINTER: c_int = -20;
pub const CODES_MISSING_KEY: c_int = -21;
pub const CODES_OUT_OF_MEMORY: c_int = -22;
pub const CODES_READ_ONLY: c_int = -23;
pub const CODES_INVALID_ARGUMENT: c_int = -24;
pub const CODES_NULL_HANDLE: c_int = -25;
pub const CODES_INVALID_SECTION_NUMBER: c_int = -26;
pub const CODES_VALUE_CANNOT_BE_MISSING: c_int = -27;
pub const CODES_WRONG_LENGTH: c_int = -28;
pub const CODES_INVALID_TYPE: c_int = -29;
pub const CODES_WRONG_STEP: c_int = -30;
pub const CODES_WRONG_STEP_UNIT: c_int = -31;
pub const CODES_INVALID_FILE: c_int = -32;
pub const CODES_INVALID_GRIB: c_int = -33;
pub const CODES_INVALID_INDEX: c_int = -34;
pub const CODES_INVALID_ITERATOR: c_int = -35;
pub const CODES_INVALID_KEYS_ITERATOR: c_int = -36;
pub const CODES_INVALID_NEAREST: c_int = -37;
pub const CODES_INVALID_ORDERBY: c_int = -38;
pub const CODES_MISSING_BUFR_ENTRY: c_int = -39;
pub const CODES_NULL_INDEX: c_int = -40;
pub const CODES_PREMATURE_END_OF_FILE: c_int = -41;
pub const CODES_INTERNAL_ARRAY_TOO_SMALL: c_int = -42;
pub const CODES_MESSAGE_TOO_LARGE: c_int = -43;
pub const CODES_CONSTANT_FIELD: c_int = -44;
pub const CODES_SWITCH_NO_MATCH: c_int = -45;
pub const CODES_UNDERFLOW: c_int = -46;
pub const CODES_MESSAGE_MALFORMED: c_int = -47;
pub const CODES_CORRUPTED_INDEX: c_int = -48;
pub const CODES_INVALID_BPV: c_int = -49;
pub const CODES_DIFFERENT_EDITION: c_int = -50;
pub const CODES_VALUE_DIFFERENT: c_int = -51;
pub const CODES_INVALID_KEY_VALUE: c_int = -52;
pub const CODES_STRING_TOO_SMALL: c_int = -53;
pub const CODES_WRONG_CONVERSION: c_int = -54;
pub const CODES_MISSING_BUFR_DESCRIPTOR: c_int = -55;
pub const CODES_OUT_OF_AREA: c_int = -56;
pub const CODES_CONCEPT_NO_MATCH: c_int = -57;
pub const CODES_HASH_ARRAY_NO_MATCH: c_int = -58;
pub const CODES_NO_DEFINITIONS: c_int = -59;
pub const CODES_WRONG_TYPE: c_int = -60;
pub const CODES_END: c_int = -61;
pub const CODES_NO_VALUES: c_int = -62;
pub const CODES_WRONG_GRID: c_int = -63;
pub const CODES_IO_PROBLEM: c_int = -64;
pub const CODES_NO_MORE_IN_SET: c_int = -65;
pub const CODES_ENCODING_ERROR: c_int = -66;
pub const CODES_DECODING_ERROR: c_int = -67;
pub const CODES_INVALID_MESSAGE: c_int = -68;
pub const CODES_MESSAGE_INCOMPLETE: c_int = -69;
pub const CODES_TOO_MANY_ATTRIBUTES: c_int = -70;
pub const CODES_ATTRIBUTE_NOT_FOUND: c_int = -71;
pub const CODES_UNSUPPORTED_EDITION: c_int = -72;
pub const CODES_NO_MATCH: c_int = -73;

// Product types
pub const PRODUCT_ANY: c_int = 0;
pub const PRODUCT_GRIB: c_int = 1;
pub const PRODUCT_BUFR: c_int = 2;
pub const PRODUCT_METAR: c_int = 3;
pub const PRODUCT_GTS: c_int = 4;
pub const PRODUCT_TAF: c_int = 5;

// ProductKind enum aliases (matches bindgen output)
pub const ProductKind_PRODUCT_ANY: c_int = PRODUCT_ANY;
pub const ProductKind_PRODUCT_GRIB: c_int = PRODUCT_GRIB;
pub const ProductKind_PRODUCT_BUFR: c_int = PRODUCT_BUFR;
pub const ProductKind_PRODUCT_METAR: c_int = PRODUCT_METAR;
pub const ProductKind_PRODUCT_GTS: c_int = PRODUCT_GTS;
pub const ProductKind_PRODUCT_TAF: c_int = PRODUCT_TAF;

// Type flags
pub const CODES_TYPE_UNDEFINED: c_int = 0;
pub const CODES_TYPE_LONG: c_int = 1;
pub const CODES_TYPE_DOUBLE: c_int = 2;
pub const CODES_TYPE_STRING: c_int = 3;
pub const CODES_TYPE_BYTES: c_int = 4;
pub const CODES_TYPE_SECTION: c_int = 5;
pub const CODES_TYPE_LABEL: c_int = 6;
pub const CODES_TYPE_MISSING: c_int = 7;

// Key iterator flags
pub const CODES_KEYS_ITERATOR_ALL_KEYS: c_ulong = 0;
pub const CODES_KEYS_ITERATOR_SKIP_READ_ONLY: c_ulong = 1;
pub const CODES_KEYS_ITERATOR_SKIP_OPTIONAL: c_ulong = 2;
pub const CODES_KEYS_ITERATOR_SKIP_EDITION_SPECIFIC: c_ulong = 4;
pub const CODES_KEYS_ITERATOR_SKIP_CODED: c_ulong = 8;
pub const CODES_KEYS_ITERATOR_SKIP_COMPUTED: c_ulong = 16;
pub const CODES_KEYS_ITERATOR_SKIP_DUPLICATES: c_ulong = 32;
pub const CODES_KEYS_ITERATOR_SKIP_FUNCTION: c_ulong = 64;

// Missing value
pub const CODES_MISSING_LONG: c_long = 2147483647;
pub const CODES_MISSING_DOUBLE: c_double = -1e100;

unsafe extern "C" {
    // Context functions
    pub fn codes_context_get_default() -> *mut codes_context;

    // Handle functions - file operations
    pub fn codes_handle_new_from_file(
        c: *mut codes_context,
        f: *mut FILE,
        product: c_int,
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

    // Handle functions - memory operations
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

    pub fn codes_handle_clone(h: *const codes_handle) -> *mut codes_handle;
    pub fn codes_handle_delete(h: *mut codes_handle) -> c_int;

    // Get functions
    pub fn codes_get_long(h: *const codes_handle, key: *const c_char, value: *mut c_long) -> c_int;
    pub fn codes_get_double(
        h: *const codes_handle,
        key: *const c_char,
        value: *mut c_double,
    ) -> c_int;
    pub fn codes_get_string(
        h: *const codes_handle,
        key: *const c_char,
        mesg: *mut c_char,
        length: *mut usize,
    ) -> c_int;
    pub fn codes_get_bytes(
        h: *const codes_handle,
        key: *const c_char,
        bytes: *mut u8,
        length: *mut usize,
    ) -> c_int;
    pub fn codes_get_double_array(
        h: *const codes_handle,
        key: *const c_char,
        vals: *mut c_double,
        length: *mut usize,
    ) -> c_int;
    pub fn codes_get_long_array(
        h: *const codes_handle,
        key: *const c_char,
        vals: *mut c_long,
        length: *mut usize,
    ) -> c_int;
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
    pub fn codes_get_native_type(
        h: *const codes_handle,
        key: *const c_char,
        type_: *mut c_int,
    ) -> c_int;

    // Set functions
    pub fn codes_set_long(h: *mut codes_handle, key: *const c_char, val: c_long) -> c_int;
    pub fn codes_set_double(h: *mut codes_handle, key: *const c_char, val: c_double) -> c_int;
    pub fn codes_set_string(
        h: *mut codes_handle,
        key: *const c_char,
        mesg: *const c_char,
        length: *mut usize,
    ) -> c_int;
    pub fn codes_set_bytes(
        h: *mut codes_handle,
        key: *const c_char,
        bytes: *const u8,
        length: *mut usize,
    ) -> c_int;
    pub fn codes_set_double_array(
        h: *mut codes_handle,
        key: *const c_char,
        vals: *const c_double,
        length: usize,
    ) -> c_int;
    pub fn codes_set_long_array(
        h: *mut codes_handle,
        key: *const c_char,
        vals: *const c_long,
        length: usize,
    ) -> c_int;

    // Message functions
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
    pub fn codes_get_message_size(
        h: *const codes_handle,
        size: *mut usize,
    ) -> c_int;
    pub fn codes_write_message(
        h: *const codes_handle,
        filename: *const c_char,
        mode: *const c_char,
    ) -> c_int;

    // Keys iterator
    pub fn codes_keys_iterator_new(
        h: *mut codes_handle,
        filter_flags: c_ulong,
        name_space: *const c_char,
    ) -> *mut codes_keys_iterator;
    pub fn codes_keys_iterator_next(kiter: *mut codes_keys_iterator) -> c_int;
    pub fn codes_keys_iterator_get_name(kiter: *const codes_keys_iterator) -> *const c_char;
    pub fn codes_keys_iterator_delete(kiter: *mut codes_keys_iterator) -> c_int;
    pub fn codes_keys_iterator_rewind(kiter: *mut codes_keys_iterator) -> c_int;

    // Index functions
    pub fn codes_index_new_from_file(
        c: *mut codes_context,
        filename: *const c_char,
        keys: *const c_char,
        err: *mut c_int,
    ) -> *mut codes_index;
    pub fn codes_index_new(
        c: *mut codes_context,
        keys: *const c_char,
        err: *mut c_int,
    ) -> *mut codes_index;
    pub fn codes_index_add_file(
        index: *mut codes_index,
        filename: *const c_char,
    ) -> c_int;
    pub fn codes_index_delete(index: *mut codes_index);
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

    // Error handling
    pub fn codes_get_error_message(code: c_int) -> *const c_char;

    // Nearest neighbor
    pub fn codes_grib_nearest_new(
        h: *const codes_handle,
        error: *mut c_int,
    ) -> *mut codes_nearest;
    pub fn codes_grib_nearest_delete(nearest: *mut codes_nearest) -> c_int;
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

    // Iterator
    pub fn codes_grib_iterator_new(
        h: *const codes_handle,
        flags: c_ulong,
        error: *mut c_int,
    ) -> *mut grib_iterator;
    pub fn codes_grib_iterator_next(
        iter: *mut grib_iterator,
        lat: *mut c_double,
        lon: *mut c_double,
        value: *mut c_double,
    ) -> c_int;
    pub fn codes_grib_iterator_delete(iter: *mut grib_iterator) -> c_int;

    // Multi-field support
    pub fn codes_grib_multi_handle_new(c: *mut codes_context) -> *mut grib_multi_handle;
    pub fn codes_grib_multi_handle_append(
        h: *mut codes_handle,
        start_section: c_int,
        mh: *mut grib_multi_handle,
    ) -> c_int;
    pub fn codes_grib_multi_handle_delete(mh: *mut grib_multi_handle) -> c_int;
    pub fn codes_grib_multi_handle_write(
        mh: *mut grib_multi_handle,
        f: *mut FILE,
    ) -> c_int;

    // Path configuration
    pub fn codes_context_set_definitions_path(
        c: *mut codes_context,
        path: *const c_char,
    );
    pub fn codes_context_set_samples_path(
        c: *mut codes_context,
        path: *const c_char,
    );

    // Count messages
    pub fn codes_count_in_file(
        c: *mut codes_context,
        f: *mut FILE,
        n: *mut c_int,
    ) -> c_int;
}
