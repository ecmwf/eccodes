//! Errors.
//!
//! [`Error`] is opaque: it carries the [`Code`] the C library returned plus
//! the key or file the call was about, so a failure says *what* failed and
//! *where* — `eccodes::NotFound: Key/value not found (key "shortName")`
//! rather than a bare `NotFound`.
//!
//! The `eccodes::<Variant>: <message>` rendering matches the `eckit::<Class>:
//! <message>` convention of the sibling wrappers; unlike eckit's C++
//! exceptions, the eccodes C API returns bare status codes with no per-call
//! message, so the context after the message is added by this crate.
//!
//! Match on [`Error::code`] — adding context never changes it:
//!
//! ```no_run
//! use eccodes::{Code, Message};
//!
//! # fn main() -> eccodes::Result<()> {
//! # let message: Message = unimplemented!();
//! match message.get::<f64>("levelist") {
//!     Ok(level) => println!("{level}"),
//!     Err(e) if e.code() == Some(Code::NotFound) => println!("no level"),
//!     Err(e) => return Err(e),
//! }
//! # Ok(())
//! # }
//! ```

use std::ffi::{CStr, c_int};
use std::fmt;
use std::path::{Path, PathBuf};

use eccodes_sys as sys;

/// Result of an eccodes operation.
pub type Result<T> = std::result::Result<T, Error>;

macro_rules! codes_codes {
    ($($variant:ident = $code:ident),* $(,)?) => {
        /// A `CODES_*` status code from the C library.
        #[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
        #[non_exhaustive]
        pub enum Code {
            $(
                #[doc = concat!("`", stringify!($code), "`")]
                $variant,
            )*
            /// A code this crate does not know, as returned by the library.
            Unknown(i32),
        }

        impl Code {
            /// The raw `CODES_*` value.
            #[must_use]
            pub const fn as_raw(self) -> i32 {
                match self {
                    $(Self::$variant => sys::$code,)*
                    Self::Unknown(code) => code,
                }
            }

            /// The variant for a raw `CODES_*` value.
            ///
            /// `CODES_SUCCESS` is not an error, and maps to
            /// `Code::Unknown(0)` here; the crate never builds an [`Error`]
            /// from it.
            #[must_use]
            pub const fn from_raw(code: i32) -> Self {
                match code {
                    $(sys::$code => Self::$variant,)*
                    other => Self::Unknown(other),
                }
            }

            /// The variant's name, as it appears in [`Display`](fmt::Display).
            #[must_use]
            pub const fn name(self) -> &'static str {
                match self {
                    $(Self::$variant => stringify!($variant),)*
                    Self::Unknown(_) => "Unknown",
                }
            }
        }
    };
}

codes_codes! {
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

impl Code {
    /// The library's message for this code (`codes_get_error_message`).
    #[must_use]
    pub fn message(self) -> &'static str {
        // SAFETY: defined for any code; returns a pointer into the library's
        // static message table (with a fallback for unknown codes).
        let message = unsafe { sys::codes_get_error_message(self.as_raw() as c_int) };
        if message.is_null() {
            return "unknown eccodes error";
        }
        // SAFETY: non-null, NUL-terminated, static — never freed.
        unsafe { CStr::from_ptr(message) }
            .to_str()
            .unwrap_or("unknown eccodes error")
    }
}

impl fmt::Display for Code {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Unknown(code) => write!(f, "eccodes::Unknown({code}): {}", self.message()),
            _ => write!(f, "eccodes::{}: {}", self.name(), self.message()),
        }
    }
}

/// An eccodes operation failed.
///
/// Carries the [`Code`] the library returned (where there was one) and the
/// key or file the operation was about. Inspect it with
/// [`code`](Self::code), [`key`](Self::key), [`path`](Self::path) and
/// [`io_error`](Self::io_error).
pub struct Error {
    // Boxed so `Result<T>` stays pointer-sized on the happy path.
    repr: Box<Repr>,
}

#[derive(Debug)]
struct Repr {
    cause: Cause,
    context: Context,
}

