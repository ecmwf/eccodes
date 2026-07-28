//! `Index` — indexed access to the messages in a set of files.

use std::ffi::{CStr, CString, c_char, c_long};
use std::path::Path;
use std::ptr::{self, NonNull};

use eccodes_sys as sys;

use crate::error::{Error, Result, check};
use crate::handle::{Handle, ckey, cpath};
use crate::kind::Any;

/// Index over the messages in one or more files, keyed by a fixed set of
/// keys (`codes_index`).
///
/// Select values with the `select_*` methods, then drain the matching
/// messages with [`Index::next_handle`].
pub struct Index {
    raw: NonNull<sys::codes_index>,
}

// SAFETY: the index owns its C object exclusively; not Sync — the C index
// carries internal iteration state.
unsafe impl Send for Index {}

impl Index {
    /// Create an empty index on `keys` (`codes_index_new`).
    pub fn new(keys: &[&str]) -> Result<Self> {
        let keys = ckey(&keys.join(","))?;
        let mut err = 0;
        // SAFETY: NULL context selects the default context; `keys` is a valid
        // NUL-terminated comma-separated list.
        let raw = unsafe { sys::codes_index_new(ptr::null_mut(), keys.as_ptr(), &raw mut err) };
        Error::from_code(err)?;
        NonNull::new(raw)
            .map(|raw| Self { raw })
            .ok_or(Error::NullIndex)
    }

    /// Index the messages in the file at `path` on `keys`
    /// (`codes_index_new_from_file`).
    pub fn from_file(path: impl AsRef<Path>, keys: &[&str]) -> Result<Self> {
        let path = cpath(path.as_ref())?;
        let keys = ckey(&keys.join(","))?;
        let mut err = 0;
        // SAFETY: as for `new`, plus a valid NUL-terminated path.
        let raw = unsafe {
            sys::codes_index_new_from_file(
                ptr::null_mut(),
                path.as_ptr(),
                keys.as_ptr(),
                &raw mut err,
            )
        };
        Error::from_code(err)?;
        NonNull::new(raw)
            .map(|raw| Self { raw })
            .ok_or(Error::NullIndex)
    }

    /// Load an index previously saved with [`Index::write`]
    /// (`codes_index_read`).
    pub fn read(path: impl AsRef<Path>) -> Result<Self> {
        let path = cpath(path.as_ref())?;
        let mut err = 0;
        // SAFETY: NULL context selects the default context; valid path.
        let raw = unsafe { sys::codes_index_read(ptr::null_mut(), path.as_ptr(), &raw mut err) };
        Error::from_code(err)?;
        NonNull::new(raw)
            .map(|raw| Self { raw })
            .ok_or(Error::NullIndex)
    }

    /// Save this index to the file at `path` (`codes_index_write`).
    pub fn write(&self, path: impl AsRef<Path>) -> Result<()> {
        let path = cpath(path.as_ref())?;
        check!(sys::codes_index_write(self.raw.as_ptr(), path.as_ptr()))
    }

    /// Index the messages of another file into this index
    /// (`codes_index_add_file`).
    pub fn add_file(&mut self, path: impl AsRef<Path>) -> Result<()> {
        let path = cpath(path.as_ref())?;
        check!(sys::codes_index_add_file(self.raw.as_ptr(), path.as_ptr()))
    }

    /// Number of distinct values of `key` in the index
    /// (`codes_index_get_size`).
    pub fn size(&self, key: &str) -> Result<usize> {
        let key = ckey(key)?;
        let mut size: usize = 0;
        check!(sys::codes_index_get_size(
            self.raw.as_ptr(),
            key.as_ptr(),
            &raw mut size
        ))?;
        Ok(size)
    }

    /// The distinct `long` values of `key` (`codes_index_get_long`).
    pub fn values_long(&self, key: &str) -> Result<Vec<i64>> {
        let ckey_ = ckey(key)?;
        let mut len = self.size(key)?;
        let mut values = vec![0_i64; len];
        check!(sys::codes_index_get_long(
            self.raw.as_ptr(),
            ckey_.as_ptr(),
            values.as_mut_ptr().cast::<c_long>(),
            &raw mut len,
        ))?;
        values.truncate(len);
        Ok(values)
    }

