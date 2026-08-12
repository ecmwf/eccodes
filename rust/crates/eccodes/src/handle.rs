//! `Handle` — a GRIB/BUFR message, and `MessageReader` — iteration over
//! messages in a file.

use std::ffi::{CString, c_char, c_int, c_void};
use std::fs::File;
use std::io::Write;
use std::marker::PhantomData;
use std::mem::ManuallyDrop;
use std::os::fd::AsRawFd;
use std::path::Path;
use std::ptr::{self, NonNull};

use eccodes_sys as sys;

use crate::error::{Error, Result, check};
use crate::key::{KeyGet, KeySet};
use crate::keys_iter::{KeyFlags, KeysIterator};
use crate::kind::{Any, Grib, HandleKind, SingleKind};
use crate::{Kind, NativeType};

/// Convert a key name to a C string.
pub fn ckey(key: &str) -> Result<CString> {
    Ok(CString::new(key)?)
}

/// Convert a path to a C string.
pub fn cpath(path: &Path) -> Result<CString> {
    Ok(CString::new(path.as_os_str().as_encoded_bytes())?)
}

// =============================================================================
// Handle
// =============================================================================

/// Handle to a single GRIB/BUFR message (or a multi-field GRIB message under
/// construction, for `K = GribMulti`).
///
/// The type parameter encodes the product kind at compile time — see
/// [`crate::kind`]. The default `Handle` (= `Handle<Any>`) covers any single
/// message; [`Handle<Grib>`] unlocks the GRIB-only API.
pub struct Handle<K: HandleKind = Any> {
    raw: NonNull<K::CType>,
    _kind: PhantomData<K>,
}

// SAFETY: the handle owns its C object exclusively; eccodes handles can be
// moved between threads. NOT Sync: they carry internal mutable state
// (accessor caches) even on const calls — concurrent access is a data race.
unsafe impl<K: HandleKind> Send for Handle<K> {}

impl<K: HandleKind> Drop for Handle<K> {
    fn drop(&mut self) {
        // SAFETY: `raw` is valid and owned by us; freed exactly once here.
        unsafe { K::delete(self.raw) };
    }
}

impl<K: HandleKind> Handle<K> {
    pub(crate) const fn from_raw(raw: NonNull<K::CType>) -> Self {
        Self {
            raw,
            _kind: PhantomData,
        }
    }

    /// Access the underlying `codes_handle`/`codes_multi_handle` pointer for
    /// FFI interop. The pointer stays owned by this handle.
    #[must_use]
    pub const fn as_sys(&self) -> *mut K::CType {
        self.raw.as_ptr()
    }
}

// =============================================================================
// Single-message API (any kind)
// =============================================================================

impl<K: SingleKind> Handle<K> {
    /// Clone the underlying handle (`codes_handle_clone`).
    pub fn try_clone(&self) -> Result<Self> {
        // SAFETY: `self.raw` is a valid handle.
        let raw = unsafe { sys::codes_handle_clone(self.as_sys()) };
        NonNull::new(raw)
            .map(Self::from_raw)
            .ok_or(Error::NullHandle)
    }

    /// Clone the message headers only, with the data section replaced by
    /// dummy values (`codes_handle_clone_headers_only`).
    pub fn try_clone_headers_only(&self) -> Result<Self> {
        // SAFETY: `self.raw` is a valid handle.
        let raw = unsafe { sys::codes_handle_clone_headers_only(self.as_sys()) };
        NonNull::new(raw)
            .map(Self::from_raw)
            .ok_or(Error::NullHandle)
    }

    /// Raw message data (no copy — lifetime bound to this handle).
    pub fn message_data(&self) -> Result<&[u8]> {
        let mut data: *const c_void = ptr::null();
        let mut len: usize = 0;
        check!(sys::codes_get_message(
            self.as_sys(),
            &raw mut data,
            &raw mut len
        ))?;
        if data.is_null() {
            return Err(Error::NullPointer);
        }
        // SAFETY: the library returned a valid pointer to `len` bytes of
        // message data owned by the handle; the `&self` borrow keeps the
        // handle (and thus the buffer) alive for the returned lifetime.
        Ok(unsafe { std::slice::from_raw_parts(data.cast::<u8>(), len) })
    }