#[derive(Debug, thiserror::Error)]
enum Cause {
    #[error("{0}")]
    Codes(Code),
    #[error("eccodes::Io: {0}")]
    Io(#[from] std::io::Error),
    #[error("eccodes::Utf8: {0}")]
    Utf8(#[from] std::str::Utf8Error),
    #[error("eccodes::Nul: {0}")]
    Nul(#[from] std::ffi::NulError),
}

#[derive(Debug)]
enum Context {
    None,
    Key(String),
    Path(PathBuf),
}

impl Error {
    /// The `CODES_*` code the library returned, if the failure came from it
    /// rather than from I/O or string conversion on the Rust side.
    #[must_use]
    pub fn code(&self) -> Option<Code> {
        match self.repr.cause {
            Cause::Codes(code) => Some(code),
            _ => None,
        }
    }

    /// The key the failing operation was about, if any.
    #[must_use]
    pub fn key(&self) -> Option<&str> {
        match &self.repr.context {
            Context::Key(key) => Some(key),
            _ => None,
        }
    }

    /// The file the failing operation was about, if any.
    #[must_use]
    pub fn path(&self) -> Option<&Path> {
        match &self.repr.context {
            Context::Path(path) => Some(path),
            _ => None,
        }
    }

    /// The underlying I/O error, if this failure came from I/O.
    #[must_use]
    pub fn io_error(&self) -> Option<&std::io::Error> {
        match &self.repr.cause {
            Cause::Io(err) => Some(err),
            _ => None,
        }
    }

    /// Map a raw `CODES_*` return value: `CODES_SUCCESS` is `Ok(())`,
    /// anything else an error with that [`Code`].
    pub(crate) fn from_raw(code: c_int) -> Result<()> {
        if code == 0 {
            return Ok(());
        }
        Err(Self::from(Code::from_raw(code)))
    }

    /// The last OS error — for a failed `fopen`/`fdopen`/`open_memstream`.
    pub(crate) fn last_os_error() -> Self {
        Self::from(std::io::Error::last_os_error())
    }

    fn with_context(mut self, context: Context) -> Self {
        // Innermost context wins: the call that failed knows best what it was
        // doing; outer layers only fill in where nothing was recorded.
        if matches!(self.repr.context, Context::None) {
            self.repr.context = context;
        }
        self
    }
}

impl From<Code> for Error {
    fn from(code: Code) -> Self {
        Self {
            repr: Box::new(Repr {
                cause: Cause::Codes(code),
                context: Context::None,
            }),
        }
    }
}

macro_rules! from_cause {
    ($($ty:ty => $variant:ident),* $(,)?) => {
        $(
            impl From<$ty> for Error {
                fn from(source: $ty) -> Self {
                    Self {
                        repr: Box::new(Repr {
                            cause: Cause::$variant(source),
                            context: Context::None,
                        }),
                    }
                }
            }
        )*
    };
}

from_cause! {
    std::io::Error => Io,
    std::str::Utf8Error => Utf8,
    std::ffi::NulError => Nul,
}

impl From<std::string::FromUtf8Error> for Error {
    fn from(source: std::string::FromUtf8Error) -> Self {
        Self::from(source.utf8_error())
    }
}

impl From<time::error::ComponentRange> for Error {
    /// A date or time that cannot exist is an invalid key value — the same
    /// answer as for a message that codes one.
    ///
    /// This crate's API takes and returns [`time`] types, so callers build
    /// them in the same functions that return [`Result`]; without this,
    /// `Date::from_calendar_date(..)?` would not compile there.
    fn from(_: time::error::ComponentRange) -> Self {
        Self::from(Code::InvalidKeyValue)
    }
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.repr.cause)?;
        match &self.repr.context {
            Context::None => Ok(()),
            Context::Key(key) => write!(f, " (key {key:?})"),
            Context::Path(path) => write!(f, " (file {})", path.display()),
        }
    }
}

impl fmt::Debug for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let mut out = f.debug_struct("Error");
        match &self.repr.cause {
            Cause::Codes(code) => out.field("code", code),
            Cause::Io(err) => out.field("io", err),
            Cause::Utf8(err) => out.field("utf8", err),
            Cause::Nul(err) => out.field("nul", err),
        };
        match &self.repr.context {
            Context::None => {}
            Context::Key(key) => {
                out.field("key", key);
            }
            Context::Path(path) => {
                out.field("path", path);
            }
        }
        out.field("message", &self.to_string()).finish()
    }
}

