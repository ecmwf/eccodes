//! [`Message`] — one GRIB, BUFR or other WMO message.

use std::ffi::{c_int, c_void};
use std::fmt;
use std::io::Write;
use std::marker::PhantomData;
use std::mem::ManuallyDrop;
use std::ptr::{self, NonNull};

use eccodes_sys as sys;

use crate::error::{Code, Error, ErrorContext, Result, check};
use crate::ffi;
use crate::key::{KeyElement, KeyForce, KeyGet, KeySet, KeyType};
use crate::keys::KeysQuery;
use crate::kind::{Any, Bufr, Grib, Kind, MessageKind};

/// One message: a GRIB field, a BUFR observation set, a METAR report.
///
/// The type parameter records the product at compile time — see
/// [`kind`](crate::kind). Plain `Message` is `Message<Any>` and covers
/// anything; [`GribMessage`] and [`BufrMessage`] unlock the product-specific
/// API.
///
/// Keys are read and written by name, typed by the value you ask for:
///
/// ```no_run
/// use eccodes::GribFile;
///
/// # fn main() -> eccodes::Result<()> {
/// for message in &GribFile::open("data.grib2")? {
///     let mut message = message?;
///     let name: String = message.get("shortName")?;
///     let values: Vec<f64> = message.get("values")?;
///     message.set("centre", "ecmf")?;
///     println!("{name}: {} values", values.len());
/// }
/// # Ok(())
/// # }
/// ```
pub struct Message<K: MessageKind = Any> {
    raw: NonNull<sys::codes_handle>,
    _kind: PhantomData<K>,
}

/// A GRIB message — [`Message<Grib>`].
pub type GribMessage = Message<Grib>;

/// A BUFR message — [`Message<Bufr>`].
pub type BufrMessage = Message<Bufr>;

// SAFETY: a message owns its C object exclusively, and the C library lets a
// handle move between threads. Deliberately not `Sync`: handles cache decoded
// accessors internally, so even a `&self` call mutates them.
unsafe impl<K: MessageKind> Send for Message<K> {}

impl<K: MessageKind> Drop for Message<K> {
    fn drop(&mut self) {
        // SAFETY: a valid handle owned by us, freed exactly once; the return
        // code carries nothing actionable during destruction.
        unsafe { sys::codes_handle_delete(self.raw.as_ptr()) };
    }
}

impl<K: MessageKind> Message<K> {
    pub(crate) const fn from_raw(raw: NonNull<sys::codes_handle>) -> Self {
        Self {
            raw,
            _kind: PhantomData,
        }
    }

    pub(crate) const fn as_ptr(&self) -> *mut sys::codes_handle {
        self.raw.as_ptr()
    }

    /// Decode a message from its raw bytes, copying them.
    ///
    /// Fails with [`Code::InvalidMessage`] if the bytes do not hold the
    /// product this type is tagged with.
    pub fn from_bytes(bytes: &[u8]) -> Result<Self> {
        if let Some(expected) = K::EXPECTED
            && Kind::from_bytes(bytes) != expected
        {
            return Err(Error::from(Code::InvalidMessage));
        }
        // SAFETY: a NULL context selects the default one; `bytes` is valid for
        // its own length and the library copies out of it before returning.
        let raw = unsafe {
            sys::codes_handle_new_from_message_copy(
                ptr::null_mut(),
                bytes.as_ptr().cast::<c_void>(),
                bytes.len(),
            )
        };
        NonNull::new(raw)
            .map(Self::from_raw)
            .ok_or_else(|| Error::from(Code::InvalidMessage))
    }

    /// The product this message actually holds, from its framing.
    ///
    /// For [`GribMessage`] and [`BufrMessage`] the answer is settled by the
    /// type; it is on `Message<Any>` that this earns its keep.
    pub fn kind(&self) -> Result<Kind> {
        Ok(Kind::from_bytes(self.as_bytes()?))
    }

    // ---- keys ----