    /// Copy of the raw message data (`codes_get_message_copy`).
    pub fn message_copy(&self) -> Result<Vec<u8>> {
        let mut len = self.message_size()?;
        let mut buf = vec![0_u8; len];
        check!(sys::codes_get_message_copy(
            self.as_sys(),
            buf.as_mut_ptr().cast::<c_void>(),
            &raw mut len,
        ))?;
        buf.truncate(len);
        Ok(buf)
    }

    /// Size of the raw message in bytes (`codes_get_message_size`).
    pub fn message_size(&self) -> Result<usize> {
        let mut size: usize = 0;
        check!(sys::codes_get_message_size(self.as_sys(), &raw mut size))?;
        Ok(size)
    }

    /// Byte offset of this message within its source file
    /// (`codes_get_message_offset`).
    pub fn message_offset(&self) -> Result<u64> {
        let mut offset: libc::off_t = 0;
        check!(sys::codes_get_message_offset(
            self.as_sys(),
            &raw mut offset
        ))?;
        u64::try_from(offset).map_err(|_| Error::InternalError)
    }

    /// Write the raw message to `w`.
    pub fn write_to(&self, w: &mut impl Write) -> Result<()> {
        Ok(w.write_all(self.message_data()?)?)
    }

    /// The product kind this handle was created as
    /// (`codes_get_product_kind`).
    ///
    /// The C library stores the reader's product kind rather than
    /// inspecting the message, so handles from
    /// [`MessageReader::any`] report [`Kind::Any`]. To detect the
    /// content kind, read the `kindOfProduct` string key.
    pub fn product_kind(&self) -> Result<Kind> {
        let mut product: sys::ProductKind = sys::ProductKind_PRODUCT_ANY;
        check!(sys::codes_get_product_kind(self.as_sys(), &raw mut product))?;
        Kind::from_sys(product).ok_or(Error::InternalError)
    }

    /// Check if a key is defined (`codes_is_defined`).
    pub fn is_defined(&self, key: &str) -> Result<bool> {
        let key = ckey(key)?;
        // SAFETY: valid handle and NUL-terminated key.
        Ok(unsafe { sys::codes_is_defined(self.as_sys(), key.as_ptr()) } != 0)
    }

    /// Check if a key is set to its missing value (`codes_is_missing`).
    pub fn is_missing(&self, key: &str) -> Result<bool> {
        let key = ckey(key)?;
        let mut err: c_int = 0;
        // SAFETY: valid handle and NUL-terminated key.
        let missing = unsafe { sys::codes_is_missing(self.as_sys(), key.as_ptr(), &raw mut err) };
        Error::from_code(err)?;
        Ok(missing != 0)
    }

    /// Check if a key is computed (not coded in the message)
    /// (`codes_key_is_computed`).
    pub fn is_computed(&self, key: &str) -> Result<bool> {
        let key = ckey(key)?;
        let mut err: c_int = 0;
        // SAFETY: valid handle and NUL-terminated key.
        let computed =
            unsafe { sys::codes_key_is_computed(self.as_sys(), key.as_ptr(), &raw mut err) };
        Error::from_code(err)?;
        Ok(computed != 0)
    }

    /// Number of elements for a key (1 for scalars, N for arrays)
    /// (`codes_get_size`).
    pub fn size(&self, key: &str) -> Result<usize> {
        let key = ckey(key)?;
        let mut size: usize = 0;
        check!(sys::codes_get_size(
            self.as_sys(),
            key.as_ptr(),
            &raw mut size
        ))?;
        Ok(size)
    }

