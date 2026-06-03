//! Error types for the ecCodes Rust API.

use std::ffi::CStr;
use std::fmt;
use std::os::raw::c_int;

/// All ecCodes error codes mapped from the C library.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[non_exhaustive]
pub enum EcCodesError {
    EndOfFile,
    InternalError,
    BufferTooSmall,
    NotImplemented,
    SevenNotFound,
    ArrayTooSmall,
    FileNotFound,
    CodeNotFoundInTable,
    WrongArraySize,
    NotFound,
    IoProblem,
    InvalidMessage,
    DecodingError,
    EncodingError,
    NoMoreInSet,
    GeocalculusProblem,
    OutOfMemory,
    ReadOnly,
    InvalidArgument,
    NullHandle,
    InvalidSectionNumber,
    ValueCannotBeMissing,
    WrongLength,
    InvalidType,
    WrongStep,
    WrongStepUnit,
    InvalidFile,
    InvalidGrib,
    InvalidIndex,
    InvalidIterator,
    InvalidKeysIterator,
    InvalidNearest,
    InvalidOrderby,
    MissingKey,
    OutOfArea,
    ConceptNoMatch,
    HashArrayNoMatch,
    NoDefinitions,
    WrongType,
    End,
    NoValues,
    WrongGrid,
    EndOfIndex,
    NullIndex,
    PrematureEndOfFile,
    InternalArrayTooSmall,
    MessageTooLarge,
    ConstantField,
    SwitchNoMatch,
    Underflow,
    MessageMalformed,
    CorruptedIndex,
    InvalidBpv,
    DifferentEdition,
    ValueDifferent,
    InvalidKeyValue,
    StringTooSmall,
    WrongConversion,
    MissingBufrEntry,
    NullPointer,
    AttributeClash,
    TooManyAttributes,
    AttributeNotFound,
    UnsupportedEdition,
    OutOfRange,
    WrongBitmapSize,
    FunctionalityNotEnabled,
    /// An error code not (yet) mapped.
    Unknown(i32),
}

impl EcCodesError {
    /// Check a C return code.  Returns `Ok(())` for success (0).
    #[must_use]
    pub fn check(code: c_int) -> Result<()> {
        if code == 0 {
            return Ok(());
        }
        Err(Self::from_code(code))
    }