    /// Read a key, decoded as `T`.
    ///
    /// `T` may be `i64`, `f64`, `f32`, `String`, `Vec<_>` of those, or
    /// `Option<T>` — which is `None` when the key is undefined or coded as
    /// missing, instead of an error.
    pub fn get<T: KeyGet>(&self, key: &str) -> Result<T> {
        T::get_from(self, key)
    }

    /// Write a key.
    ///
    /// `None` sets the key to its missing value, the same as
    /// [`set_missing`](Self::set_missing).
    pub fn set<T: KeySet>(&mut self, key: &str, value: T) -> Result<()> {
        value.set_on(self, key)
    }

    /// Write an array over the library's objection that the key is read-only
    /// (`codes_set_force_*_array`), as `codedValues` requires.
    pub fn set_forced<T: KeyForce>(&mut self, key: &str, values: &[T]) -> Result<()> {
        T::set_forced_on(values, self, key)
    }

    /// Read one element of an array key, without decoding the rest.
    pub fn element<T: KeyElement>(&self, key: &str, index: usize) -> Result<T> {
        T::element_from(self, key, index)
    }

    /// Read selected elements of an array key, without decoding the rest.
    pub fn elements<T: KeyElement>(&self, key: &str, indexes: &[usize]) -> Result<Vec<T>> {
        T::elements_from(self, key, indexes)
    }

    /// Whether the message defines this key.
    ///
    /// A name the C API could never look up — one containing a NUL byte — is
    /// simply not defined.
    #[must_use]
    pub fn contains_key(&self, key: &str) -> bool {
        let Ok(ckey) = ffi::cstring(key) else {
            return false;
        };
        // SAFETY: valid handle and NUL-terminated key.
        unsafe { sys::codes_is_defined(self.as_ptr(), ckey.as_ptr()) != 0 }
    }

    /// Whether the key is present but its value is coded as missing.
    pub fn is_value_missing(&self, key: &str) -> Result<bool> {
        let ckey = ffi::cstring(key)?;
        let mut status: c_int = 0;
        // SAFETY: valid handle, NUL-terminated key, out-pointer to a local.
        let missing =
            unsafe { sys::codes_is_missing(self.as_ptr(), ckey.as_ptr(), &raw mut status) };
        Error::from_raw(status).with_key(key)?;
        Ok(missing != 0)
    }

    /// Whether the key is computed by the definitions rather than coded in
    /// the message.
    pub fn is_computed(&self, key: &str) -> Result<bool> {
        let ckey = ffi::cstring(key)?;
        let mut status: c_int = 0;
        // SAFETY: valid handle, NUL-terminated key, out-pointer to a local.
        let computed =
            unsafe { sys::codes_key_is_computed(self.as_ptr(), ckey.as_ptr(), &raw mut status) };
        Error::from_raw(status).with_key(key)?;
        Ok(computed != 0)
    }

    /// Code the key as missing.
    pub fn set_missing(&mut self, key: &str) -> Result<()> {
        let ckey = ffi::cstring(key)?;
        check!(sys::codes_set_missing(self.as_ptr(), ckey.as_ptr())).with_key(key)
    }

    /// How the key is stored in the message.
    pub fn key_type(&self, key: &str) -> Result<KeyType> {
        let ckey = ffi::cstring(key)?;
        let mut raw: c_int = 0;
        check!(sys::codes_get_native_type(
            self.as_ptr(),
            ckey.as_ptr(),
            &raw mut raw
        ))
        .with_key(key)?;
        KeyType::from_raw(raw)
            .ok_or(Code::InvalidType)
            .with_key(key)
    }

    /// How many elements the key holds — 1 for a scalar, N for an array.
    pub fn key_len(&self, key: &str) -> Result<usize> {
        let ckey = ffi::cstring(key)?;
        let mut len: usize = 0;
        check!(sys::codes_get_size(
            self.as_ptr(),
            ckey.as_ptr(),
            &raw mut len
        ))
        .with_key(key)?;
        Ok(len)
    }

