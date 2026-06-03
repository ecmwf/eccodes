//! Index — key-based indexed access to messages in files.
//!
//! Requires `features = ["advanced"]`.

use crate::error::{EcCodesError, Result};
use crate::message::Message;
use std::ffi::{CStr, CString};
use std::os::raw::{c_int, c_long};
use std::ptr;

/// Trait for types that can be used to query or select in an index.
pub trait IndexKey: Sized {
    fn index_get(index: &Index, key: &str) -> Result<Vec<Self>>;
    fn index_select(index: &mut Index, key: &str, value: &Self) -> Result<()>;
}

impl IndexKey for i64 {
    fn index_get(index: &Index, key: &str) -> Result<Vec<Self>> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut size: usize = 0;
        let err = unsafe {
            eccodes_sys::codes_index_get_size(index.index, c_key.as_ptr(), &mut size)
        };
        EcCodesError::check(err)?;
        let mut vals = vec![0i64 as c_long; size];
        let err = unsafe {
            eccodes_sys::codes_index_get_long(
                index.index,
                c_key.as_ptr(),
                vals.as_mut_ptr(),
                &mut size,
            )
        };
        EcCodesError::check(err)?;
        vals.truncate(size);
        Ok(vals.into_iter().map(|v| v as i64).collect())
    }

    fn index_select(index: &mut Index, key: &str, value: &Self) -> Result<()> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let err = unsafe {
            eccodes_sys::codes_index_select_long(index.index, c_key.as_ptr(), *value as c_long)
        };
        EcCodesError::check(err)
    }
}

impl IndexKey for f64 {
    fn index_get(index: &Index, key: &str) -> Result<Vec<Self>> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut size: usize = 0;
        let err = unsafe {
            eccodes_sys::codes_index_get_size(index.index, c_key.as_ptr(), &mut size)
        };
        EcCodesError::check(err)?;
        let mut vals = vec![0.0f64; size];
        let err = unsafe {
            eccodes_sys::codes_index_get_double(
                index.index,
                c_key.as_ptr(),
                vals.as_mut_ptr(),
                &mut size,
            )
        };
        EcCodesError::check(err)?;
        vals.truncate(size);
        Ok(vals)
    }

    fn index_select(index: &mut Index, key: &str, value: &Self) -> Result<()> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let err = unsafe {
            eccodes_sys::codes_index_select_double(index.index, c_key.as_ptr(), *value)
        };
        EcCodesError::check(err)
    }
}

impl IndexKey for String {
    fn index_get(index: &Index, key: &str) -> Result<Vec<Self>> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut size: usize = 0;
        let err = unsafe {
            eccodes_sys::codes_index_get_size(index.index, c_key.as_ptr(), &mut size)
        };
        EcCodesError::check(err)?;
        let mut ptrs: Vec<*mut i8> = vec![ptr::null_mut(); size];
        let err = unsafe {
            eccodes_sys::codes_index_get_string(
                index.index,
                c_key.as_ptr(),
                ptrs.as_mut_ptr(),
                &mut size,
            )
        };
        EcCodesError::check(err)?;
        let mut result = Vec::with_capacity(size);
        for i in 0..size {
            if ptrs[i].is_null() {
                result.push(String::new());
            } else {
                let s = unsafe { CStr::from_ptr(ptrs[i]) };
                result.push(s.to_string_lossy().into_owned());
                // SAFETY: strings were allocated by the C library via malloc
                unsafe { crate::libc_free(ptrs[i] as *mut std::ffi::c_void) };
            }
        }
        Ok(result)
    }

    fn index_select(index: &mut Index, key: &str, value: &Self) -> Result<()> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let c_val = CString::new(value.as_str()).map_err(|_| EcCodesError::InvalidArgument)?;
        let err = unsafe {
            eccodes_sys::codes_index_select_string(index.index, c_key.as_ptr(), c_val.as_ptr())
        };
        EcCodesError::check(err)
    }
}

/// An index for key-based access to messages in files.
pub struct Index {
    index: *mut eccodes_sys::codes_index,
}

unsafe impl Send for Index {}

impl Index {
    /// Create a new empty index for the given keys.
    pub fn new(keys: &[&str]) -> Result<Self> {
        let keys_str = keys.join(",");
        let c_keys = CString::new(keys_str).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut err: c_int = 0;
        let index = unsafe {
            eccodes_sys::codes_index_new(ptr::null_mut(), c_keys.as_ptr(), &mut err)
        };
        EcCodesError::check(err)?;
        if index.is_null() {
            return Err(EcCodesError::InternalError);
        }
        Ok(Index { index })
    }

    /// Create an index from a file with the given keys.
    pub fn from_file(path: &str, keys: &[&str]) -> Result<Self> {
        let c_path = CString::new(path).map_err(|_| EcCodesError::InvalidArgument)?;
        let keys_str = keys.join(",");
        let c_keys = CString::new(keys_str).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut err: c_int = 0;
        let index = unsafe {
            eccodes_sys::codes_index_new_from_file(
                ptr::null_mut(),
                c_path.as_ptr(),
                c_keys.as_ptr(),
                &mut err,
            )
        };
        EcCodesError::check(err)?;
        if index.is_null() {
            return Err(EcCodesError::InternalError);
        }
        Ok(Index { index })
    }

    /// Add a file to this index.
    pub fn add_file(&mut self, path: &str) -> Result<()> {
        let c_path = CString::new(path).map_err(|_| EcCodesError::InvalidArgument)?;
        let err = unsafe { eccodes_sys::codes_index_add_file(self.index, c_path.as_ptr()) };
        EcCodesError::check(err)
    }

    /// Get the number of distinct values for a key.
    pub fn get_size(&self, key: &str) -> Result<usize> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut size: usize = 0;
        let err = unsafe {
            eccodes_sys::codes_index_get_size(self.index, c_key.as_ptr(), &mut size)
        };
        EcCodesError::check(err)?;
        Ok(size)
    }

    /// Get distinct values for a key (type inferred).
    pub fn get<T: IndexKey>(&self, key: &str) -> Result<Vec<T>> {
        T::index_get(self, key)
    }

    /// Select messages where key == value.
    pub fn select<T: IndexKey>(&mut self, key: &str, value: &T) -> Result<()> {
        T::index_select(self, key, value)
    }

    /// Get the next message matching the current selection.
    pub fn next_message(&mut self) -> Result<Option<Message>> {
        let mut err: c_int = 0;
        let handle = unsafe {
            eccodes_sys::codes_handle_new_from_index(self.index, &mut err)
        };
        if handle.is_null() {
            if err == eccodes_sys::CODES_END_OF_INDEX {
                return Ok(None);
            }
            EcCodesError::check(err)?;
            return Err(EcCodesError::InternalError);
        }
        EcCodesError::check(err)?;
        Ok(Some(unsafe { Message::from_raw(handle)? }))
    }
}

impl Drop for Index {
    fn drop(&mut self) {
        if !self.index.is_null() {
            unsafe {
                eccodes_sys::codes_index_delete(self.index);
            }
        }
    }
}