    /// Length of the string representation of a key (`codes_get_length`).
    pub fn length(&self, key: &str) -> Result<usize> {
        let key = ckey(key)?;
        let mut len: usize = 0;
        check!(sys::codes_get_length(
            self.as_sys(),
            key.as_ptr(),
            &raw mut len
        ))?;
        Ok(len)
    }

    /// Byte offset of a key within the message (`codes_get_offset`).
    pub fn offset(&self, key: &str) -> Result<usize> {
        let key = ckey(key)?;
        let mut offset: usize = 0;
        check!(sys::codes_get_offset(
            self.as_sys(),
            key.as_ptr(),
            &raw mut offset
        ))?;
        Ok(offset)
    }

    /// The native type of a key (`codes_get_native_type`).
    pub fn native_type(&self, key: &str) -> Result<NativeType> {
        let key = ckey(key)?;
        let mut ty: c_int = 0;
        check!(sys::codes_get_native_type(
            self.as_sys(),
            key.as_ptr(),
            &raw mut ty
        ))?;
        NativeType::from_code(ty).ok_or(Error::InvalidType)
    }

    /// Get a typed value.
    pub fn get<T: KeyGet>(&self, key: &str) -> Result<T> {
        T::get_from(self, key)
    }

    /// Set a typed value.
    pub fn set<T: KeySet>(&mut self, key: &str, value: T) -> Result<()> {
        value.set_on(self, key)
    }

    /// Set a key to its missing value (`codes_set_missing`).
    pub fn set_missing(&mut self, key: &str) -> Result<()> {
        let key = ckey(key)?;
        check!(sys::codes_set_missing(self.as_sys(), key.as_ptr()))
    }

    /// Iterate over key names, filtered by `flags`
    /// (`codes_keys_iterator_new`).
    pub fn keys(&self, flags: KeyFlags) -> Result<KeysIterator<'_>> {
        KeysIterator::new(self.as_sys(), flags, None)
    }

    /// Iterate over key names in a namespace (e.g. `"mars"`, `"ls"`),
    /// filtered by `flags`.
    pub fn keys_in_namespace(&self, namespace: &str, flags: KeyFlags) -> Result<KeysIterator<'_>> {
        KeysIterator::new(self.as_sys(), flags, Some(namespace))
    }

    /// Copy all keys of a namespace from `src` into this message
    /// (`codes_copy_namespace`).
    pub fn copy_namespace(&mut self, src: &Handle<impl SingleKind>, namespace: &str) -> Result<()> {
        let namespace = ckey(namespace)?;
        check!(sys::codes_copy_namespace(
            self.as_sys(),
            namespace.as_ptr(),
            src.as_sys(),
        ))
    }

    /// Dump the message content in `wmo` format to `w`
    /// (`codes_dump_content`).
    pub fn dump(&self, w: &mut impl Write) -> Result<()> {
        static MODE: &std::ffi::CStr = c"wmo";
        with_memstream(w, |file| {
            // SAFETY: valid handle, open FILE*, NUL-terminated mode string.
            unsafe {
                sys::codes_dump_content(self.as_sys(), file, MODE.as_ptr(), 0, ptr::null_mut());
            };
            Ok(())
        })
    }
}

// =============================================================================
// Constructors + BUFR (Handle<Any>)
// =============================================================================

impl Handle<Any> {
    /// Create from raw message bytes (copies the data)
    /// (`codes_handle_new_from_message_copy`).
    pub fn from_message(data: &[u8]) -> Result<Self> {
        // SAFETY: NULL context selects the default context; `data` is valid
        // for `data.len()` bytes and copied by the library.
        let raw = unsafe {
            sys::codes_handle_new_from_message_copy(
                ptr::null_mut(),
                data.as_ptr().cast::<c_void>(),
                data.len(),
            )
        };
        NonNull::new(raw)
            .map(Self::from_raw)
            .ok_or(Error::InvalidMessage)
    }

