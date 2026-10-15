//! The error types of `ScaleWeather` eccodes 0.15, mapped from [`eccodes::Error`].

use errno::Errno;
use num_derive::FromPrimitive;
use num_traits::FromPrimitive as _;
#[cfg(feature = "ndarray")]
use std::num;
use std::{ffi, io, str};
use thiserror::Error;

/// Errors returned by all functions in the crate.
#[derive(Error, Debug)]
pub enum CodesError {
    /// An ecCodes library function returned an error code.
    #[error("ecCodes function returned a non-zero code {0}")]
    Internal(#[from] CodesInternal),

    /// A libc function returned a non-zero error code.
    ///
    /// Kept for source compatibility; this layer never constructs it.
    #[error("libc function returned an error with code {0} and errno {1}")]
    LibcNonZero(i32, Errno),

    /// An issue while handling the file.
    #[error("Error occured while opening the file: {0}")]
    FileHandlingInterrupted(#[from] io::Error),

    /// The string cannot be parsed as valid UTF8.
    #[error("Cannot parse string as UTF8: {0}")]
    CstrUTF8(#[from] str::Utf8Error),

    /// The string cannot be converted into a `CString`.
    #[error("Cannot parse string as CString: {0}")]
    CStringNul(#[from] ffi::NulError),

    /// The `CString` returned by ecCodes cannot be converted into a Rust string.
    #[error("String returned by ecCodes is not nul terminated: {0}")]
    NulChar(#[from] ffi::FromBytesWithNulError),

    /// The requested key is not present in the message.
    #[error("The key is missing in present message")]
    MissingKey,

    /// The size of the requested key is lower than 1.
    #[error("Incorrect key size")]
    IncorrectKeySize,

    /// Tried to read an array as a number.
    #[error("Requested key size is incorrect")]
    WrongRequestedKeySize,

    /// Tried to checked-read a key in a non-native type.
    #[error("Requested key type is incorrect")]
    WrongRequestedKeyType,

    /// Cloning the message failed.
    #[error("Cannot clone the message")]
    CloneFailed,

    /// `CodesNearest::find_nearest` failed internally.
    #[error("Internal error occured while trying to find nearest points")]
    NearestFindFailed,

    /// The keys iterator could not be created.
    #[error("Cannot create or manipulate keys iterator")]
    KeysIteratorFailed,

    /// A null pointer was encountered where it should not be.
    #[error("Null pointer encountered where it should not be")]
    NullPtr,

    /// Conversion of a message to ndarray failed.
    #[cfg(feature = "ndarray")]
    #[error("error occured while converting CodesMessage to ndarray {0}")]
    NdarrayConvert(#[from] MessageNdarrayError),

    /// The values array element count exceeds `usize::MAX`.
    #[cfg(feature = "ndarray")]
    #[error(
        "CodesMessage contains to much elements in the values array to be converted into ndarray"
    )]
    TooMuchValues,

    /// ecCodes returned an error code not present in [`CodesInternal`].
    #[error("eccodes returned unrecognized error code: {0}")]
    UnrecognizedErrorCode(i32),

    /// ecCodes returned a native key type code not known to this crate.
    #[error("Unrecognized native key type code: {0}")]
    UnrecognizedKeyTypeCode(i32),
}

impl From<eccodes::Error> for CodesError {
    fn from(err: eccodes::Error) -> Self {
        if let Some(code) = err.code() {
            return CodesInternal::from_i32(code.as_raw()).map_or_else(
                || Self::UnrecognizedErrorCode(code.as_raw()),
                Self::Internal,
            );
        }
        if let Some(io_err) = err.io_error() {
            return Self::FileHandlingInterrupted(io::Error::new(io_err.kind(), err.to_string()));
        }
        // No code and no io::Error: a string-conversion failure inside the
        // official crate; the message is all that is left to carry over.
        Self::FileHandlingInterrupted(io::Error::other(err.to_string()))
    }
}

/// Errors returned by the ndarray conversion methods.
#[cfg(feature = "ndarray")]
#[derive(PartialEq, Clone, Error, Debug)]
pub enum MessageNdarrayError {
    /// A key necessary for the conversion has a different type than expected.
    #[error("Requested key {0} has a different type than expected")]
    UnexpectedKeyType(String),

    /// The length of the values array does not equal `Ni * Nj`.
    #[error("The length of the values array ({0}) is different than expected ({1})")]
    UnexpectedValuesLength(usize, usize),

    /// A key necessary for the conversion has a value out of expected range.
    #[error("Requested key {0} has a value out of expected range")]
    UnexpectedKeyValue(String),

    /// ndarray cannot create an array with the shape defined by `Ni` and `Nj`.
    #[error("Error occured while converting to ndarray: {0}")]
    InvalidShape(#[from] ndarray::ShapeError),

    /// Casting shape types failed.
    #[error(transparent)]
    IntCasting(#[from] num::TryFromIntError),
}

/// Errors returned by internal ecCodes library functions, as the `CODES_*` codes.
#[derive(Copy, Eq, PartialEq, Clone, Ord, PartialOrd, Hash, Error, Debug, FromPrimitive)]
#[allow(missing_docs)] // each variant's Display text is its documentation
pub enum CodesInternal {
    #[error("No error")]
    CodesSuccess = 0,
    #[error("End of resource reached")]
    CodesEndOfFile = -1,
    #[error("Internal error")]
    CodesInternalError = -2,
    #[error("Passed buffer is too small")]
    CodesBufferTooSmall = -3,
    #[error("Function not yet implemented")]
    CodesNotImplemented = -4,
    #[error("Missing 7777 at end of message")]
    Codes7777NotFound = -5,
    #[error("Passed array is too small")]
    CodesArrayTooSmall = -6,
    #[error("File not found")]
    CodesFileNotFound = -7,
    #[error("Code not found in code table")]
    CodesCodeNotFoundInTable = -8,
    #[error("Array size mismatch")]
    CodesWrongArraySize = -9,
    #[error("Key/value not found")]
    CodesNotFound = -10,
    #[error("Input output problem")]
    CodesIoProblem = -11,
    #[error("Message invalid")]
    CodesInvalidMessage = -12,
    #[error("Decoding invalid")]
    CodesDecodingError = -13,
    #[error("Encoding invalid")]
    CodesEncodingError = -14,
    #[error("Code cannot unpack because of string too small")]
    CodesNoMoreInSet = -15,
    #[error("Problem with calculation of geographic attributes")]
    CodesGeocalculusProblem = -16,
    #[error("Memory allocation error")]
    CodesOutOfMemory = -17,
    #[error("Value is read only")]
    CodesReadOnly = -18,
    #[error("Invalid argument")]
    CodesInvalidArgument = -19,
    #[error("Null handle")]
    CodesNullHandle = -20,
    #[error("Invalid section number")]
    CodesInvalidSectionNumber = -21,
    #[error("Value cannot be missing")]
    CodesValueCannotBeMissing = -22,
    #[error("Wrong message length")]
    CodesWrongLength = -23,
    #[error("Invalid key type")]
    CodesInvalidType = -24,
    #[error("Unable to set step")]
    CodesWrongStep = -25,
    #[error("Wrong units for step (step must be integer)")]
    CodesWrongStepUnit = -26,
    #[error("Invalid file id")]
    CodesInvalidFile = -27,
    #[error("Invalid grib id")]
    CodesInvalidGrib = -28,
    #[error("Invalid index id")]
    CodesInvalidIndex = -29,
    #[error("Invalid iterator id")]
    CodesInvalidIterator = -30,
    #[error("Invalid keys iterator id")]
    CodesInvalidKeysIterator = -31,
    #[error("Invalid nearest id")]
    CodesInvalidNearest = -32,
    #[error("Invalid order by")]
    CodesInvalidOrderby = -33,
    #[error("Missing a key from the fieldset")]
    CodesMissingKey = -34,
    #[error("The point is out of the grid area")]
    CodesOutOfArea = -35,
    #[error("Concept no match")]
    CodesConceptNoMatch = -36,
    #[error("Hash array no match")]
    CodesHashArrayNoMatch = -37,
    #[error("Definitions files not found")]
    CodesNoDefinitions = -38,
    #[error("Wrong type while packing")]
    CodesWrongType = -39,
    #[error("End of resource")]
    CodesEnd = -40,
    #[error("Unable to code a field without values")]
    CodesNoValues = -41,
    #[error("Grid description is wrong or inconsistent")]
    CodesWrongGrid = -42,
    #[error("End of index reached")]
    CodesEndOfIndex = -43,
    #[error("Null index")]
    CodesNullIndex = -44,
    #[error("End of resource reached when reading message")]
    CodesPrematureEndOfFile = -45,
    #[error("An internal array is too small")]
    CodesInternalArrayTooSmall = -46,
    #[error("Message is too large for the current architecture")]
    CodesMessageTooLarge = -47,
    #[error("Constant field")]
    CodesConstantField = -48,
    #[error("Switch unable to find a matching case")]
    CodesSwitchNoMatch = -49,
    #[error("Underflow")]
    CodesUnderflow = -50,
    #[error("Message malformed")]
    CodesMessageMalformed = -51,
    #[error("Index is corrupted")]
    CodesCorruptedIndex = -52,
    #[error("Invalid number of bits per value")]
    CodesInvalidBpv = -53,
    #[error("Edition of two messages is different")]
    CodesDifferentEdition = -54,
    #[error("Value is different")]
    CodesValueDifferent = -55,
    #[error("Invalid key value")]
    CodesInvalidKeyValue = -56,
    #[error("String is smaller than requested")]
    CodesStringTooSmall = -57,
    #[error("Wrong type conversion")]
    CodesWrongConversion = -58,
    #[error("Missing BUFR table entry for descriptor")]
    CodesMissingBufrEntry = -59,
    #[error("Null pointer")]
    CodesNullPointer = -60,
    #[error("Attribute is already present =  cannot add")]
    CodesAttributeClash = -61,
    #[error("Too many attributes. Increase MAX_ACCESSOR_ATTRIBUTES")]
    CodesTooManyAttributes = -62,
    #[error("Attribute not found")]
    CodesAttributeNotFound = -63,
    #[error("Edition not supported")]
    CodesUnsupportedEdition = -64,
    #[error("Value out of coding range")]
    CodesOutOfRange = -65,
    #[error("Size of bitmap is incorrect")]
    CodesWrongBitmapSize = -66,
    #[error("Functionality not enabled")]
    CodesFunctionalityNotEnabled = -67,
}