    /// The distinct `double` values of `key` (`codes_index_get_double`).
    pub fn values_double(&self, key: &str) -> Result<Vec<f64>> {
        let ckey_ = ckey(key)?;
        let mut len = self.size(key)?;
        let mut values = vec![0.0_f64; len];
        check!(sys::codes_index_get_double(
            self.raw.as_ptr(),
            ckey_.as_ptr(),
            values.as_mut_ptr(),
            &raw mut len,
        ))?;
        values.truncate(len);
        Ok(values)
    }

    /// The distinct string values of `key` (`codes_index_get_string`).
    pub fn values_string(&self, key: &str) -> Result<Vec<String>> {
        let ckey_ = ckey(key)?;
        let mut len = self.size(key)?;
        let mut ptrs: Vec<*mut c_char> = vec![ptr::null_mut(); len];
        check!(sys::codes_index_get_string(
            self.raw.as_ptr(),
            ckey_.as_ptr(),
            ptrs.as_mut_ptr(),
            &raw mut len,
        ))?;
        ptrs.truncate(len);
        let mut values = Vec::with_capacity(len);
        let mut utf8_err = None;
        for ptr in ptrs {
            if ptr.is_null() {
                continue;
            }
            // SAFETY: non-null NUL-terminated string allocated by the library.
            match unsafe { CStr::from_ptr(ptr) }.to_str() {
                Ok(s) => values.push(s.to_owned()),
                Err(e) => utf8_err = Some(e),
            }
            // SAFETY: allocated with malloc by the library, ownership is ours.
            unsafe { libc::free(ptr.cast()) };
        }
        utf8_err.map_or(Ok(values), |e| Err(e.into()))
    }

    /// Restrict the selection to messages with `key == value`
    /// (`codes_index_select_long`).
    pub fn select_long(&mut self, key: &str, value: i64) -> Result<()> {
        let key = ckey(key)?;
        check!(sys::codes_index_select_long(
            self.raw.as_ptr(),
            key.as_ptr(),
            value
        ))
    }

    /// Restrict the selection to messages with `key == value`
    /// (`codes_index_select_double`).
    pub fn select_double(&mut self, key: &str, value: f64) -> Result<()> {
        let key = ckey(key)?;
        check!(sys::codes_index_select_double(
            self.raw.as_ptr(),
            key.as_ptr(),
            value
        ))
    }

    /// Restrict the selection to messages with `key == value`
    /// (`codes_index_select_string`).
    pub fn select_string(&mut self, key: &str, value: &str) -> Result<()> {
        let key = ckey(key)?;
        let value = CString::new(value)?;
        check!(sys::codes_index_select_string(
            self.raw.as_ptr(),
            key.as_ptr(),
            value.as_ptr()
        ))
    }

    /// The next message matching the current selection
    /// (`codes_handle_new_from_index`). `Ok(None)` when the selection is
    /// exhausted.
    ///
    /// All keys of the index must have a selected value before calling this.
    pub fn next_handle(&mut self) -> Result<Option<Handle<Any>>> {
        let mut err = 0;
        // SAFETY: valid index.
        let raw = unsafe { sys::codes_handle_new_from_index(self.raw.as_ptr(), &raw mut err) };
        let Some(raw) = NonNull::new(raw) else {
            return match Error::from_code(err) {
                Ok(()) | Err(Error::EndOfIndex) => Ok(None),
                Err(e) => Err(e),
            };
        };
        Ok(Some(Handle::from_raw(raw)))
    }
}

impl Drop for Index {
    fn drop(&mut self) {
        // SAFETY: `raw` is a valid index owned by us; freed exactly once.
        unsafe { sys::codes_index_delete(self.raw.as_ptr()) };
    }
}
