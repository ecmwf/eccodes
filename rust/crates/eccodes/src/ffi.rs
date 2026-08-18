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
use std::fs::File;
use std::io::Write;
use std::os::fd::IntoRawFd;
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

    /// Take over an open [`File`] (`fdopen`).
    ///
    /// The `File` is consumed: the stream owns the descriptor from here on,
    /// so no second handle shares its cursor.
    pub(crate) fn from_file(file: File) -> Result<Self> {
        let fd = file.into_raw_fd();
        // SAFETY: `fd` is open and owned by us; on success its ownership
        // moves into the returned stream.
        let raw = unsafe { libc::fdopen(fd, c"rb".as_ptr()) };
        NonNull::new(raw.cast::<sys::FILE>())
            .map(|raw| Self { raw })
            .ok_or_else(|| {
                let err = Error::last_os_error();
                // SAFETY: fdopen failed, so the bare descriptor is still ours
                // to close.
                unsafe { libc::close(fd) };
                err
            })
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
