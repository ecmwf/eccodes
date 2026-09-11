//! C plumbing — crate-internal.
//!
//! Everything the C API needs but a Rust caller should never see lives here:
//! NUL-terminated strings, owned `FILE*` streams, library-allocated string
//! arrays and the `c_int`/`usize` conversions. No item in this module is
//! public, and no type in the public API mentions one.
//!
//! The rule for the rest of the crate: call `sys::` functions freely, but
//! build no `CString`, open no stream and cast no integer by hand — use the
//! helpers here, so each of those has exactly one audited implementation.

use std::ffi::{CStr, CString, c_char, c_int, c_long, c_void};
use std::io::{ErrorKind, Read, Write};
use std::path::Path;
use std::ptr::{self, NonNull};

use eccodes_sys as sys;

use crate::error::{Code, Error, ErrorContext, Result};

/// A NUL-terminated copy of `text`, for the C API.
pub fn cstring(text: &str) -> Result<CString> {
    Ok(CString::new(text)?)
}

/// A NUL-terminated copy of `path`, for the C API.
pub fn cpath(path: &Path) -> Result<CString> {
    CString::new(path.as_os_str().as_encoded_bytes()).map_err(Error::from)
}

/// Borrow a static string the library owns (version strings, messages,
/// paths). `None` for NULL or non-UTF-8.
///
/// # Safety
///
/// `ptr` must be NULL or point to a NUL-terminated string that lives for the
/// rest of the process — a string literal or a static buffer inside the C
/// library, never something the caller has to free.
pub unsafe fn static_str(ptr: *const c_char) -> Option<&'static str> {
    if ptr.is_null() {
        return None;
    }
    // SAFETY: non-null and NUL-terminated by the caller's contract; the
    // 'static lifetime is sound because the string outlives the process.
    unsafe { CStr::from_ptr(ptr) }.to_str().ok()
}

/// Take ownership of an array of strings the library allocated with `malloc`,
/// freeing every one of them.
///
/// Conversion failures are reported after the whole array has been freed, so
/// a bad string cannot leak the rest.
///
/// # Safety
///
/// Each non-null pointer in `ptrs` must be a NUL-terminated string allocated
/// by the C library and not freed elsewhere; ownership moves to this call.
pub unsafe fn take_strings(ptrs: &[*mut c_char]) -> Result<Vec<String>> {
    let mut values = Vec::with_capacity(ptrs.len());
    let mut failure = None;
    for &ptr in ptrs {
        if ptr.is_null() {
            continue;
        }
        // SAFETY: non-null NUL-terminated string owned by the library.
        match unsafe { CStr::from_ptr(ptr) }.to_str() {
            Ok(text) => values.push(text.to_owned()),
            Err(err) => failure = Some(err),
        }
        // SAFETY: allocated by the library with malloc; freed exactly once.
        unsafe { libc::free(ptr.cast::<c_void>()) };
    }
    failure.map_or(Ok(values), |err| Err(Error::from(err)))
}

/// Take ownership of an array of file offsets the library allocated with
/// `malloc`, freeing it.
///
/// Always call this once the C function has returned, success or not: the
/// library allocates before it can fail, and hands the array over either way.
///
/// # Safety
///
/// `offsets` must be NULL, or an array of at least `count` offsets allocated
/// by the C library and not freed elsewhere; ownership moves to this call.
pub unsafe fn take_offsets(offsets: *mut libc::off_t, count: usize) -> Result<Vec<u64>> {
    let taken = if offsets.is_null() {
        Ok(Vec::new())
    } else {
        // SAFETY: `count` offsets allocated by the library, by the caller's
        // contract.
        unsafe { std::slice::from_raw_parts(offsets, count) }
            .iter()
            .map(|&offset| u64::try_from(offset).map_err(|_| Error::from(Code::InternalError)))
            .collect()
    };
    // SAFETY: allocated by the library with malloc; freed exactly once, after
    // the last read above. `free` accepts NULL.
    unsafe { libc::free(offsets.cast::<c_void>()) };
    taken
}

