//! Typed get/set traits for message keys.
//!
//! Mirrors the `CodesGet`/`CodesSet` pattern from the metkit crate, adapted
//! for the C API: [`Handle::get`](crate::Handle::get) dispatches on the
//! requested type, [`Handle::set`](crate::Handle::set) on the value type.

use std::ffi::{CString, c_char, c_int, c_long};

use eccodes_sys as sys;

use crate::error::{Error, Result, check};
use crate::handle::{Handle, ckey};
use crate::kind::SingleKind;

// `long` keys cross the FFI boundary as `i64`; on every supported target
// (64-bit Linux/macOS) `c_long` is `i64`, so arrays are passed pointer-wise
// without conversion.
const _: () = assert!(size_of::<c_long>() == size_of::<i64>());

/// Trait for types that can be read from a [`Handle`] key.
pub trait KeyGet: Sized {
    /// Get a value for `key` from the handle.
    fn get_from<K: SingleKind>(handle: &Handle<K>, key: &str) -> Result<Self>;
}

/// Trait for types that can be written to a [`Handle`] key.
pub trait KeySet {
    /// Set `self` as the value of `key` on the handle.
    fn set_on<K: SingleKind>(self, handle: &mut Handle<K>, key: &str) -> Result<()>;
}

// ==================== KeyGet impls ====================

impl KeyGet for i64 {
    fn get_from<K: SingleKind>(handle: &Handle<K>, key: &str) -> Result<Self> {
        let key = ckey(key)?;
        let mut value: Self = 0;
        check!(sys::codes_get_long(
            handle.as_sys(),
            key.as_ptr(),
            &raw mut value
        ))?;
        Ok(value)
    }
}

impl KeyGet for f64 {
    fn get_from<K: SingleKind>(handle: &Handle<K>, key: &str) -> Result<Self> {
        let key = ckey(key)?;
        let mut value = 0.0_f64;
        check!(sys::codes_get_double(
            handle.as_sys(),
            key.as_ptr(),
            &raw mut value
        ))?;
        Ok(value)
    }
}

impl KeyGet for f32 {
    fn get_from<K: SingleKind>(handle: &Handle<K>, key: &str) -> Result<Self> {
        let key = ckey(key)?;
        let mut value = 0.0_f32;
        check!(sys::codes_get_float(
            handle.as_sys(),
            key.as_ptr(),
            &raw mut value
        ))?;
        Ok(value)
    }
}

impl KeyGet for String {
    fn get_from<K: SingleKind>(handle: &Handle<K>, key: &str) -> Result<Self> {
        let ckey_ = ckey(key)?;
        // +1 for the NUL codes_get_string appends.
        let mut len = handle.length(key)? + 1;
        let mut buf = vec![0_u8; len];
        check!(sys::codes_get_string(
            handle.as_sys(),
            ckey_.as_ptr(),
            buf.as_mut_ptr().cast::<c_char>(),
            &raw mut len,
        ))?;
        // `len` now counts the string including its NUL terminator.
        buf.truncate(len.saturating_sub(1));
        Ok(Self::from_utf8(buf).map_err(|e| e.utf8_error())?)
    }
}

impl KeyGet for Vec<u8> {
    fn get_from<K: SingleKind>(handle: &Handle<K>, key: &str) -> Result<Self> {
        let ckey_ = ckey(key)?;
        let mut len = handle.size(key)?;
        let mut buf = vec![0_u8; len];
        check!(sys::codes_get_bytes(
            handle.as_sys(),
            ckey_.as_ptr(),
            buf.as_mut_ptr(),
            &raw mut len,
        ))?;
        buf.truncate(len);
        Ok(buf)
    }
}

impl KeyGet for Vec<i64> {
    fn get_from<K: SingleKind>(handle: &Handle<K>, key: &str) -> Result<Self> {
        let ckey_ = ckey(key)?;
        let mut len = handle.size(key)?;
        let mut values = vec![0_i64; len];
        check!(sys::codes_get_long_array(
            handle.as_sys(),
            ckey_.as_ptr(),
            values.as_mut_ptr().cast::<c_long>(),
            &raw mut len,
        ))?;
        values.truncate(len);
        Ok(values)
    }
}

