//! Typed key access: what [`Message::get`] and [`Message::set`] dispatch on.
//!
//! The type you ask for picks the decoding:
//!
//! ```no_run
//! # fn main() -> eccodes::Result<()> {
//! # let mut message: eccodes::Message = unimplemented!();
//! let edition: i64 = message.get("edition")?;
//! let name: String = message.get("shortName")?;
//! let values: Vec<f64> = message.get("values")?;
//! let level: Option<i64> = message.get("levelist")?;   // None if absent or missing
//!
//! message.set("centre", "ecmf")?;
//! message.set("values", values.as_slice())?;
//! message.set("scaleFactorOfFirstFixedSurface", None::<i64>)?;   // code as missing
//! # Ok(())
//! # }
//! ```

use std::ffi::{c_char, c_int, c_long};
use std::fmt;

use eccodes_sys as sys;

use crate::error::{Code, Error, ErrorContext, Result, check};
use crate::ffi;
use crate::kind::MessageKind;
use crate::message::Message;

// `long` keys cross the boundary as `i64`, so arrays are passed pointer-wise
// rather than element-by-element. True on every supported target.
const _: () = assert!(size_of::<c_long>() == size_of::<i64>());

/// How a key is stored in a message (`CODES_TYPE_*`).
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
#[non_exhaustive]
pub enum KeyType {
    /// No type — the key exists but codes nothing.
    Undefined,
    /// Integer (`CODES_TYPE_LONG`).
    I64,
    /// Floating point (`CODES_TYPE_DOUBLE`).
    F64,
    /// Text.
    String,
    /// Raw octets.
    Bytes,
    /// A section boundary.
    Section,
    /// A label.
    Label,
    /// The key's missing value.
    Missing,
}

impl KeyType {
    #[allow(clippy::cast_sign_loss)] // negative codes simply match nothing
    pub(crate) const fn from_raw(raw: c_int) -> Option<Self> {
        match raw as u32 {
            sys::CODES_TYPE_UNDEFINED => Some(Self::Undefined),
            sys::CODES_TYPE_LONG => Some(Self::I64),
            sys::CODES_TYPE_DOUBLE => Some(Self::F64),
            sys::CODES_TYPE_STRING => Some(Self::String),
            sys::CODES_TYPE_BYTES => Some(Self::Bytes),
            sys::CODES_TYPE_SECTION => Some(Self::Section),
            sys::CODES_TYPE_LABEL => Some(Self::Label),
            sys::CODES_TYPE_MISSING => Some(Self::Missing),
            _ => None,
        }
    }

    #[allow(clippy::cast_possible_wrap)] // the CODES_TYPE_* values are small
    pub(crate) const fn as_raw(self) -> c_int {
        (match self {
            Self::Undefined => sys::CODES_TYPE_UNDEFINED,
            Self::I64 => sys::CODES_TYPE_LONG,
            Self::F64 => sys::CODES_TYPE_DOUBLE,
            Self::String => sys::CODES_TYPE_STRING,
            Self::Bytes => sys::CODES_TYPE_BYTES,
            Self::Section => sys::CODES_TYPE_SECTION,
            Self::Label => sys::CODES_TYPE_LABEL,
            Self::Missing => sys::CODES_TYPE_MISSING,
        }) as c_int
    }
}

impl fmt::Display for KeyType {
    /// The library's own name for the type — `"long"`, `"double"`, … — so
    /// diagnostics read the same as the C tools'.
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        // SAFETY: defined for any type code; returns a static string, with a
        // fallback for codes it does not know.
        let name = unsafe { ffi::static_str(sys::codes_get_type_name(self.as_raw())) };
        f.write_str(name.unwrap_or("unknown"))
    }
}

/// A type a key can be read as — see [`Message::get`].
pub trait KeyGet: Sized {
    /// Read `key` from `message`.
    fn get_from<K: MessageKind>(message: &Message<K>, key: &str) -> Result<Self>;
}

/// A type a key can be written from — see [`Message::set`].
pub trait KeySet {
    /// Write `self` to `key` of `message`.
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()>;
}

/// An array element type — see [`Message::element`].
pub trait KeyElement: Sized {
    /// Read one element of `key`.
    fn element_from<K: MessageKind>(message: &Message<K>, key: &str, index: usize) -> Result<Self>;

    /// Read selected elements of `key`.
    fn elements_from<K: MessageKind>(
        message: &Message<K>,
        key: &str,
        indexes: &[usize],
    ) -> Result<Vec<Self>>;
}