/// An owned `FILE*`, closed on drop.
pub struct CFile {
    raw: NonNull<sys::FILE>,
}

impl CFile {
    /// Open `path` for reading (`fopen`).
    pub(crate) fn open(path: &Path) -> Result<Self> {
        let cpath = cpath(path)?;
        // SAFETY: NUL-terminated path and mode strings.
        let raw = unsafe { sys::fopen(cpath.as_ptr(), c"rb".as_ptr()) };
        NonNull::new(raw)
            .map(|raw| Self { raw })
            .ok_or_else(Error::last_os_error)
            .with_path(path)
    }

    pub(crate) const fn as_ptr(&self) -> *mut sys::FILE {
        self.raw.as_ptr()
    }
}

impl Drop for CFile {
    fn drop(&mut self) {
        // SAFETY: an open stream owned by us; closed exactly once.
        unsafe { sys::fclose(self.raw.as_ptr()) };
    }
}

/// A Rust reader the C library can pull messages out of.
///
/// `codes_handle_new_from_stream` reads through a callback and an opaque
/// pointer rather than a `FILE*`, which is what lets a message come from a
/// socket, a pipe or a buffer. This holds the reader that callback pulls
/// from, together with the I/O failure it hit — the C signature has nowhere
/// to put one.
///
/// The reader is called from a C frame, so a panic inside it aborts the
/// process, as any panic crossing one does.
pub struct ReadStream<'src> {
    reader: Box<dyn Read + 'src>,
    failure: Option<std::io::Error>,
}

impl<'src> ReadStream<'src> {
    pub fn new(reader: impl Read + 'src) -> Self {
        Self {
            reader: Box::new(reader),
            failure: None,
        }
    }

    /// The `stream_data` pointer to hand to the C reader, alongside
    /// [`read_stream`].
    pub const fn as_data(&mut self) -> *mut c_void {
        (&raw mut *self).cast::<c_void>()
    }

    /// The I/O failure the reader hit, taken out of the stream.
    ///
    /// The callback reports one as the end of the stream, so a read that
    /// stopped early asks here for the reason before believing it.
    pub fn take_failure(&mut self) -> Option<Error> {
        self.failure.take().map(Error::from)
    }
}