impl KeyGet for Vec<f64> {
    fn get_from<K: SingleKind>(handle: &Handle<K>, key: &str) -> Result<Self> {
        let ckey_ = ckey(key)?;
        let mut len = handle.size(key)?;
        let mut values = vec![0.0_f64; len];
        check!(sys::codes_get_double_array(
            handle.as_sys(),
            ckey_.as_ptr(),
            values.as_mut_ptr(),
            &raw mut len,
        ))?;
        values.truncate(len);
        Ok(values)
    }
}

impl KeyGet for Vec<f32> {
    fn get_from<K: SingleKind>(handle: &Handle<K>, key: &str) -> Result<Self> {
        let ckey_ = ckey(key)?;
        let mut len = handle.size(key)?;
        let mut values = vec![0.0_f32; len];
        check!(sys::codes_get_float_array(
            handle.as_sys(),
            ckey_.as_ptr(),
            values.as_mut_ptr(),
            &raw mut len,
        ))?;
        values.truncate(len);
        Ok(values)
    }
}

impl KeyGet for Vec<String> {
    fn get_from<K: SingleKind>(handle: &Handle<K>, key: &str) -> Result<Self> {
        let ckey_ = ckey(key)?;
        let mut len = handle.size(key)?;
        // The library allocates each string; we allocate the pointer array
        // and free the strings after conversion.
        let mut ptrs: Vec<*mut c_char> = vec![std::ptr::null_mut(); len];
        check!(sys::codes_get_string_array(
            handle.as_sys(),
            ckey_.as_ptr(),
            ptrs.as_mut_ptr(),
            &raw mut len,
        ))?;
        ptrs.truncate(len);
        let mut values = Self::with_capacity(len);
        let mut utf8_err = None;
        for ptr in ptrs {
            if ptr.is_null() {
                continue;
            }
            // SAFETY: non-null NUL-terminated string allocated by the library.
            match unsafe { std::ffi::CStr::from_ptr(ptr) }.to_str() {
                Ok(s) => values.push(s.to_owned()),
                Err(e) => utf8_err = Some(e),
            }
            // SAFETY: allocated with malloc by the library, ownership is ours.
            unsafe { libc::free(ptr.cast()) };
        }
        utf8_err.map_or(Ok(values), |e| Err(e.into()))
    }
}

// ==================== KeySet impls ====================

impl KeySet for i64 {
    fn set_on<K: SingleKind>(self, handle: &mut Handle<K>, key: &str) -> Result<()> {
        let key = ckey(key)?;
        check!(sys::codes_set_long(handle.as_sys(), key.as_ptr(), self))
    }
}

impl KeySet for f64 {
    fn set_on<K: SingleKind>(self, handle: &mut Handle<K>, key: &str) -> Result<()> {
        let key = ckey(key)?;
        check!(sys::codes_set_double(handle.as_sys(), key.as_ptr(), self))
    }
}

impl KeySet for &str {
    fn set_on<K: SingleKind>(self, handle: &mut Handle<K>, key: &str) -> Result<()> {
        let key = ckey(key)?;
        let value = CString::new(self)?;
        let mut len = self.len();
        check!(sys::codes_set_string(
            handle.as_sys(),
            key.as_ptr(),
            value.as_ptr(),
            &raw mut len,
        ))
    }
}

impl KeySet for String {
    fn set_on<K: SingleKind>(self, handle: &mut Handle<K>, key: &str) -> Result<()> {
        self.as_str().set_on(handle, key)
    }
}

impl KeySet for &[u8] {
    fn set_on<K: SingleKind>(self, handle: &mut Handle<K>, key: &str) -> Result<()> {
        let key = ckey(key)?;
        let mut len = self.len();
        check!(sys::codes_set_bytes(
            handle.as_sys(),
            key.as_ptr(),
            self.as_ptr(),
            &raw mut len,
        ))
    }
}

impl KeySet for &[i64] {
    fn set_on<K: SingleKind>(self, handle: &mut Handle<K>, key: &str) -> Result<()> {
        let key = ckey(key)?;
        check!(sys::codes_set_long_array(
            handle.as_sys(),
            key.as_ptr(),
            self.as_ptr().cast::<c_long>(),
            self.len(),
        ))
    }
}

impl KeySet for &[f64] {
    fn set_on<K: SingleKind>(self, handle: &mut Handle<K>, key: &str) -> Result<()> {
        let key = ckey(key)?;
        check!(sys::codes_set_double_array(
            handle.as_sys(),
            key.as_ptr(),
            self.as_ptr(),
            self.len(),
        ))
    }
}