    /// Create from an eccodes sample (e.g. `"GRIB2"`)
    /// (`codes_handle_new_from_samples`).
    pub fn from_samples(name: impl AsRef<str>) -> Result<Self> {
        let name = ckey(name.as_ref())?;
        // SAFETY: NULL context selects the default context.
        let raw = unsafe { sys::codes_handle_new_from_samples(ptr::null_mut(), name.as_ptr()) };
        NonNull::new(raw)
            .map(Self::from_raw)
            .ok_or(Error::FileNotFound)
    }

    /// Create from a BUFR sample (`codes_bufr_handle_new_from_samples`).
    pub fn bufr_from_samples(name: impl AsRef<str>) -> Result<Self> {
        let name = ckey(name.as_ref())?;
        // SAFETY: NULL context selects the default context.
        let raw =
            unsafe { sys::codes_bufr_handle_new_from_samples(ptr::null_mut(), name.as_ptr()) };
        NonNull::new(raw)
            .map(Self::from_raw)
            .ok_or(Error::FileNotFound)
    }

    /// Read the next BUFR message from an open file
    /// (`codes_bufr_handle_new_from_file`). `Ok(None)` at end of file.
    pub fn bufr_from_file(file: &mut File) -> Result<Option<Self>> {
        let file = CFile::from_file(file)?;
        let mut err: c_int = 0;
        // SAFETY: NULL context selects the default context; `file` is open.
        let raw = unsafe {
            sys::codes_bufr_handle_new_from_file(ptr::null_mut(), file.as_ptr(), &raw mut err)
        };
        let Some(raw) = NonNull::new(raw) else {
            return Error::from_code(err).map(|()| None);
        };
        Ok(Some(Self::from_raw(raw)))
    }

    /// Copy the BUFR data section values from `src` into this message
    /// (`codes_bufr_copy_data`). Errors with [`Error::InvalidArgument`] if
    /// either message is not BUFR.
    pub fn bufr_copy_data(&mut self, src: &Self) -> Result<()> {
        if self.product_kind()? != Kind::Bufr || src.product_kind()? != Kind::Bufr {
            return Err(Error::InvalidArgument);
        }
        check!(sys::codes_bufr_copy_data(src.as_sys(), self.as_sys()))
    }

    /// Retag as a GRIB handle after a runtime product-kind check. On any
    /// other kind the original handle is returned unchanged.
    pub fn try_into_grib(self) -> std::result::Result<Handle<Grib>, Self> {
        match self.product_kind() {
            Ok(Kind::Grib) => {
                let this = ManuallyDrop::new(self);
                Ok(Handle::from_raw(this.raw))
            }
            _ => Err(self),
        }
    }
}

// =============================================================================
// GRIB-only API (Handle<Grib>)
// =============================================================================

impl Handle<Grib> {
    /// Create from a GRIB sample (`codes_grib_handle_new_from_samples`).
    pub fn from_samples(name: impl AsRef<str>) -> Result<Self> {
        let name = ckey(name.as_ref())?;
        // SAFETY: NULL context selects the default context.
        let raw =
            unsafe { sys::codes_grib_handle_new_from_samples(ptr::null_mut(), name.as_ptr()) };
        NonNull::new(raw)
            .map(Self::from_raw)
            .ok_or(Error::FileNotFound)
    }

    /// Read the next GRIB message from an open file
    /// (`codes_grib_handle_new_from_file`). `Ok(None)` at end of file.
    pub fn from_file(file: &mut File) -> Result<Option<Self>> {
        let file = CFile::from_file(file)?;
        let mut err: c_int = 0;
        // SAFETY: NULL context selects the default context; `file` is open.
        let raw = unsafe {
            sys::codes_grib_handle_new_from_file(ptr::null_mut(), file.as_ptr(), &raw mut err)
        };
        let Some(raw) = NonNull::new(raw) else {
            return Error::from_code(err).map(|()| None);
        };
        Ok(Some(Self::from_raw(raw)))
    }