/// The `stream_proc` of a [`ReadStream`]: fill `buffer` with `len` bytes.
///
/// The library wants exactly what it asked for; `-1` says the stream ended,
/// and anything shorter is a message that stops in the middle, which it
/// reports as an I/O problem. A failure of the Rust reader is kept on the
/// stream and reported as the end, for the caller to replace with the real
/// error.
///
/// # Safety
///
/// `data` must point to a live [`ReadStream`] that outlives the call, and
/// `buffer` must be writable for `len` bytes — what the C reader passes back
/// from `codes_handle_new_from_stream`.
pub unsafe extern "C" fn read_stream(
    data: *mut c_void,
    buffer: *mut c_void,
    len: c_long,
) -> c_long {
    let Ok(wanted) = usize::try_from(len) else {
        return -1;
    };
    // SAFETY: the caller's contract — a live stream, and a buffer of `len`
    // writable bytes. The borrow ends with this call, so the lifetime the
    // pointer is read back at cannot outlive the reader it names.
    let stream = unsafe { &mut *data.cast::<ReadStream<'_>>() };
    // SAFETY: as above.
    let buf = unsafe { std::slice::from_raw_parts_mut(buffer.cast::<u8>(), wanted) };

    let mut filled = 0;
    while filled < wanted {
        match stream.reader.read(&mut buf[filled..]) {
            Ok(0) => break,
            Ok(read) => filled += read,
            Err(err) if err.kind() == ErrorKind::Interrupted => {}
            Err(err) => {
                stream.failure = Some(err);
                return -1;
            }
        }
    }
    if filled == 0 && wanted > 0 {
        return -1;
    }
    // `filled` never exceeds `len`, which came from a `c_long`.
    c_long::try_from(filled).unwrap_or(-1)
}

/// Run `write_message` with a `FILE*` backed by memory, then hand the bytes
/// it wrote to `out`.
///
/// For the C entry points that only write to a stream — `codes_dump_content`
/// and `codes_grib_multi_handle_write`.
pub fn with_memstream(
    mut out: impl Write,
    write_message: impl FnOnce(*mut sys::FILE) -> Result<()>,
) -> Result<()> {
    let mut buf: *mut c_char = ptr::null_mut();
    let mut len: libc::size_t = 0;
    // SAFETY: out-pointers to locals that outlive the stream.
    let stream = unsafe { libc::open_memstream(&raw mut buf, &raw mut len) };
    let Some(stream) = NonNull::new(stream) else {
        return Err(Error::last_os_error());
    };

    let written = write_message(stream.as_ptr().cast::<sys::FILE>());
    // SAFETY: `stream` is open; fclose flushes and finalizes `buf`/`len`.
    unsafe { libc::fclose(stream.as_ptr()) };

    let result = written.and_then(|()| {
        if buf.is_null() {
            return Err(Error::last_os_error());
        }
        // SAFETY: after fclose, `buf` points to `len` initialized bytes.
        let bytes = unsafe { std::slice::from_raw_parts(buf.cast::<u8>(), len) };
        Ok(out.write_all(bytes)?)
    });

    // SAFETY: allocated by open_memstream; freed exactly once, after the last
    // read of the buffer above.
    unsafe { libc::free(buf.cast::<c_void>()) };
    result
}

/// A count or size the C API returned, as a `usize`.
///
/// A negative count means the library contradicted its own contract, which is
/// [`Code::InternalError`], not something a caller can act on.
pub fn to_usize<T: TryInto<usize>>(value: T) -> Result<usize> {
    value
        .try_into()
        .map_err(|_| Error::from(Code::InternalError))
}

/// A caller-supplied length or index, as the C `int` the API expects.
pub fn to_c_int(value: usize) -> Result<c_int> {
    c_int::try_from(value).map_err(|_| Error::from(Code::InvalidArgument))
}

/// A caller-supplied length, as the C `long` the API expects.
pub fn to_c_long(value: usize) -> Result<c_long> {
    c_long::try_from(value).map_err(|_| Error::from(Code::InvalidArgument))
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn interior_nul_is_rejected_rather_than_truncated() {
        // C would silently see "short" where the caller meant "short\0Name";
        // the error keeps that from reaching the library as a valid lookup.
        let err = cstring("short\0Name").expect_err("interior NUL");
        assert_eq!(err.code(), None);
        assert!(err.to_string().starts_with("eccodes::Nul: "));
    }

    #[test]
    fn missing_file_reports_the_path_it_tried() {
        let err = CFile::open(Path::new("/nonexistent/eccodes-test.grib2"))
            .err()
            .expect("open of a missing file fails");
        assert_eq!(
            err.path(),
            Some(Path::new("/nonexistent/eccodes-test.grib2"))
        );
        assert!(err.io_error().is_some());
    }

    #[test]
    fn memstream_hands_back_what_was_written() {
        let mut out = Vec::new();
        with_memstream(&mut out, |stream| {
            // SAFETY: `stream` is the open memory stream owned by the caller.
            unsafe { libc::fputs(c"hello".as_ptr(), stream.cast()) };
            Ok(())
        })
        .expect("memstream write");
        assert_eq!(out, b"hello");
    }

    #[test]
    fn memstream_frees_its_buffer_when_the_body_fails() {
        let mut out = Vec::new();
        let err = with_memstream(&mut out, |_| Err(Error::from(Code::ReadOnly)))
            .expect_err("body failed");
        assert_eq!(err.code(), Some(Code::ReadOnly));
        assert!(out.is_empty(), "nothing is written when the body fails");
    }

    #[test]
    fn conversions_refuse_out_of_range_values() {
        assert_eq!(to_usize(7_i32).expect("in range"), 7);
        assert_eq!(
            to_usize(-1_i32).expect_err("negative count").code(),
            Some(Code::InternalError)
        );
        assert_eq!(
            to_c_int(usize::MAX).expect_err("too large").code(),
            Some(Code::InvalidArgument)
        );
    }
}