    /// Convert from a C error code.
    pub fn from_code(code: c_int) -> Self {
        use eccodes_sys::*;
        match code {
            CODES_END_OF_FILE => Self::EndOfFile,
            CODES_INTERNAL_ERROR => Self::InternalError,
            CODES_BUFFER_TOO_SMALL => Self::BufferTooSmall,
            CODES_NOT_IMPLEMENTED => Self::NotImplemented,
            CODES_7777_NOT_FOUND => Self::SevenNotFound,
            CODES_ARRAY_TOO_SMALL => Self::ArrayTooSmall,
            CODES_FILE_NOT_FOUND => Self::FileNotFound,
            CODES_CODE_NOT_FOUND_IN_TABLE => Self::CodeNotFoundInTable,
            CODES_WRONG_ARRAY_SIZE => Self::WrongArraySize,
            CODES_NOT_FOUND => Self::NotFound,
            CODES_IO_PROBLEM => Self::IoProblem,
            CODES_INVALID_MESSAGE => Self::InvalidMessage,
            CODES_DECODING_ERROR => Self::DecodingError,
            CODES_ENCODING_ERROR => Self::EncodingError,
            CODES_NO_MORE_IN_SET => Self::NoMoreInSet,
            CODES_GEOCALCULUS_PROBLEM => Self::GeocalculusProblem,
            CODES_OUT_OF_MEMORY => Self::OutOfMemory,
            CODES_READ_ONLY => Self::ReadOnly,
            CODES_INVALID_ARGUMENT => Self::InvalidArgument,
            CODES_NULL_HANDLE => Self::NullHandle,
            CODES_INVALID_SECTION_NUMBER => Self::InvalidSectionNumber,
            CODES_VALUE_CANNOT_BE_MISSING => Self::ValueCannotBeMissing,
            CODES_WRONG_LENGTH => Self::WrongLength,
            CODES_INVALID_TYPE => Self::InvalidType,
            CODES_WRONG_STEP => Self::WrongStep,
            CODES_WRONG_STEP_UNIT => Self::WrongStepUnit,
            CODES_INVALID_FILE => Self::InvalidFile,
            CODES_INVALID_GRIB => Self::InvalidGrib,
            CODES_INVALID_INDEX => Self::InvalidIndex,
            CODES_INVALID_ITERATOR => Self::InvalidIterator,
            CODES_INVALID_KEYS_ITERATOR => Self::InvalidKeysIterator,
            CODES_INVALID_NEAREST => Self::InvalidNearest,
            CODES_INVALID_ORDERBY => Self::InvalidOrderby,
            CODES_MISSING_KEY => Self::MissingKey,
            CODES_OUT_OF_AREA => Self::OutOfArea,
            CODES_CONCEPT_NO_MATCH => Self::ConceptNoMatch,
            CODES_HASH_ARRAY_NO_MATCH => Self::HashArrayNoMatch,
            CODES_NO_DEFINITIONS => Self::NoDefinitions,
            CODES_WRONG_TYPE => Self::WrongType,
            CODES_END => Self::End,
            CODES_NO_VALUES => Self::NoValues,
            CODES_WRONG_GRID => Self::WrongGrid,
            CODES_END_OF_INDEX => Self::EndOfIndex,
            CODES_NULL_INDEX => Self::NullIndex,
            CODES_PREMATURE_END_OF_FILE => Self::PrematureEndOfFile,
            CODES_INTERNAL_ARRAY_TOO_SMALL => Self::InternalArrayTooSmall,
            CODES_MESSAGE_TOO_LARGE => Self::MessageTooLarge,
            CODES_CONSTANT_FIELD => Self::ConstantField,
            CODES_SWITCH_NO_MATCH => Self::SwitchNoMatch,
            CODES_UNDERFLOW => Self::Underflow,
            CODES_MESSAGE_MALFORMED => Self::MessageMalformed,
            CODES_CORRUPTED_INDEX => Self::CorruptedIndex,
            CODES_INVALID_BPV => Self::InvalidBpv,
            CODES_DIFFERENT_EDITION => Self::DifferentEdition,
            CODES_VALUE_DIFFERENT => Self::ValueDifferent,
            CODES_INVALID_KEY_VALUE => Self::InvalidKeyValue,
            CODES_STRING_TOO_SMALL => Self::StringTooSmall,
            CODES_WRONG_CONVERSION => Self::WrongConversion,
            CODES_MISSING_BUFR_ENTRY => Self::MissingBufrEntry,
            CODES_NULL_POINTER => Self::NullPointer,
            CODES_ATTRIBUTE_CLASH => Self::AttributeClash,
            CODES_TOO_MANY_ATTRIBUTES => Self::TooManyAttributes,
            CODES_ATTRIBUTE_NOT_FOUND => Self::AttributeNotFound,
            CODES_UNSUPPORTED_EDITION => Self::UnsupportedEdition,
            CODES_OUT_OF_RANGE => Self::OutOfRange,
            CODES_WRONG_BITMAP_SIZE => Self::WrongBitmapSize,
            CODES_FUNCTIONALITY_NOT_ENABLED => Self::FunctionalityNotEnabled,
            _ => Self::Unknown(code),
        }
    }

    /// Get the C library error message for this error.
    pub fn message(&self) -> String {
        let code = self.to_code();
        unsafe {
            let p = eccodes_sys::codes_get_error_message(code);
            if p.is_null() {
                format!("Unknown error ({})", code)
            } else {
                CStr::from_ptr(p).to_string_lossy().into_owned()
            }
        }
    }