    /// How many bytes the key's string form occupies.
    pub fn key_string_len(&self, key: &str) -> Result<usize> {
        let ckey = ffi::cstring(key)?;
        let mut len: usize = 0;
        check!(sys::codes_get_length(
            self.as_ptr(),
            ckey.as_ptr(),
            &raw mut len
        ))
        .with_key(key)?;
        Ok(len)
    }

    /// Where the key sits in the message, in bytes from its start.
    pub fn key_offset(&self, key: &str) -> Result<usize> {
        let ckey = ffi::cstring(key)?;
        let mut offset: usize = 0;
        check!(sys::codes_get_offset(
            self.as_ptr(),
            ckey.as_ptr(),
            &raw mut offset
        ))
        .with_key(key)?;
        Ok(offset)
    }

    /// The keys of this message, as a query you can narrow before iterating.
    ///
    /// ```no_run
    /// # fn main() -> eccodes::Result<()> {
    /// # let message: eccodes::Message = unimplemented!();
    /// for key in message.keys().namespace("ls") {
    ///     println!("{}", key?);
    /// }
    /// # Ok(())
    /// # }
    /// ```
    #[must_use]
    pub const fn keys(&self) -> KeysQuery<'_> {
        KeysQuery::new(self.as_ptr())
    }

    /// Copy every key of a namespace out of `source` into this message.
    pub fn copy_namespace_from(
        &mut self,
        source: &Message<impl MessageKind>,
        namespace: &str,
    ) -> Result<()> {
        let cnamespace = ffi::cstring(namespace)?;
        check!(sys::codes_copy_namespace(
            self.as_ptr(),
            cnamespace.as_ptr(),
            source.as_ptr(),
        ))
        .with_key(namespace)
    }

    // ---- the message itself ----

    /// The message's raw bytes, borrowed from the message.
    pub fn as_bytes(&self) -> Result<&[u8]> {
        let mut bytes: *const c_void = ptr::null();
        let mut len: usize = 0;
        check!(sys::codes_get_message(
            self.as_ptr(),
            &raw mut bytes,
            &raw mut len
        ))?;
        if bytes.is_null() {
            return Err(Error::from(Code::NullPointer));
        }
        // SAFETY: the library handed us `len` readable bytes it owns; the
        // `&self` borrow keeps the message, and therefore that buffer, alive
        // for as long as the slice.
        Ok(unsafe { std::slice::from_raw_parts(bytes.cast::<u8>(), len) })
    }

    /// The message's raw bytes, copied out.
    pub fn to_vec(&self) -> Result<Vec<u8>> {
        let mut len = self.byte_len()?;
        let mut buf = vec![0_u8; len];
        check!(sys::codes_get_message_copy(
            self.as_ptr(),
            buf.as_mut_ptr().cast::<c_void>(),
            &raw mut len,
        ))?;
        buf.truncate(len);
        Ok(buf)
    }

    /// The message's size in bytes.
    pub fn byte_len(&self) -> Result<usize> {
        let mut len: usize = 0;
        check!(sys::codes_get_message_size(self.as_ptr(), &raw mut len))?;
        Ok(len)
    }

    /// Where this message started in the file it was read from.
    pub fn file_offset(&self) -> Result<u64> {
        // Typed by the C signature (`off_t`), so this module names no libc type.
        let mut offset = 0;
        check!(sys::codes_get_message_offset(
            self.as_ptr(),
            &raw mut offset
        ))?;
        u64::try_from(offset).map_err(|_| Error::from(Code::InternalError))
    }

    /// Write the message's bytes to `out`.
    pub fn write_to(&self, mut out: impl Write) -> Result<()> {
        Ok(out.write_all(self.as_bytes()?)?)
    }

    /// Write the message's contents to `out` in the library's `wmo` dump
    /// format — what `grib_dump` prints.
    pub fn dump_to(&self, out: impl Write) -> Result<()> {
        ffi::with_memstream(out, |stream| {
            // SAFETY: valid handle, open stream, NUL-terminated mode string;
            // a NULL argument list selects the default dump options.
            unsafe {
                sys::codes_dump_content(self.as_ptr(), stream, c"wmo".as_ptr(), 0, ptr::null_mut());
            }
            Ok(())
        })
    }

    /// Copy the message, keys and data alike.
    pub fn try_clone(&self) -> Result<Self> {
        // SAFETY: valid handle; the copy is independently owned.
        let raw = unsafe { sys::codes_handle_clone(self.as_ptr()) };
        NonNull::new(raw)
            .map(Self::from_raw)
            .ok_or_else(|| Error::from(Code::NullHandle))
    }

    /// Copy the message's headers, replacing the data section with dummy
    /// values — much cheaper than [`try_clone`](Self::try_clone) when only
    /// metadata matters.
    pub fn try_clone_headers_only(&self) -> Result<Self> {
        // SAFETY: valid handle; the copy is independently owned.
        let raw = unsafe { sys::codes_handle_clone_headers_only(self.as_ptr()) };
        NonNull::new(raw)
            .map(Self::from_raw)
            .ok_or_else(|| Error::from(Code::NullHandle))
    }

    /// Re-tag this message as another product, keeping the same C object.
    pub(crate) fn retag<T: MessageKind>(self) -> Message<T> {
        // The handle moves out of `self` without running its destructor: the
        // new tag takes over ownership of the same C object.
        let this = ManuallyDrop::new(self);
        Message::from_raw(this.raw)
    }

    /// The underlying `codes_handle`, for handing this message to another
    /// library built on eccodes.
    ///
    /// The message keeps ownership: the pointer is valid while it lives, and
    /// must not be deleted.
    #[cfg(feature = "raw")]
    #[must_use]
    pub const fn as_raw(&self) -> *mut sys::codes_handle {
        self.raw.as_ptr()
    }
}