/// An array type that can be written over the library's read-only flag — see
/// [`Message::set_forced`].
pub trait KeyForce: Sized {
    /// Write `values` to `key`, ignoring that the key is read-only.
    fn set_forced_on<K: MessageKind>(
        values: &[Self],
        message: &mut Message<K>,
        key: &str,
    ) -> Result<()>;
}

// ---- reading ----

impl KeyGet for i64 {
    fn get_from<K: MessageKind>(message: &Message<K>, key: &str) -> Result<Self> {
        let ckey = ffi::cstring(key)?;
        let mut value: Self = 0;
        check!(sys::codes_get_long(
            message.as_ptr(),
            ckey.as_ptr(),
            &raw mut value
        ))
        .with_key(key)?;
        Ok(value)
    }
}

impl KeyGet for f64 {
    fn get_from<K: MessageKind>(message: &Message<K>, key: &str) -> Result<Self> {
        let ckey = ffi::cstring(key)?;
        let mut value = 0.0_f64;
        check!(sys::codes_get_double(
            message.as_ptr(),
            ckey.as_ptr(),
            &raw mut value
        ))
        .with_key(key)?;
        Ok(value)
    }
}

impl KeyGet for f32 {
    fn get_from<K: MessageKind>(message: &Message<K>, key: &str) -> Result<Self> {
        let ckey = ffi::cstring(key)?;
        let mut value = 0.0_f32;
        check!(sys::codes_get_float(
            message.as_ptr(),
            ckey.as_ptr(),
            &raw mut value
        ))
        .with_key(key)?;
        Ok(value)
    }
}

impl KeyGet for bool {
    /// Reads the library's `0`/`1` convention for flag keys such as
    /// `bitmapPresent`.
    fn get_from<K: MessageKind>(message: &Message<K>, key: &str) -> Result<Self> {
        Ok(i64::get_from(message, key)? != 0)
    }
}

impl KeyGet for String {
    fn get_from<K: MessageKind>(message: &Message<K>, key: &str) -> Result<Self> {
        let ckey = ffi::cstring(key)?;
        // One more than the key's own length, for the NUL the library appends.
        let mut len = message.key_string_len(key)? + 1;
        let mut buf = vec![0_u8; len];
        check!(sys::codes_get_string(
            message.as_ptr(),
            ckey.as_ptr(),
            buf.as_mut_ptr().cast::<c_char>(),
            &raw mut len,
        ))
        .with_key(key)?;
        // `len` comes back counting the NUL — except for the accessors that
        // leave it at the buffer size (`kindOfProduct` among them), so cut at
        // the first NUL as well.
        buf.truncate(len.saturating_sub(1));
        if let Some(nul) = buf.iter().position(|&byte| byte == 0) {
            buf.truncate(nul);
        }
        Self::from_utf8(buf).map_err(Error::from).with_key(key)
    }
}

impl KeyGet for Vec<u8> {
    fn get_from<K: MessageKind>(message: &Message<K>, key: &str) -> Result<Self> {
        let ckey = ffi::cstring(key)?;
        // `codes_get_size` counts elements, not bytes, so the first guess can
        // undershoot — `codedValues` packs several bytes per element. On
        // ArrayTooSmall the library writes the byte count it wants into `len`.
        let mut len = message.key_len(key)?;
        let mut buf = vec![0_u8; len];
        let mut outcome = check!(sys::codes_get_bytes(
            message.as_ptr(),
            ckey.as_ptr(),
            buf.as_mut_ptr(),
            &raw mut len,
        ));
        if outcome.as_ref().err().and_then(Error::code) == Some(Code::ArrayTooSmall) {
            buf = vec![0_u8; len];
            outcome = check!(sys::codes_get_bytes(
                message.as_ptr(),
                ckey.as_ptr(),
                buf.as_mut_ptr(),
                &raw mut len,
            ));
        }
        outcome.with_key(key)?;
        buf.truncate(len);
        Ok(buf)
    }
}

impl KeyGet for Vec<i64> {
    fn get_from<K: MessageKind>(message: &Message<K>, key: &str) -> Result<Self> {
        let ckey = ffi::cstring(key)?;
        let mut len = message.key_len(key)?;
        let mut values = vec![0_i64; len];
        check!(sys::codes_get_long_array(
            message.as_ptr(),
            ckey.as_ptr(),
            values.as_mut_ptr().cast::<c_long>(),
            &raw mut len,
        ))
        .with_key(key)?;
        values.truncate(len);
        Ok(values)
    }
}

