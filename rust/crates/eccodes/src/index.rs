//! [`Index`] — selecting messages by key value, across one or more files.
//!
//! ```no_run
//! use eccodes::Index;
//!
//! # fn main() -> eccodes::Result<()> {
//! let mut index = Index::from_file("data.grib2", ["shortName", "step"])?;
//!
//! println!("{:?}", index.values::<String>("shortName")?);
//!
//! index.select("shortName", "2t")?.select("step", 0_i64)?;
//! for message in index.messages() {
//!     println!("{}", message?.get::<f64>("average")?);
//! }
//! # Ok(())
//! # }
//! ```
//!
//! Every key of the index must have a selected value before the messages can
//! be read — the index is a lookup, not a filter.

use std::ffi::c_char;
use std::fmt;
use std::iter::FusedIterator;
use std::path::Path;
use std::ptr::{self, NonNull};

use eccodes_sys as sys;

use crate::error::{Code, Error, ErrorContext, Result, check};
use crate::ffi;
use crate::message::Message;
use crate::multi;

/// An index of the messages in one or more files, keyed on a fixed set of
/// keys.
pub struct Index {
    raw: NonNull<sys::codes_index>,
}

// SAFETY: an index owns its C object exclusively and may move between
// threads. Not `Sync`: selection and iteration mutate state inside it.
unsafe impl Send for Index {}

impl Index {
    /// An empty index over `keys`, to be filled with
    /// [`add_file`](Self::add_file).
    pub fn new<I, S>(keys: I) -> Result<Self>
    where
        I: IntoIterator<Item = S>,
        S: AsRef<str>,
    {
        let ckeys = ffi::cstring(&join_keys(keys))?;
        let mut status = 0;
        // SAFETY: a NULL context selects the default one; `ckeys` is a
        // NUL-terminated comma-separated list, copied by the library.
        let raw = unsafe { sys::codes_index_new(ptr::null_mut(), ckeys.as_ptr(), &raw mut status) };
        Error::from_raw(status)?;
        NonNull::new(raw)
            .map(|raw| Self { raw })
            .ok_or_else(|| Error::from(Code::NullIndex))
    }

    /// An index over `keys`, built from the messages in `path`.
    pub fn from_file<I, S>(path: impl AsRef<Path>, keys: I) -> Result<Self>
    where
        I: IntoIterator<Item = S>,
        S: AsRef<str>,
    {
        let path = path.as_ref();
        let cpath = ffi::cpath(path)?;
        let ckeys = ffi::cstring(&join_keys(keys))?;
        let mut status = 0;
        // SAFETY: as for `new`, plus a NUL-terminated path that is only read.
        // Indexing decodes the file's messages, so it reads the multi-field
        // switch: `reading` holds it off for the call.
        let raw = multi::reading(false, || unsafe {
            sys::codes_index_new_from_file(
                ptr::null_mut(),
                cpath.as_ptr(),
                ckeys.as_ptr(),
                &raw mut status,
            )
        });
        Error::from_raw(status).with_path(path)?;
        NonNull::new(raw)
            .map(|raw| Self { raw })
            .ok_or_else(|| Error::from(Code::NullIndex))
            .with_path(path)
    }

    /// Load an index written by [`save`](Self::save).
    pub fn open(path: impl AsRef<Path>) -> Result<Self> {
        let path = path.as_ref();
        let cpath = ffi::cpath(path)?;
        let mut status = 0;
        // SAFETY: a NULL context selects the default one; NUL-terminated path.
        let raw =
            unsafe { sys::codes_index_read(ptr::null_mut(), cpath.as_ptr(), &raw mut status) };
        Error::from_raw(status).with_path(path)?;
        NonNull::new(raw)
            .map(|raw| Self { raw })
            .ok_or_else(|| Error::from(Code::NullIndex))
            .with_path(path)
    }

    /// Write the index to `path`, to be loaded later by [`open`](Self::open).
    pub fn save(&self, path: impl AsRef<Path>) -> Result<()> {
        let path = path.as_ref();
        let cpath = ffi::cpath(path)?;
        check!(sys::codes_index_write(self.raw.as_ptr(), cpath.as_ptr())).with_path(path)
    }

    /// Add the messages of another file to the index.
    pub fn add_file(&mut self, path: impl AsRef<Path>) -> Result<()> {
        let path = path.as_ref();
        let cpath = ffi::cpath(path)?;
        // Decodes the file, so it reads the multi-field switch — as for
        // [`from_file`](Self::from_file).
        multi::reading(false, || {
            check!(sys::codes_index_add_file(self.raw.as_ptr(), cpath.as_ptr()))
        })
        .with_path(path)
    }

    /// How many distinct values of `key` the index holds.
    pub fn value_count(&self, key: &str) -> Result<usize> {
        let ckey = ffi::cstring(key)?;
        let mut count: usize = 0;
        check!(sys::codes_index_get_size(
            self.raw.as_ptr(),
            ckey.as_ptr(),
            &raw mut count
        ))
        .with_key(key)?;
        Ok(count)
    }

    /// The distinct values of `key`, decoded as `T` — `i64`, `f64` or
    /// `String`.
    pub fn values<T: IndexValue>(&self, key: &str) -> Result<Vec<T>> {
        T::values_from(self, key)
    }

    /// Restrict the selection to messages whose `key` equals `value`.
    ///
    /// Chainable: each call narrows the selection further, and every key of
    /// the index must be selected before [`messages`](Self::messages).
    pub fn select<T: IndexSelect>(&mut self, key: &str, value: T) -> Result<&mut Self> {
        value.select_on(self, key)?;
        Ok(self)
    }