    /// Iterate over the GRIB messages in a byte buffer, handling both
    /// single-field and multi-field encodings
    /// (`codes_grib_handle_new_from_multi_message`).
    ///
    /// Each yielded handle owns a copy of its message, so it may outlive
    /// `buf`.
    #[must_use]
    pub const fn read_from_bytes(buf: &[u8]) -> GribMessagesInBytes<'_> {
        GribMessagesInBytes { buf, done: false }
    }

    /// The first GRIB message in a byte buffer.
    pub fn from_bytes(buf: &[u8]) -> Result<Self> {
        Self::read_from_bytes(buf)
            .next()
            .unwrap_or(Err(Error::InvalidMessage))
    }

    /// Widen back to `Handle<Any>` — same C struct, no runtime cost.
    #[must_use]
    pub fn into_any(self) -> Handle<Any> {
        let this = ManuallyDrop::new(self);
        Handle::from_raw(this.raw)
    }

    /// Latitude/longitude/value triples for all data points, as three
    /// parallel vectors (`codes_grib_get_data`).
    pub fn grib_get_data(&self) -> Result<(Vec<f64>, Vec<f64>, Vec<f64>)> {
        let n = self.size("values")?;
        let mut lats = vec![0.0_f64; n];
        let mut lons = vec![0.0_f64; n];
        let mut values = vec![0.0_f64; n];
        check!(sys::codes_grib_get_data(
            self.as_sys(),
            lats.as_mut_ptr(),
            lons.as_mut_ptr(),
            values.as_mut_ptr(),
        ))?;
        Ok((lats, lons, values))
    }
}

/// Iterator over GRIB messages in a byte buffer — see
/// [`Handle::read_from_bytes`].
pub struct GribMessagesInBytes<'a> {
    buf: &'a [u8],
    done: bool,
}

impl Iterator for GribMessagesInBytes<'_> {
    type Item = Result<Handle<Grib>>;

    fn next(&mut self) -> Option<Self::Item> {
        if self.done || self.buf.is_empty() {
            return None;
        }
        let mut data = self.buf.as_ptr().cast::<c_void>().cast_mut();
        let mut len = self.buf.len();
        let mut err: c_int = 0;
        // SAFETY: NULL context selects the default context; `data`/`len`
        // describe the unread part of the buffer and are advanced by the
        // library past the message it consumed.
        let raw = unsafe {
            sys::codes_grib_handle_new_from_multi_message(
                ptr::null_mut(),
                &raw mut data,
                &raw mut len,
                &raw mut err,
            )
        };
        self.buf = &self.buf[self.buf.len() - len..];
        let Some(raw) = NonNull::new(raw) else {
            self.done = true;
            return Error::from_code(err).err().map(Err);
        };
        // The handle references the caller's buffer; clone it so the yielded
        // handle owns its message and may outlive `buf`.
        let borrowed: Handle<Grib> = Handle::from_raw(raw);
        Some(borrowed.try_clone())
    }
}

// =============================================================================
// CFile — owned FILE* for the codes_*_from_file entry points
// =============================================================================

pub struct CFile {
    raw: NonNull<sys::FILE>,
}

impl CFile {
    /// Open `path` for reading (`fopen`).
    pub(crate) fn open(path: &Path) -> Result<Self> {
        let path = cpath(path)?;
        // SAFETY: NUL-terminated path and mode strings.
        let raw = unsafe { sys::fopen(path.as_ptr(), c"rb".as_ptr()) };
        NonNull::new(raw)
            .map(|raw| Self { raw })
            .ok_or_else(|| Error::Io(std::io::Error::last_os_error()))
    }