impl KeyGet for Vec<f64> {
    fn get_from<K: MessageKind>(message: &Message<K>, key: &str) -> Result<Self> {
        let ckey = ffi::cstring(key)?;
        let mut len = message.key_len(key)?;
        let mut values = vec![0.0_f64; len];
        check!(sys::codes_get_double_array(
            message.as_ptr(),
            ckey.as_ptr(),
            values.as_mut_ptr(),
            &raw mut len,
        ))
        .with_key(key)?;
        values.truncate(len);
        Ok(values)
    }
}

impl KeyGet for Vec<f32> {
    fn get_from<K: MessageKind>(message: &Message<K>, key: &str) -> Result<Self> {
        let ckey = ffi::cstring(key)?;
        let mut len = message.key_len(key)?;
        let mut values = vec![0.0_f32; len];
        check!(sys::codes_get_float_array(
            message.as_ptr(),
            ckey.as_ptr(),
            values.as_mut_ptr(),
            &raw mut len,
        ))
        .with_key(key)?;
        values.truncate(len);
        Ok(values)
    }
}

impl KeyGet for Vec<String> {
    fn get_from<K: MessageKind>(message: &Message<K>, key: &str) -> Result<Self> {
        let ckey = ffi::cstring(key)?;
        let mut len = message.key_len(key)?;
        // The library allocates each string; we own them from here on.
        let mut ptrs: Vec<*mut c_char> = vec![std::ptr::null_mut(); len];
        check!(sys::codes_get_string_array(
            message.as_ptr(),
            ckey.as_ptr(),
            ptrs.as_mut_ptr(),
            &raw mut len,
        ))
        .with_key(key)?;
        ptrs.truncate(len);
        // SAFETY: every non-null entry is a malloc'd NUL-terminated string the
        // library handed over and no longer tracks.
        unsafe { ffi::take_strings(&ptrs) }.with_key(key)
    }
}

impl<T: KeyGet> KeyGet for Option<T> {
    /// `None` when the key is absent or coded as missing.
    fn get_from<K: MessageKind>(message: &Message<K>, key: &str) -> Result<Self> {
        if !message.contains_key(key) {
            return Ok(None);
        }
        // A key the library refuses to answer "is it missing?" for is one we
        // simply read: its own error will say more than this one would.
        if message.is_value_missing(key).unwrap_or(false) {
            return Ok(None);
        }
        T::get_from(message, key).map(Some)
    }
}

// ---- writing ----

impl KeySet for i64 {
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()> {
        let ckey = ffi::cstring(key)?;
        check!(sys::codes_set_long(message.as_ptr(), ckey.as_ptr(), self)).with_key(key)
    }
}

impl KeySet for f64 {
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()> {
        let ckey = ffi::cstring(key)?;
        check!(sys::codes_set_double(message.as_ptr(), ckey.as_ptr(), self)).with_key(key)
    }
}

impl KeySet for f32 {
    /// Written through `codes_set_double` — the C API has no scalar float
    /// setter, and widening an `f32` is exact.
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()> {
        f64::from(self).set_on(message, key)
    }
}

impl KeySet for bool {
    /// Writes the library's `0`/`1` convention for flag keys.
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()> {
        i64::from(self).set_on(message, key)
    }
}

impl KeySet for &str {
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()> {
        let ckey = ffi::cstring(key)?;
        let value = ffi::cstring(self)?;
        let mut len = self.len();
        check!(sys::codes_set_string(
            message.as_ptr(),
            ckey.as_ptr(),
            value.as_ptr(),
            &raw mut len,
        ))
        .with_key(key)
    }
}

impl KeySet for &String {
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()> {
        self.as_str().set_on(message, key)
    }
}

impl KeySet for String {
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()> {
        self.as_str().set_on(message, key)
    }
}

impl KeySet for &[u8] {
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()> {
        let ckey = ffi::cstring(key)?;
        let mut len = self.len();
        check!(sys::codes_set_bytes(
            message.as_ptr(),
            ckey.as_ptr(),
            self.as_ptr(),
            &raw mut len,
        ))
        .with_key(key)
    }
}

impl KeySet for &[i64] {
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()> {
        let ckey = ffi::cstring(key)?;
        check!(sys::codes_set_long_array(
            message.as_ptr(),
            ckey.as_ptr(),
            self.as_ptr().cast::<c_long>(),
            self.len(),
        ))
        .with_key(key)
    }
}

impl KeySet for &[f64] {
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()> {
        let ckey = ffi::cstring(key)?;
        check!(sys::codes_set_double_array(
            message.as_ptr(),
            ckey.as_ptr(),
            self.as_ptr(),
            self.len(),
        ))
        .with_key(key)
    }
}

