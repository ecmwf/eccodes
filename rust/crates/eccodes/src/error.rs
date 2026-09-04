//! Error handling for eccodes.
//!
//! One [`Error`] variant per `CODES_*` error code returned by the C library,
//! plus Rust-side conversion and I/O failures. `Display` goes through
//! `codes_get_error_message`, so messages match the C library exactly.

use std::ffi::{CStr, c_int};

use eccodes_sys as sys;

/// Message for a `CODES_*` error code via `codes_get_error_message`.
fn message(code: c_int) -> String {
    // SAFETY: valid for any code; unknown codes yield a static fallback
    // message.
    let ptr = unsafe { sys::codes_get_error_message(code) };
    if ptr.is_null() {
        return format!("eccodes error {code}");
    }
    // SAFETY: non-null NUL-terminated static string, never freed.
    unsafe { CStr::from_ptr(ptr) }
        .to_string_lossy()
        .into_owned()
}

macro_rules! codes_errors {
    ($($variant:ident = $code:ident),* $(,)?) => {
        /// Error from an eccodes operation.
        #[derive(Debug, thiserror::Error)]
        #[non_exhaustive]
        pub enum Error {
            $(
                #[doc = concat!("`", stringify!($code), "`")]
                #[error("{}", message(sys::$code))]
                $variant,
            )*
            /// A string returned by the library was not valid UTF-8.
            #[error(transparent)]
            Utf8(#[from] std::str::Utf8Error),
            /// An input string contained an interior NUL byte.
            #[error(transparent)]
            Nul(#[from] std::ffi::NulError),
            /// I/O error from the Rust side (file open, write, ...).
            #[error(transparent)]
            Io(#[from] std::io::Error),
            /// Error code unknown to this crate.
            #[error("{}", message(*.0))]
            Unknown(i32),
        }

        impl Error {
            /// Map a raw return code: `CODES_SUCCESS` is `Ok(())`, anything
            /// else the corresponding variant.
            pub(crate) const fn from_code(code: c_int) -> Result<()> {
                match code {
                    0 => Ok(()),
                    $(sys::$code => Err(Self::$variant),)*
                    other => Err(Self::Unknown(other)),
                }
            }
        }
    };
}

codes_errors! {
    EndOfFile               = CODES_END_OF_FILE,
    InternalError           = CODES_INTERNAL_ERROR,
    BufferTooSmall          = CODES_BUFFER_TOO_SMALL,
    NotImplemented          = CODES_NOT_IMPLEMENTED,
    Marker7777NotFound      = CODES_7777_NOT_FOUND,
    ArrayTooSmall           = CODES_ARRAY_TOO_SMALL,
    FileNotFound            = CODES_FILE_NOT_FOUND,
    CodeNotFoundInTable     = CODES_CODE_NOT_FOUND_IN_TABLE,
    WrongArraySize          = CODES_WRONG_ARRAY_SIZE,
    NotFound                = CODES_NOT_FOUND,
    IoProblem               = CODES_IO_PROBLEM,
    InvalidMessage          = CODES_INVALID_MESSAGE,
    DecodingError           = CODES_DECODING_ERROR,
    EncodingError           = CODES_ENCODING_ERROR,
    NoMoreInSet             = CODES_NO_MORE_IN_SET,
    GeocalculusProblem      = CODES_GEOCALCULUS_PROBLEM,
    OutOfMemory             = CODES_OUT_OF_MEMORY,
    ReadOnly                = CODES_READ_ONLY,
    InvalidArgument         = CODES_INVALID_ARGUMENT,
    NullHandle              = CODES_NULL_HANDLE,
    InvalidSectionNumber    = CODES_INVALID_SECTION_NUMBER,
    ValueCannotBeMissing    = CODES_VALUE_CANNOT_BE_MISSING,
    WrongLength             = CODES_WRONG_LENGTH,
    InvalidType             = CODES_INVALID_TYPE,
    WrongStep               = CODES_WRONG_STEP,
    WrongStepUnit           = CODES_WRONG_STEP_UNIT,
    InvalidFile             = CODES_INVALID_FILE,
    InvalidGrib             = CODES_INVALID_GRIB,
    InvalidIndex            = CODES_INVALID_INDEX,
    InvalidIterator         = CODES_INVALID_ITERATOR,
    InvalidKeysIterator     = CODES_INVALID_KEYS_ITERATOR,
    InvalidNearest          = CODES_INVALID_NEAREST,
    InvalidOrderby          = CODES_INVALID_ORDERBY,
    MissingKey              = CODES_MISSING_KEY,
    OutOfArea               = CODES_OUT_OF_AREA,
    ConceptNoMatch          = CODES_CONCEPT_NO_MATCH,
    HashArrayNoMatch        = CODES_HASH_ARRAY_NO_MATCH,
    NoDefinitions           = CODES_NO_DEFINITIONS,
    WrongType               = CODES_WRONG_TYPE,
    End                     = CODES_END,
    NoValues                = CODES_NO_VALUES,
    WrongGrid               = CODES_WRONG_GRID,
    EndOfIndex              = CODES_END_OF_INDEX,
    NullIndex               = CODES_NULL_INDEX,
    PrematureEndOfFile      = CODES_PREMATURE_END_OF_FILE,
    InternalArrayTooSmall   = CODES_INTERNAL_ARRAY_TOO_SMALL,
    MessageTooLarge         = CODES_MESSAGE_TOO_LARGE,
    ConstantField           = CODES_CONSTANT_FIELD,
    SwitchNoMatch           = CODES_SWITCH_NO_MATCH,
    Underflow               = CODES_UNDERFLOW,
    MessageMalformed        = CODES_MESSAGE_MALFORMED,
    CorruptedIndex          = CODES_CORRUPTED_INDEX,
    InvalidBpv              = CODES_INVALID_BPV,
    DifferentEdition        = CODES_DIFFERENT_EDITION,
    ValueDifferent          = CODES_VALUE_DIFFERENT,
    InvalidKeyValue         = CODES_INVALID_KEY_VALUE,
    StringTooSmall          = CODES_STRING_TOO_SMALL,
    WrongConversion         = CODES_WRONG_CONVERSION,
    MissingBufrEntry        = CODES_MISSING_BUFR_ENTRY,
    NullPointer             = CODES_NULL_POINTER,
    AttributeClash          = CODES_ATTRIBUTE_CLASH,
    TooManyAttributes       = CODES_TOO_MANY_ATTRIBUTES,
    AttributeNotFound       = CODES_ATTRIBUTE_NOT_FOUND,
    UnsupportedEdition      = CODES_UNSUPPORTED_EDITION,
    OutOfRange              = CODES_OUT_OF_RANGE,
    WrongBitmapSize         = CODES_WRONG_BITMAP_SIZE,
    FunctionalityNotEnabled = CODES_FUNCTIONALITY_NOT_ENABLED,
}

/// Result type alias for eccodes operations.
pub type Result<T> = std::result::Result<T, Error>;

/// Check the `CODES_*` return code of an `unsafe` sys call.
macro_rules! check {
    ($expr:expr) => {
        $crate::error::Error::from_code(unsafe { $expr })
    };
}
pub(crate) use check;