impl std::error::Error for Error {
    fn source(&self) -> Option<&(dyn std::error::Error + 'static)> {
        match &self.repr.cause {
            // The code *is* this error — there is nothing underneath it.
            Cause::Codes(_) => None,
            Cause::Io(err) => Some(err),
            Cause::Utf8(err) => Some(err),
            Cause::Nul(err) => Some(err),
        }
    }
}

/// Record the key or file an operation was about on its error.
///
/// Crate-internal: every FFI call site says what it was working on, so users
/// never have to reconstruct it from a bare status code.
pub trait ErrorContext<T> {
    /// Record the key this operation was about.
    fn with_key(self, key: &str) -> Result<T>;

    /// Record the file this operation was about.
    fn with_path(self, path: impl AsRef<Path>) -> Result<T>;
}

impl<T, E: Into<Error>> ErrorContext<T> for std::result::Result<T, E> {
    fn with_key(self, key: &str) -> Result<T> {
        self.map_err(|err| err.into().with_context(Context::Key(key.to_owned())))
    }

    fn with_path(self, path: impl AsRef<Path>) -> Result<T> {
        self.map_err(|err| {
            err.into()
                .with_context(Context::Path(path.as_ref().to_path_buf()))
        })
    }
}

/// Check the `CODES_*` return value of an `unsafe` sys call.
macro_rules! check {
    ($expr:expr) => {
        $crate::error::Error::from_raw(unsafe { $expr })
    };
}
pub(crate) use check;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn result_stays_pointer_sized() {
        // The boxed representation is what keeps every `Result<T>` in the
        // crate from growing to the width of the largest error payload.
        assert_eq!(size_of::<Error>(), size_of::<*const ()>());
        assert_eq!(size_of::<Result<()>>(), size_of::<*const ()>());
    }

    #[test]
    fn display_carries_variant_library_message_and_key() {
        let err = Err::<(), _>(Code::NotFound)
            .with_key("shortName")
            .expect_err("constructed from an error");
        assert_eq!(
            err.to_string(),
            format!(
                "eccodes::NotFound: {} (key \"shortName\")",
                Code::NotFound.message()
            )
        );
    }

    #[test]
    fn innermost_context_wins() {
        // A key recorded where the call failed must survive an outer layer
        // labelling the same error with the file it was reading.
        let err = Err::<(), _>(Code::NotFound)
            .with_key("shortName")
            .with_path("/data/x.grib2")
            .expect_err("constructed from an error");
        assert_eq!(err.key(), Some("shortName"));
        assert_eq!(err.path(), None);
    }

    #[test]
    fn context_does_not_disturb_the_code() {
        let err = Err::<(), _>(Code::ReadOnly)
            .with_key("values")
            .expect_err("constructed from an error");
        assert_eq!(err.code(), Some(Code::ReadOnly));
        assert!(err.io_error().is_none());
    }

    #[test]
    fn io_errors_keep_their_source_for_the_error_chain() {
        use std::error::Error as _;

        let err: Error = std::io::Error::from(std::io::ErrorKind::PermissionDenied).into();
        assert_eq!(err.code(), None);
        assert!(err.source().is_some());
        assert_eq!(
            err.io_error().map(std::io::Error::kind),
            Some(std::io::ErrorKind::PermissionDenied)
        );
    }

    #[test]
    fn success_is_not_an_error() {
        assert!(Error::from_raw(0).is_ok());
        assert!(Error::from_raw(sys::CODES_END_OF_FILE).is_err());
    }
}