impl KeySet for &[f32] {
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()> {
        let ckey = ffi::cstring(key)?;
        check!(sys::codes_set_float_array(
            message.as_ptr(),
            ckey.as_ptr(),
            self.as_ptr(),
            self.len(),
        ))
        .with_key(key)
    }
}

impl KeySet for &[&str] {
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()> {
        let ckey = ffi::cstring(key)?;
        let values = self
            .iter()
            .map(|value| ffi::cstring(value))
            .collect::<Result<Vec<_>>>()?;
        let mut ptrs: Vec<*const c_char> = values.iter().map(|value| value.as_ptr()).collect();
        check!(sys::codes_set_string_array(
            message.as_ptr(),
            ckey.as_ptr(),
            ptrs.as_mut_ptr(),
            ptrs.len(),
        ))
        .with_key(key)
    }
}

impl<T: KeySet> KeySet for Option<T> {
    /// `None` codes the key as missing.
    fn set_on<K: MessageKind>(self, message: &mut Message<K>, key: &str) -> Result<()> {
        match self {
            Some(value) => value.set_on(message, key),
            None => message.set_missing(key),
        }
    }
}

// ---- elements ----

impl KeyElement for f64 {
    fn element_from<K: MessageKind>(message: &Message<K>, key: &str, index: usize) -> Result<Self> {
        let ckey = ffi::cstring(key)?;
        let index = ffi::to_c_int(index).with_key(key)?;
        let mut value = 0.0_f64;
        check!(sys::codes_get_double_element(
            message.as_ptr(),
            ckey.as_ptr(),
            index,
            &raw mut value,
        ))
        .with_key(key)?;
        Ok(value)
    }

    fn elements_from<K: MessageKind>(
        message: &Message<K>,
        key: &str,
        indexes: &[usize],
    ) -> Result<Vec<Self>> {
        let ckey = ffi::cstring(key)?;
        let indexes = to_c_indexes(indexes, key)?;
        let mut values = vec![0.0_f64; indexes.len()];
        check!(sys::codes_get_double_elements(
            message.as_ptr(),
            ckey.as_ptr(),
            indexes.as_ptr(),
            ffi::to_c_long(indexes.len()).with_key(key)?,
            values.as_mut_ptr(),
        ))
        .with_key(key)?;
        Ok(values)
    }
}

impl KeyElement for f32 {
    fn element_from<K: MessageKind>(message: &Message<K>, key: &str, index: usize) -> Result<Self> {
        let ckey = ffi::cstring(key)?;
        let index = ffi::to_c_int(index).with_key(key)?;
        let mut value = 0.0_f32;
        check!(sys::codes_get_float_element(
            message.as_ptr(),
            ckey.as_ptr(),
            index,
            &raw mut value,
        ))
        .with_key(key)?;
        Ok(value)
    }

    fn elements_from<K: MessageKind>(
        message: &Message<K>,
        key: &str,
        indexes: &[usize],
    ) -> Result<Vec<Self>> {
        let ckey = ffi::cstring(key)?;
        let indexes = to_c_indexes(indexes, key)?;
        let mut values = vec![0.0_f32; indexes.len()];
        check!(sys::codes_get_float_elements(
            message.as_ptr(),
            ckey.as_ptr(),
            indexes.as_ptr(),
            ffi::to_c_long(indexes.len()).with_key(key)?,
            values.as_mut_ptr(),
        ))
        .with_key(key)?;
        Ok(values)
    }
}

fn to_c_indexes(indexes: &[usize], key: &str) -> Result<Vec<c_int>> {
    indexes
        .iter()
        .map(|&index| ffi::to_c_int(index).with_key(key))
        .collect()
}

// ---- forced writes ----

impl KeyForce for f64 {
    fn set_forced_on<K: MessageKind>(
        values: &[Self],
        message: &mut Message<K>,
        key: &str,
    ) -> Result<()> {
        let ckey = ffi::cstring(key)?;
        check!(sys::codes_set_force_double_array(
            message.as_ptr(),
            ckey.as_ptr(),
            values.as_ptr(),
            values.len(),
        ))
        .with_key(key)
    }
}

impl KeyForce for f32 {
    fn set_forced_on<K: MessageKind>(
        values: &[Self],
        message: &mut Message<K>,
        key: &str,
    ) -> Result<()> {
        let ckey = ffi::cstring(key)?;
        check!(sys::codes_set_force_float_array(
            message.as_ptr(),
            ckey.as_ptr(),
            values.as_ptr(),
            values.len(),
        ))
        .with_key(key)
    }
}