    fn to_code(&self) -> c_int {
        use eccodes_sys::*;
        match self {
            Self::EndOfFile => CODES_END_OF_FILE,
            Self::InternalError => CODES_INTERNAL_ERROR,
            Self::BufferTooSmall => CODES_BUFFER_TOO_SMALL,
            Self::NotImplemented => CODES_NOT_IMPLEMENTED,
            Self::SevenNotFound => CODES_7777_NOT_FOUND,
            Self::ArrayTooSmall => CODES_ARRAY_TOO_SMALL,
            Self::FileNotFound => CODES_FILE_NOT_FOUND,
            Self::CodeNotFoundInTable => CODES_CODE_NOT_FOUND_IN_TABLE,
            Self::WrongArraySize => CODES_WRONG_ARRAY_SIZE,
            Self::NotFound => CODES_NOT_FOUND,
            Self::IoProblem => CODES_IO_PROBLEM,
            Self::InvalidMessage => CODES_INVALID_MESSAGE,
            Self::DecodingError => CODES_DECODING_ERROR,
            Self::EncodingError => CODES_ENCODING_ERROR,
            Self::NoMoreInSet => CODES_NO_MORE_IN_SET,
            Self::GeocalculusProblem => CODES_GEOCALCULUS_PROBLEM,
            Self::OutOfMemory => CODES_OUT_OF_MEMORY,
            Self::ReadOnly => CODES_READ_ONLY,
            Self::InvalidArgument => CODES_INVALID_ARGUMENT,
            Self::NullHandle => CODES_NULL_HANDLE,
            Self::InvalidSectionNumber => CODES_INVALID_SECTION_NUMBER,
            Self::ValueCannotBeMissing => CODES_VALUE_CANNOT_BE_MISSING,
            Self::WrongLength => CODES_WRONG_LENGTH,
            Self::InvalidType => CODES_INVALID_TYPE,
            Self::WrongStep => CODES_WRONG_STEP,
            Self::WrongStepUnit => CODES_WRONG_STEP_UNIT,
            Self::InvalidFile => CODES_INVALID_FILE,
            Self::InvalidGrib => CODES_INVALID_GRIB,
            Self::InvalidIndex => CODES_INVALID_INDEX,
            Self::InvalidIterator => CODES_INVALID_ITERATOR,
            Self::InvalidKeysIterator => CODES_INVALID_KEYS_ITERATOR,
            Self::InvalidNearest => CODES_INVALID_NEAREST,
            Self::InvalidOrderby => CODES_INVALID_ORDERBY,
            Self::MissingKey => CODES_MISSING_KEY,
            Self::OutOfArea => CODES_OUT_OF_AREA,
            Self::ConceptNoMatch => CODES_CONCEPT_NO_MATCH,
            Self::HashArrayNoMatch => CODES_HASH_ARRAY_NO_MATCH,
            Self::NoDefinitions => CODES_NO_DEFINITIONS,
            Self::WrongType => CODES_WRONG_TYPE,
            Self::End => CODES_END,
            Self::NoValues => CODES_NO_VALUES,
            Self::WrongGrid => CODES_WRONG_GRID,
            Self::EndOfIndex => CODES_END_OF_INDEX,
            Self::NullIndex => CODES_NULL_INDEX,
            Self::PrematureEndOfFile => CODES_PREMATURE_END_OF_FILE,
            Self::InternalArrayTooSmall => CODES_INTERNAL_ARRAY_TOO_SMALL,
            Self::MessageTooLarge => CODES_MESSAGE_TOO_LARGE,
            Self::ConstantField => CODES_CONSTANT_FIELD,
            Self::SwitchNoMatch => CODES_SWITCH_NO_MATCH,
            Self::Underflow => CODES_UNDERFLOW,
            Self::MessageMalformed => CODES_MESSAGE_MALFORMED,
            Self::CorruptedIndex => CODES_CORRUPTED_INDEX,
            Self::InvalidBpv => CODES_INVALID_BPV,
            Self::DifferentEdition => CODES_DIFFERENT_EDITION,
            Self::ValueDifferent => CODES_VALUE_DIFFERENT,
            Self::InvalidKeyValue => CODES_INVALID_KEY_VALUE,
            Self::StringTooSmall => CODES_STRING_TOO_SMALL,
            Self::WrongConversion => CODES_WRONG_CONVERSION,
            Self::MissingBufrEntry => CODES_MISSING_BUFR_ENTRY,
            Self::NullPointer => CODES_NULL_POINTER,
            Self::AttributeClash => CODES_ATTRIBUTE_CLASH,
            Self::TooManyAttributes => CODES_TOO_MANY_ATTRIBUTES,
            Self::AttributeNotFound => CODES_ATTRIBUTE_NOT_FOUND,
            Self::UnsupportedEdition => CODES_UNSUPPORTED_EDITION,
            Self::OutOfRange => CODES_OUT_OF_RANGE,
            Self::WrongBitmapSize => CODES_WRONG_BITMAP_SIZE,
            Self::FunctionalityNotEnabled => CODES_FUNCTIONALITY_NOT_ENABLED,
            Self::Unknown(c) => *c,
        }
    }
}

impl fmt::Display for EcCodesError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "ecCodes error: {}", self.message())
    }
}

impl std::error::Error for EcCodesError {}

/// Convenience result type for ecCodes operations.
pub type Result<T> = std::result::Result<T, EcCodesError>;