impl<K: MessageKind> fmt::Debug for Message<K> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self.byte_len() {
            Ok(len) => write!(f, "Message<{}> {{ bytes: {len} }}", K::NAME),
            Err(_) => write!(f, "Message<{}> {{ bytes: ? }}", K::NAME),
        }
    }
}

impl Message<Any> {
    /// Build a message from one of the library's samples, e.g. `"GRIB2"` or
    /// `"BUFR4"` — see [`Library::samples_path`](crate::Library::samples_path).
    pub fn from_sample(name: impl AsRef<str>) -> Result<Self> {
        let name = name.as_ref();
        let cname = ffi::cstring(name)?;
        // SAFETY: a NULL context selects the default one; `cname` is
        // NUL-terminated and only read.
        let raw = unsafe { sys::codes_handle_new_from_samples(ptr::null_mut(), cname.as_ptr()) };
        NonNull::new(raw)
            .map(Self::from_raw)
            .ok_or_else(|| Error::from(Code::FileNotFound))
            .with_path(name)
    }

    /// Re-tag as a GRIB message, or hand it back unchanged.
    ///
    /// ```no_run
    /// # fn main() -> eccodes::Result<()> {
    /// # let message: eccodes::Message = unimplemented!();
    /// let Ok(grib) = message.try_into_grib() else {
    ///     return Ok(());   // not a GRIB message
    /// };
    /// println!("{:?}", grib.values()?.len());
    /// # Ok(())
    /// # }
    /// ```
    pub fn try_into_grib(self) -> std::result::Result<GribMessage, WrongKind> {
        self.retag_checked(Kind::Grib)
    }

    /// Re-tag as a BUFR message, or hand it back unchanged.
    pub fn try_into_bufr(self) -> std::result::Result<BufrMessage, WrongKind> {
        self.retag_checked(Kind::Bufr)
    }

    fn retag_checked<T: MessageKind>(
        self,
        expected: Kind,
    ) -> std::result::Result<Message<T>, WrongKind> {
        // A message whose bytes cannot be read is not the kind we want.
        let actual = self.kind().unwrap_or(Kind::Unknown);
        if actual == expected {
            Ok(self.retag())
        } else {
            Err(WrongKind {
                message: self,
                expected,
                actual,
            })
        }
    }
}