    /// Duplicate an open `File` into an owned `FILE*` (`dup` + `fdopen`).
    /// The duplicated descriptor shares the file offset with `file`, so
    /// reads through the returned stream advance `file`'s position.
    pub(crate) fn from_file(file: &File) -> Result<Self> {
        // SAFETY: `file` holds a valid open descriptor.
        let fd = unsafe { libc::dup(file.as_raw_fd()) };
        if fd < 0 {
            return Err(Error::Io(std::io::Error::last_os_error()));
        }
        // SAFETY: `fd` is a valid descriptor owned by us; on success its
        // ownership moves into the returned stream.
        let raw = unsafe { libc::fdopen(fd, c"rb".as_ptr()) };
        NonNull::new(raw.cast::<sys::FILE>())
            .map(|raw| Self { raw })
            .ok_or_else(|| {
                let err = std::io::Error::last_os_error();
                // SAFETY: fdopen failed, so we still own the bare descriptor.
                unsafe { libc::close(fd) };
                Error::Io(err)
            })
    }

    pub(crate) const fn as_ptr(&self) -> *mut sys::FILE {
        self.raw.as_ptr()
    }
}

impl Drop for CFile {
    fn drop(&mut self) {
        // SAFETY: `raw` is an open stream owned by us; closed exactly once.
        unsafe { sys::fclose(self.raw.as_ptr()) };
    }
}

/// Run `f` with a `FILE*` writing into memory, then copy the result to `w`.
/// Used for the C entry points that only write to `FILE*`
/// (`codes_dump_content`, `codes_grib_multi_handle_write`).
pub fn with_memstream(
    w: &mut impl Write,
    f: impl FnOnce(*mut sys::FILE) -> Result<()>,
) -> Result<()> {
    let mut buf: *mut c_char = ptr::null_mut();
    let mut size: libc::size_t = 0;
    // SAFETY: out-pointers to locals that outlive the stream.
    let file = unsafe { libc::open_memstream(&raw mut buf, &raw mut size) };
    let Some(file) = NonNull::new(file) else {
        return Err(Error::Io(std::io::Error::last_os_error()));
    };
    let result = f(file.as_ptr().cast::<sys::FILE>());
    // SAFETY: `file` is open; fclose flushes and finalizes `buf`/`size`.
    unsafe { libc::fclose(file.as_ptr()) };
    let result = result.and_then(|()| {
        if buf.is_null() {
            return Err(Error::Io(std::io::Error::last_os_error()));
        }
        // SAFETY: after fclose, `buf` points to `size` valid bytes.
        Ok(w.write_all(unsafe { std::slice::from_raw_parts(buf.cast::<u8>(), size) })?)
    });
    // SAFETY: `buf` was allocated by open_memstream and is freed exactly once.
    unsafe { libc::free(buf.cast::<c_void>()) };
    result
}

// =============================================================================
// MessageReader
// =============================================================================

/// Iterator over the messages in a file, yielding one [`Handle`] per message.
///
/// The product kind is picked by the constructor — [`MessageReader::grib`],
/// [`MessageReader::bufr`], [`MessageReader::any`] — and encoded in the
/// handle type where it is known at compile time.
///
/// # Example
///
/// ```no_run
/// use eccodes::MessageReader;
///
/// # fn main() -> eccodes::Result<()> {
/// for handle in MessageReader::grib("data.grib2")? {
///     let handle = handle?;
///     let short_name: String = handle.get("shortName")?;
///     println!("{short_name}");
/// }
/// # Ok(())
/// # }
/// ```
pub struct MessageReader<'f, K: HandleKind = Any> {
    file: CFile,
    product: sys::ProductKind,
    _marker: PhantomData<(&'f mut File, K)>,
}

impl MessageReader<'static, Grib> {
    /// Read the GRIB messages in the file at `path`.
    pub fn grib(path: impl AsRef<Path>) -> Result<Self> {
        Ok(Self {
            file: CFile::open(path.as_ref())?,
            product: sys::ProductKind_PRODUCT_GRIB,
            _marker: PhantomData,
        })
    }
}

impl<'f> MessageReader<'f, Grib> {
    /// Read the GRIB messages from an open `File`, borrowing it.
    pub fn grib_from_file(file: &'f mut File) -> Result<Self> {
        Ok(Self {
            file: CFile::from_file(file)?,
            product: sys::ProductKind_PRODUCT_GRIB,
            _marker: PhantomData,
        })
    }
}