impl KeySet for &[f32] {
    fn set_on<K: SingleKind>(self, handle: &mut Handle<K>, key: &str) -> Result<()> {
        let key = ckey(key)?;
        check!(sys::codes_set_float_array(
            handle.as_sys(),
            key.as_ptr(),
            self.as_ptr(),
            self.len(),
        ))
    }
}

impl KeySet for &[&str] {
    fn set_on<K: SingleKind>(self, handle: &mut Handle<K>, key: &str) -> Result<()> {
        let key = ckey(key)?;
        let values: Vec<CString> = self.iter().map(|s| ckey(s)).collect::<Result<_>>()?;
        let mut ptrs: Vec<*const c_char> = values.iter().map(|s| s.as_ptr()).collect();
        check!(sys::codes_set_string_array(
            handle.as_sys(),
            key.as_ptr(),
            ptrs.as_mut_ptr(),
            ptrs.len(),
        ))
    }
}

/// Wrapper routing a slice through `codes_set_force_double_array` /
/// `codes_set_force_float_array`, which allow setting read-only keys like
/// `codedValues`.
pub struct Force<T>(pub T);

impl KeySet for Force<&[f64]> {
    fn set_on<K: SingleKind>(self, handle: &mut Handle<K>, key: &str) -> Result<()> {
        let key = ckey(key)?;
        check!(sys::codes_set_force_double_array(
            handle.as_sys(),
            key.as_ptr(),
            self.0.as_ptr(),
            self.0.len(),
        ))
    }
}

impl KeySet for Force<&[f32]> {
    fn set_on<K: SingleKind>(self, handle: &mut Handle<K>, key: &str) -> Result<()> {
        let key = ckey(key)?;
        check!(sys::codes_set_force_float_array(
            handle.as_sys(),
            key.as_ptr(),
            self.0.as_ptr(),
            self.0.len(),
        ))
    }
}

// ==================== Element access ====================

impl<K: SingleKind> Handle<K> {
    /// A single element of a `double` array key
    /// (`codes_get_double_element`).
    pub fn get_double_element(&self, key: &str, index: usize) -> Result<f64> {
        let key = ckey(key)?;
        let index = c_int::try_from(index).map_err(|_| Error::InvalidArgument)?;
        let mut value = 0.0_f64;
        check!(sys::codes_get_double_element(
            self.as_sys(),
            key.as_ptr(),
            index,
            &raw mut value,
        ))?;
        Ok(value)
    }

    /// Selected elements of a `double` array key
    /// (`codes_get_double_elements`).
    pub fn get_double_elements(&self, key: &str, indexes: &[usize]) -> Result<Vec<f64>> {
        let key = ckey(key)?;
        let idx: Vec<c_int> = indexes
            .iter()
            .map(|&i| c_int::try_from(i).map_err(|_| Error::InvalidArgument))
            .collect::<Result<_>>()?;
        let mut values = vec![0.0_f64; idx.len()];
        check!(sys::codes_get_double_elements(
            self.as_sys(),
            key.as_ptr(),
            idx.as_ptr(),
            c_long::try_from(idx.len()).map_err(|_| Error::InvalidArgument)?,
            values.as_mut_ptr(),
        ))?;
        Ok(values)
    }

    /// A single element of a `float` array key (`codes_get_float_element`).
    pub fn get_float_element(&self, key: &str, index: usize) -> Result<f32> {
        let key = ckey(key)?;
        let index = c_int::try_from(index).map_err(|_| Error::InvalidArgument)?;
        let mut value = 0.0_f32;
        check!(sys::codes_get_float_element(
            self.as_sys(),
            key.as_ptr(),
            index,
            &raw mut value,
        ))?;
        Ok(value)
    }

    /// Selected elements of a `float` array key
    /// (`codes_get_float_elements`).
    pub fn get_float_elements(&self, key: &str, indexes: &[usize]) -> Result<Vec<f32>> {
        let key = ckey(key)?;
        let idx: Vec<c_int> = indexes
            .iter()
            .map(|&i| c_int::try_from(i).map_err(|_| Error::InvalidArgument))
            .collect::<Result<_>>()?;
        let mut values = vec![0.0_f32; idx.len()];
        check!(sys::codes_get_float_elements(
            self.as_sys(),
            key.as_ptr(),
            idx.as_ptr(),
            c_long::try_from(idx.len()).map_err(|_| Error::InvalidArgument)?,
            values.as_mut_ptr(),
        ))?;
        Ok(values)
    }
}