impl GribMessage {
    /// Build a GRIB message from a sample, e.g. `"GRIB2"`.
    pub fn from_sample(name: impl AsRef<str>) -> Result<Self> {
        let name = name.as_ref();
        let cname = ffi::cstring(name)?;
        // SAFETY: a NULL context selects the default one; `cname` is
        // NUL-terminated and only read.
        let raw =
            unsafe { sys::codes_grib_handle_new_from_samples(ptr::null_mut(), cname.as_ptr()) };
        NonNull::new(raw)
            .map(Self::from_raw)
            .ok_or_else(|| Error::from(Code::FileNotFound))
            .with_path(name)
    }

    /// Forget the GRIB tag — free, since it is the same C object.
    #[must_use]
    pub fn into_any(self) -> Message {
        self.retag()
    }
}

impl BufrMessage {
    /// Build a BUFR message from a sample, e.g. `"BUFR4"`.
    pub fn from_sample(name: impl AsRef<str>) -> Result<Self> {
        let name = name.as_ref();
        let cname = ffi::cstring(name)?;
        // SAFETY: a NULL context selects the default one; `cname` is
        // NUL-terminated and only read.
        let raw =
            unsafe { sys::codes_bufr_handle_new_from_samples(ptr::null_mut(), cname.as_ptr()) };
        NonNull::new(raw)
            .map(Self::from_raw)
            .ok_or_else(|| Error::from(Code::FileNotFound))
            .with_path(name)
    }

    /// Expand the data section so its element keys can be read.
    ///
    /// BUFR keeps observations packed until asked; every read of a data key
    /// needs this first.
    pub fn unpack(&mut self) -> Result<()> {
        self.set("unpack", 1_i64)
    }

    /// Copy the data section values out of `source` into this message.
    pub fn copy_data_from(&mut self, source: &Self) -> Result<()> {
        check!(sys::codes_bufr_copy_data(source.as_ptr(), self.as_ptr()))
    }

    /// Forget the BUFR tag — free, since it is the same C object.
    #[must_use]
    pub fn into_any(self) -> Message {
        self.retag()
    }
}

/// A message was not the product it was asked to be.
///
/// Returned by [`Message::try_into_grib`], [`Message::try_into_bufr`] and the
/// matching [`TryFrom`] impls, carrying the message so nothing is lost.
pub struct WrongKind {
    message: Message<Any>,
    expected: Kind,
    actual: Kind,
}

impl WrongKind {
    /// The product that was asked for.
    #[must_use]
    pub const fn expected(&self) -> Kind {
        self.expected
    }

    /// The product the message actually holds.
    #[must_use]
    pub const fn actual(&self) -> Kind {
        self.actual
    }

    /// Take the message back.
    #[must_use]
    pub fn into_message(self) -> Message<Any> {
        self.message
    }
}

impl fmt::Display for WrongKind {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "expected a {} message, found {}",
            self.expected, self.actual
        )
    }
}

impl fmt::Debug for WrongKind {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("WrongKind")
            .field("expected", &self.expected)
            .field("actual", &self.actual)
            .finish_non_exhaustive()
    }
}

impl std::error::Error for WrongKind {}

impl TryFrom<Message<Any>> for GribMessage {
    type Error = WrongKind;

    fn try_from(message: Message<Any>) -> std::result::Result<Self, WrongKind> {
        message.try_into_grib()
    }
}

impl TryFrom<Message<Any>> for BufrMessage {
    type Error = WrongKind;

    fn try_from(message: Message<Any>) -> std::result::Result<Self, WrongKind> {
        message.try_into_bufr()
    }
}

impl From<GribMessage> for Message<Any> {
    fn from(message: GribMessage) -> Self {
        message.into_any()
    }
}

impl From<BufrMessage> for Message<Any> {
    fn from(message: BufrMessage) -> Self {
        message.into_any()
    }
}