impl MessageReader<'static, Any> {
    /// Read the messages of any product kind in the file at `path`.
    pub fn any(path: impl AsRef<Path>) -> Result<Self> {
        Ok(Self {
            file: CFile::open(path.as_ref())?,
            product: sys::ProductKind_PRODUCT_ANY,
            _marker: PhantomData,
        })
    }

    /// Read the BUFR messages in the file at `path`.
    pub fn bufr(path: impl AsRef<Path>) -> Result<Self> {
        Ok(Self {
            file: CFile::open(path.as_ref())?,
            product: sys::ProductKind_PRODUCT_BUFR,
            _marker: PhantomData,
        })
    }
}

impl<'f> MessageReader<'f, Any> {
    /// Read the messages of any product kind from an open `File`,
    /// borrowing it.
    pub fn any_from_file(file: &'f mut File) -> Result<Self> {
        Ok(Self {
            file: CFile::from_file(file)?,
            product: sys::ProductKind_PRODUCT_ANY,
            _marker: PhantomData,
        })
    }

    /// Read the BUFR messages from an open `File`, borrowing it.
    pub fn bufr_from_file(file: &'f mut File) -> Result<Self> {
        Ok(Self {
            file: CFile::from_file(file)?,
            product: sys::ProductKind_PRODUCT_BUFR,
            _marker: PhantomData,
        })
    }
}

impl<K: SingleKind> Iterator for MessageReader<'_, K> {
    type Item = Result<Handle<K>>;

    fn next(&mut self) -> Option<Self::Item> {
        let mut err: c_int = 0;
        // SAFETY: NULL context selects the default context; `file` is open.
        let raw = unsafe {
            sys::codes_handle_new_from_file(
                ptr::null_mut(),
                self.file.as_ptr(),
                self.product,
                &raw mut err,
            )
        };
        let Some(raw) = NonNull::new(raw) else {
            // NULL + CODES_SUCCESS is end of file; anything else an error.
            return Error::from_code(err).err().map(Err);
        };
        Some(Ok(Handle::from_raw(raw)))
    }
}

// =============================================================================
// Counting
// =============================================================================

/// Number of messages (any product kind) in the file at `path`
/// (`codes_count_in_filename`).
pub fn count_in_file(path: impl AsRef<Path>) -> Result<usize> {
    let path = cpath(path.as_ref())?;
    let mut n: c_int = 0;
    check!(sys::codes_count_in_filename(
        ptr::null_mut(),
        path.as_ptr(),
        &raw mut n
    ))?;
    usize::try_from(n).map_err(|_| Error::InternalError)
}

/// Number of GRIB messages in the file at `path`.
pub fn count_grib_in_file(path: impl AsRef<Path>) -> Result<usize> {
    count_product(path.as_ref(), sys::ProductKind_PRODUCT_GRIB)
}

/// Number of BUFR messages in the file at `path`.
pub fn count_bufr_in_file(path: impl AsRef<Path>) -> Result<usize> {
    count_product(path.as_ref(), sys::ProductKind_PRODUCT_BUFR)
}

/// Count messages of one product kind via `codes_extract_offsets_malloc`.
fn count_product(path: &Path, product: sys::ProductKind) -> Result<usize> {
    let path = cpath(path)?;
    let mut offsets: *mut libc::off_t = ptr::null_mut();
    let mut n: c_int = 0;
    // SAFETY: NULL context selects the default context; out-pointers to
    // locals. On success the library allocates `offsets` for us to free.
    let code = unsafe {
        sys::codes_extract_offsets_malloc(
            ptr::null_mut(),
            path.as_ptr(),
            product,
            &raw mut offsets,
            &raw mut n,
            0,
        )
    };
    // SAFETY: `offsets` is either NULL or malloc'd by the library; we only
    // need the count.
    unsafe { libc::free(offsets.cast::<c_void>()) };
    Error::from_code(code)?;
    usize::try_from(n).map_err(|_| Error::InternalError)
}