    /// The messages matching the current selection.
    #[must_use]
    pub const fn messages(&mut self) -> IndexMessages<'_> {
        IndexMessages {
            index: self,
            done: false,
        }
    }

    pub(crate) const fn as_ptr(&self) -> *mut sys::codes_index {
        self.raw.as_ptr()
    }
}

impl Drop for Index {
    fn drop(&mut self) {
        // SAFETY: a valid index owned by us, freed exactly once.
        unsafe { sys::codes_index_delete(self.raw.as_ptr()) };
    }
}

impl fmt::Debug for Index {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("Index").finish_non_exhaustive()
    }
}

fn join_keys<I, S>(keys: I) -> String
where
    I: IntoIterator<Item = S>,
    S: AsRef<str>,
{
    keys.into_iter()
        .map(|key| key.as_ref().to_owned())
        .collect::<Vec<_>>()
        .join(",")
}

/// A type the distinct values of an index key can be read as — see
/// [`Index::values`].
pub trait IndexValue: Sized {
    /// Read the distinct values of `key`.
    fn values_from(index: &Index, key: &str) -> Result<Vec<Self>>;
}

/// A type an index selection can be made with — see [`Index::select`].
pub trait IndexSelect {
    /// Select messages whose `key` equals `self`.
    fn select_on(self, index: &mut Index, key: &str) -> Result<()>;
}

impl IndexValue for i64 {
    fn values_from(index: &Index, key: &str) -> Result<Vec<Self>> {
        let ckey = ffi::cstring(key)?;
        let mut len = index.value_count(key)?;
        let mut values = vec![0_i64; len];
        check!(sys::codes_index_get_long(
            index.as_ptr(),
            ckey.as_ptr(),
            values.as_mut_ptr(),
            &raw mut len,
        ))
        .with_key(key)?;
        values.truncate(len);
        Ok(values)
    }
}

impl IndexValue for f64 {
    fn values_from(index: &Index, key: &str) -> Result<Vec<Self>> {
        let ckey = ffi::cstring(key)?;
        let mut len = index.value_count(key)?;
        let mut values = vec![0.0_f64; len];
        check!(sys::codes_index_get_double(
            index.as_ptr(),
            ckey.as_ptr(),
            values.as_mut_ptr(),
            &raw mut len,
        ))
        .with_key(key)?;
        values.truncate(len);
        Ok(values)
    }
}

impl IndexValue for String {
    fn values_from(index: &Index, key: &str) -> Result<Vec<Self>> {
        let ckey = ffi::cstring(key)?;
        let mut len = index.value_count(key)?;
        let mut ptrs: Vec<*mut c_char> = vec![ptr::null_mut(); len];
        check!(sys::codes_index_get_string(
            index.as_ptr(),
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

impl IndexSelect for i64 {
    fn select_on(self, index: &mut Index, key: &str) -> Result<()> {
        let ckey = ffi::cstring(key)?;
        check!(sys::codes_index_select_long(
            index.as_ptr(),
            ckey.as_ptr(),
            self
        ))
        .with_key(key)
    }
}

impl IndexSelect for f64 {
    fn select_on(self, index: &mut Index, key: &str) -> Result<()> {
        let ckey = ffi::cstring(key)?;
        check!(sys::codes_index_select_double(
            index.as_ptr(),
            ckey.as_ptr(),
            self
        ))
        .with_key(key)
    }
}

impl IndexSelect for &str {
    fn select_on(self, index: &mut Index, key: &str) -> Result<()> {
        let ckey = ffi::cstring(key)?;
        let value = ffi::cstring(self)?;
        check!(sys::codes_index_select_string(
            index.as_ptr(),
            ckey.as_ptr(),
            value.as_ptr()
        ))
        .with_key(key)
    }
}

impl IndexSelect for &String {
    fn select_on(self, index: &mut Index, key: &str) -> Result<()> {
        self.as_str().select_on(index, key)
    }
}

impl IndexSelect for String {
    fn select_on(self, index: &mut Index, key: &str) -> Result<()> {
        self.as_str().select_on(index, key)
    }
}

/// The messages matching an index selection — see [`Index::messages`].
pub struct IndexMessages<'i> {
    index: &'i mut Index,
    done: bool,
}

impl Iterator for IndexMessages<'_> {
    type Item = Result<Message>;

    fn next(&mut self) -> Option<Self::Item> {
        if self.done {
            return None;
        }
        let mut status = 0;
        // SAFETY: valid index; the status out-pointer addresses a local.
        // Re-reads the message from its file, so it reads the multi-field
        // switch: an index yields the messages it indexed, never fields.
        let raw = multi::reading(false, || unsafe {
            sys::codes_handle_new_from_index(self.index.as_ptr(), &raw mut status)
        });
        if let Some(raw) = NonNull::new(raw) {
            return Some(Ok(Message::from_raw(raw)));
        }

        self.done = true;
        match Error::from_raw(status) {
            // Both spellings the library uses for "that was the last one".
            Ok(()) => None,
            Err(err) if err.code() == Some(Code::EndOfIndex) => None,
            Err(err) => Some(Err(err)),
        }
    }
}

impl FusedIterator for IndexMessages<'_> {}

impl fmt::Debug for IndexMessages<'_> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "IndexMessages {{ done: {} }}", self.done)
    }
}
