//! Message handle — the core type for reading/writing GRIB and BUFR data.

use crate::error::{EcCodesError, Result};
use crate::keys_iterator::KeysIterator;
use crate::types::ProductKind;
#[cfg(feature = "advanced")]
use crate::types::KeyType;
use std::ffi::{CStr, CString};
use std::os::raw::{c_int, c_long};
use std::ptr;

// ---------------------------------------------------------------------------
// Trait: GetKey — enables generic get
// ---------------------------------------------------------------------------

/// Trait for types that can be read from a message key.
///
/// Collapses `codes_get_long`, `codes_get_double`, `codes_get_string`,
/// `codes_get_long_array`, `codes_get_double_array`, etc. into a single
/// `message.get::<T>(key)`.
pub trait GetKey: Sized {
    fn get_from_message(message: &Message, key: &str) -> Result<Self>;
}

/// Trait for types that can be written to a message key.
pub trait SetKey {
    fn set_on_message(&self, message: &mut Message, key: &str) -> Result<()>;
}

// -- Scalar impls --

impl GetKey for i64 {
    fn get_from_message(message: &Message, key: &str) -> Result<Self> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut value: c_long = 0;
        let err = unsafe {
            eccodes_sys::codes_get_long(message.handle, c_key.as_ptr(), &mut value)
        };
        EcCodesError::check(err)?;
        Ok(value as i64)
    }
}

impl SetKey for i64 {
    fn set_on_message(&self, message: &mut Message, key: &str) -> Result<()> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let err = unsafe {
            eccodes_sys::codes_set_long(message.handle, c_key.as_ptr(), *self as c_long)
        };
        EcCodesError::check(err)
    }
}

impl GetKey for f64 {
    fn get_from_message(message: &Message, key: &str) -> Result<Self> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut value: f64 = 0.0;
        let err = unsafe {
            eccodes_sys::codes_get_double(message.handle, c_key.as_ptr(), &mut value)
        };
        EcCodesError::check(err)?;
        Ok(value)
    }
}

impl SetKey for f64 {
    fn set_on_message(&self, message: &mut Message, key: &str) -> Result<()> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let err = unsafe {
            eccodes_sys::codes_set_double(message.handle, c_key.as_ptr(), *self)
        };
        EcCodesError::check(err)
    }
}

impl GetKey for f32 {
    fn get_from_message(message: &Message, key: &str) -> Result<Self> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut value: f32 = 0.0;
        let err = unsafe {
            eccodes_sys::codes_get_float(message.handle, c_key.as_ptr(), &mut value)
        };
        EcCodesError::check(err)?;
        Ok(value)
    }
}

impl GetKey for String {
    fn get_from_message(message: &Message, key: &str) -> Result<Self> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        // First get the length (includes null terminator)
        let mut len: usize = 0;
        let err = unsafe {
            eccodes_sys::codes_get_length(message.handle, c_key.as_ptr(), &mut len)
        };
        EcCodesError::check(err)?;
        if len == 0 {
            return Ok(String::new());
        }
        // Allocate buffer and get string
        let mut buf = vec![0u8; len];
        let err = unsafe {
            eccodes_sys::codes_get_string(
                message.handle,
                c_key.as_ptr(),
                buf.as_mut_ptr() as *mut i8,
                &mut len,
            )
        };
        EcCodesError::check(err)?;
        // Convert to Rust string (trim null terminator)
        let s = unsafe { CStr::from_ptr(buf.as_ptr() as *const i8) };
        Ok(s.to_string_lossy().into_owned())
    }
}

impl SetKey for str {
    fn set_on_message(&self, message: &mut Message, key: &str) -> Result<()> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let c_val = CString::new(self).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut len = self.len();
        let err = unsafe {
            eccodes_sys::codes_set_string(
                message.handle,
                c_key.as_ptr(),
                c_val.as_ptr(),
                &mut len,
            )
        };
        EcCodesError::check(err)
    }
}

impl SetKey for &str {
    fn set_on_message(&self, message: &mut Message, key: &str) -> Result<()> {
        (*self as &str).set_on_message(message, key)
    }
}

impl SetKey for String {
    fn set_on_message(&self, message: &mut Message, key: &str) -> Result<()> {
        self.as_str().set_on_message(message, key)
    }
}

// -- Byte array --

impl GetKey for Vec<u8> {
    fn get_from_message(message: &Message, key: &str) -> Result<Self> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut len: usize = 0;
        let err = unsafe {
            eccodes_sys::codes_get_length(message.handle, c_key.as_ptr(), &mut len)
        };
        EcCodesError::check(err)?;
        if len == 0 {
            return Ok(Vec::new());
        }
        let mut buf = vec![0u8; len];
        let err = unsafe {
            eccodes_sys::codes_get_bytes(
                message.handle,
                c_key.as_ptr(),
                buf.as_mut_ptr(),
                &mut len,
            )
        };
        EcCodesError::check(err)?;
        buf.truncate(len);
        Ok(buf)
    }
}

impl SetKey for [u8] {
    fn set_on_message(&self, message: &mut Message, key: &str) -> Result<()> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut len = self.len();
        let err = unsafe {
            eccodes_sys::codes_set_bytes(
                message.handle,
                c_key.as_ptr(),
                self.as_ptr(),
                &mut len,
            )
        };
        EcCodesError::check(err)
    }
}

// -- Array impls --

impl GetKey for Vec<i64> {
    fn get_from_message(message: &Message, key: &str) -> Result<Self> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut size: usize = 0;
        let err = unsafe {
            eccodes_sys::codes_get_size(message.handle, c_key.as_ptr(), &mut size)
        };
        EcCodesError::check(err)?;
        let mut vals = vec![0 as c_long; size];
        let err = unsafe {
            eccodes_sys::codes_get_long_array(
                message.handle,
                c_key.as_ptr(),
                vals.as_mut_ptr(),
                &mut size,
            )
        };
        EcCodesError::check(err)?;
        vals.truncate(size);
        Ok(vals.into_iter().map(|v| v as i64).collect())
    }
}

impl SetKey for [i64] {
    fn set_on_message(&self, message: &mut Message, key: &str) -> Result<()> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let vals: Vec<c_long> = self.iter().map(|&v| v as c_long).collect();
        let err = unsafe {
            eccodes_sys::codes_set_long_array(
                message.handle,
                c_key.as_ptr(),
                vals.as_ptr(),
                vals.len(),
            )
        };
        EcCodesError::check(err)
    }
}

impl GetKey for Vec<f64> {
    fn get_from_message(message: &Message, key: &str) -> Result<Self> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut size: usize = 0;
        let err = unsafe {
            eccodes_sys::codes_get_size(message.handle, c_key.as_ptr(), &mut size)
        };
        EcCodesError::check(err)?;
        let mut vals = vec![0.0f64; size];
        let err = unsafe {
            eccodes_sys::codes_get_double_array(
                message.handle,
                c_key.as_ptr(),
                vals.as_mut_ptr(),
                &mut size,
            )
        };
        EcCodesError::check(err)?;
        vals.truncate(size);
        Ok(vals)
    }
}

impl SetKey for [f64] {
    fn set_on_message(&self, message: &mut Message, key: &str) -> Result<()> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let err = unsafe {
            eccodes_sys::codes_set_double_array(
                message.handle,
                c_key.as_ptr(),
                self.as_ptr(),
                self.len(),
            )
        };
        EcCodesError::check(err)
    }
}

impl GetKey for Vec<f32> {
    fn get_from_message(message: &Message, key: &str) -> Result<Self> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut size: usize = 0;
        let err = unsafe {
            eccodes_sys::codes_get_size(message.handle, c_key.as_ptr(), &mut size)
        };
        EcCodesError::check(err)?;
        let mut vals = vec![0.0f32; size];
        let err = unsafe {
            eccodes_sys::codes_get_float_array(
                message.handle,
                c_key.as_ptr(),
                vals.as_mut_ptr(),
                &mut size,
            )
        };
        EcCodesError::check(err)?;
        vals.truncate(size);
        Ok(vals)
    }
}

impl SetKey for [f32] {
    fn set_on_message(&self, message: &mut Message, key: &str) -> Result<()> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let err = unsafe {
            eccodes_sys::codes_set_float_array(
                message.handle,
                c_key.as_ptr(),
                self.as_ptr(),
                self.len(),
            )
        };
        EcCodesError::check(err)
    }
}

impl GetKey for Vec<String> {
    fn get_from_message(message: &Message, key: &str) -> Result<Self> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut size: usize = 0;
        let err = unsafe {
            eccodes_sys::codes_get_size(message.handle, c_key.as_ptr(), &mut size)
        };
        EcCodesError::check(err)?;
        let mut ptrs: Vec<*mut i8> = vec![ptr::null_mut(); size];
        let err = unsafe {
            eccodes_sys::codes_get_string_array(
                message.handle,
                c_key.as_ptr(),
                ptrs.as_mut_ptr(),
                &mut size,
            )
        };
        EcCodesError::check(err)?;
        let mut result = Vec::with_capacity(size);
        for ptr in ptrs.iter().take(size) {
            if ptr.is_null() {
                result.push(String::new());
            } else {
                let s = unsafe { CStr::from_ptr(*ptr) };
                result.push(s.to_string_lossy().into_owned());
                // SAFETY: strings were allocated by the C library via malloc
                unsafe { crate::libc_free(*ptr as *mut std::ffi::c_void) };
            }
        }
        Ok(result)
    }
}

impl SetKey for [String] {
    fn set_on_message(&self, message: &mut Message, key: &str) -> Result<()> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let c_strs: Vec<CString> = self
            .iter()
            .map(|s| CString::new(s.as_str()).map_err(|_| EcCodesError::InvalidArgument))
            .collect::<Result<Vec<_>>>()?;
        let ptrs: Vec<*const i8> = c_strs.iter().map(|cs| cs.as_ptr()).collect();
        let err = unsafe {
            eccodes_sys::codes_set_string_array(
                message.handle,
                c_key.as_ptr(),
                ptrs.as_ptr(),
                ptrs.len(),
            )
        };
        EcCodesError::check(err)
    }
}

// ---------------------------------------------------------------------------
// Advanced traits
// ---------------------------------------------------------------------------

/// Trait for types that support per-element access (f64, f32).
#[cfg(feature = "advanced")]
pub trait GetElementKey: Sized {
    fn get_element_from_message(message: &Message, key: &str, index: usize) -> Result<Self>;
    fn get_elements_from_message(message: &Message, key: &str, indices: &[i32]) -> Result<Vec<Self>>;
}

#[cfg(feature = "advanced")]
impl GetElementKey for f64 {
    fn get_element_from_message(message: &Message, key: &str, index: usize) -> Result<Self> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut value: f64 = 0.0;
        let err = unsafe {
            eccodes_sys::codes_get_double_element(
                message.handle, c_key.as_ptr(), index as c_int, &mut value,
            )
        };
        EcCodesError::check(err)?;
        Ok(value)
    }
    fn get_elements_from_message(message: &Message, key: &str, indices: &[i32]) -> Result<Vec<Self>> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut vals = vec![0.0f64; indices.len()];
        let err = unsafe {
            eccodes_sys::codes_get_double_elements(
                message.handle, c_key.as_ptr(),
                indices.as_ptr() as *const c_int, indices.len() as c_long, vals.as_mut_ptr(),
            )
        };
        EcCodesError::check(err)?;
        Ok(vals)
    }
}

#[cfg(feature = "advanced")]
impl GetElementKey for f32 {
    fn get_element_from_message(message: &Message, key: &str, index: usize) -> Result<Self> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut value: f32 = 0.0;
        let err = unsafe {
            eccodes_sys::codes_get_float_element(
                message.handle, c_key.as_ptr(), index as c_int, &mut value,
            )
        };
        EcCodesError::check(err)?;
        Ok(value)
    }
    fn get_elements_from_message(message: &Message, key: &str, indices: &[i32]) -> Result<Vec<Self>> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut vals = vec![0.0f32; indices.len()];
        let err = unsafe {
            eccodes_sys::codes_get_float_elements(
                message.handle, c_key.as_ptr(),
                indices.as_ptr() as *const c_int, indices.len() as c_long, vals.as_mut_ptr(),
            )
        };
        EcCodesError::check(err)?;
        Ok(vals)
    }
}

/// Trait for force-setting read-only keys.
#[cfg(feature = "advanced")]
pub trait SetForceKey {
    fn set_force_on_message(&self, message: &mut Message, key: &str) -> Result<()>;
}

#[cfg(feature = "advanced")]
impl SetForceKey for [f64] {
    fn set_force_on_message(&self, message: &mut Message, key: &str) -> Result<()> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let err = unsafe {
            eccodes_sys::codes_set_force_double_array(
                message.handle, c_key.as_ptr(), self.as_ptr(), self.len(),
            )
        };
        EcCodesError::check(err)
    }
}

#[cfg(feature = "advanced")]
impl SetForceKey for [f32] {
    fn set_force_on_message(&self, message: &mut Message, key: &str) -> Result<()> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let err = unsafe {
            eccodes_sys::codes_set_force_float_array(
                message.handle, c_key.as_ptr(), self.as_ptr(), self.len(),
            )
        };
        EcCodesError::check(err)
    }
}

// ---------------------------------------------------------------------------
// Message
// ---------------------------------------------------------------------------

/// A message handle wrapping `codes_handle*`.
///
/// Provides access to keys via generic [`get`](Message::get) and
/// [`set`](Message::set) methods.  Automatically freed on drop.
///
/// # Thread safety
///
/// `Message` is `Send` but not `Sync`: ownership can be transferred between
/// threads, but a single `Message` must not be shared concurrently.
/// The underlying ecCodes library is **not** thread-safe; concurrent access
/// to the default context from multiple threads is undefined behaviour.
pub struct Message {
    pub(crate) handle: *mut eccodes_sys::codes_handle,
}

// SAFETY: A `Message` exclusively owns its `codes_handle*`. Transferring
// ownership to another thread is safe because no other thread holds a
// reference. `Sync` is intentionally *not* implemented because ecCodes
// handles are not safe to access concurrently.
unsafe impl Send for Message {}

impl Message {
    /// Create a `Message` from a raw handle pointer.
    ///
    /// # Safety
    /// The pointer must be non-null, valid, and not used elsewhere after
    /// this call (exclusive ownership is transferred).
    pub(crate) unsafe fn from_raw(handle: *mut eccodes_sys::codes_handle) -> Result<Self> {
        if handle.is_null() {
            return Err(EcCodesError::NullHandle);
        }
        Ok(Message { handle })
    }

    /// Get the raw handle pointer (const).
    #[cfg(feature = "advanced")]
    pub(crate) fn as_ptr(&self) -> *const eccodes_sys::codes_handle {
        self.handle
    }

    /// Get the mutable raw handle pointer.
    #[cfg(feature = "advanced")]
    pub(crate) fn as_mut_ptr(&mut self) -> *mut eccodes_sys::codes_handle {
        self.handle
    }

    // ── Constructors (basic) ─────────────────────────────────────────────

    /// Create a message from a named GRIB sample.
    pub fn from_grib_sample(sample_name: &str) -> Result<Self> {
        let c_name = CString::new(sample_name).map_err(|_| EcCodesError::InvalidArgument)?;
        let handle = unsafe {
            eccodes_sys::codes_grib_handle_new_from_samples(ptr::null_mut(), c_name.as_ptr())
        };
        unsafe { Self::from_raw(handle) }
    }

    /// Create a message from a named BUFR sample.
    pub fn from_bufr_sample(sample_name: &str) -> Result<Self> {
        let c_name = CString::new(sample_name).map_err(|_| EcCodesError::InvalidArgument)?;
        let handle = unsafe {
            eccodes_sys::codes_bufr_handle_new_from_samples(ptr::null_mut(), c_name.as_ptr())
        };
        unsafe { Self::from_raw(handle) }
    }

    /// Create a message from a named sample (auto-detected type).
    pub fn from_sample(sample_name: &str) -> Result<Self> {
        let c_name = CString::new(sample_name).map_err(|_| EcCodesError::InvalidArgument)?;
        let handle = unsafe {
            eccodes_sys::codes_handle_new_from_samples(ptr::null_mut(), c_name.as_ptr())
        };
        unsafe { Self::from_raw(handle) }
    }

    // ── Constructors (advanced) ──────────────────────────────────────────

    /// Create a message from raw bytes (zero-copy, message not freed).
    #[cfg(feature = "advanced")]
    pub fn from_bytes(data: &[u8]) -> Result<Self> {
        let handle = unsafe {
            eccodes_sys::codes_handle_new_from_message(
                ptr::null_mut(), data.as_ptr() as *const std::ffi::c_void, data.len(),
            )
        };
        unsafe { Self::from_raw(handle) }
    }

    /// Create a message from raw bytes (copied).
    #[cfg(feature = "advanced")]
    pub fn from_bytes_copy(data: &[u8]) -> Result<Self> {
        let handle = unsafe {
            eccodes_sys::codes_handle_new_from_message_copy(
                ptr::null_mut(), data.as_ptr() as *const std::ffi::c_void, data.len(),
            )
        };
        unsafe { Self::from_raw(handle) }
    }

    // ── Get (basic) ──────────────────────────────────────────────────────

    /// Get a value from a key. Type determined by `T`:
    /// - `i64`, `f64`, `f32`, `String`, `Vec<u8>`, `Vec<i64>`, `Vec<f64>`, etc.
    #[must_use]
    pub fn get<T: GetKey>(&self, key: &str) -> Result<T> {
        T::get_from_message(self, key)
    }

    /// Get the number of values for an array key.
    #[must_use]
    pub fn get_size(&self, key: &str) -> Result<usize> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut size: usize = 0;
        let err = unsafe {
            eccodes_sys::codes_get_size(self.handle, c_key.as_ptr(), &mut size)
        };
        EcCodesError::check(err)?;
        Ok(size)
    }

    /// Get the string-representation length of a key.
    #[must_use]
    pub fn get_length(&self, key: &str) -> Result<usize> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut len: usize = 0;
        let err = unsafe {
            eccodes_sys::codes_get_length(self.handle, c_key.as_ptr(), &mut len)
        };
        EcCodesError::check(err)?;
        Ok(len)
    }

    /// Check if a key has the value 'missing'.
    #[must_use]
    pub fn is_missing(&self, key: &str) -> Result<bool> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut err: c_int = 0;
        let result = unsafe {
            eccodes_sys::codes_is_missing(self.handle, c_key.as_ptr(), &mut err)
        };
        EcCodesError::check(err)?;
        Ok(result != 0)
    }

    /// Check if a key exists in the message.
    #[must_use]
    pub fn is_defined(&self, key: &str) -> bool {
        let c_key = match CString::new(key) {
            Ok(s) => s,
            Err(_) => return false,
        };
        unsafe { eccodes_sys::codes_is_defined(self.handle, c_key.as_ptr()) != 0 }
    }

    /// Get the product kind of this message.
    #[must_use]
    pub fn product_kind(&self) -> Result<ProductKind> {
        let mut pk = eccodes_sys::ProductKind::PRODUCT_ANY;
        let err = unsafe {
            eccodes_sys::codes_get_product_kind(self.handle, &mut pk)
        };
        EcCodesError::check(err)?;
        Ok(ProductKind::from_sys(pk))
    }

    // ── Get (advanced) ───────────────────────────────────────────────────

    /// Get the byte offset of a key within the message.
    #[cfg(feature = "advanced")]
    #[must_use]
    pub fn get_offset(&self, key: &str) -> Result<usize> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut offset: usize = 0;
        let err = unsafe {
            eccodes_sys::codes_get_offset(self.handle, c_key.as_ptr(), &mut offset)
        };
        EcCodesError::check(err)?;
        Ok(offset)
    }

    /// Get the native type of a key.
    #[cfg(feature = "advanced")]
    #[must_use]
    pub fn native_type(&self, key: &str) -> Result<KeyType> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut t: c_int = 0;
        let err = unsafe {
            eccodes_sys::codes_get_native_type(self.handle, c_key.as_ptr(), &mut t)
        };
        EcCodesError::check(err)?;
        Ok(KeyType::from_c(t))
    }

    /// Check if a key is computed (virtual) rather than coded.
    #[cfg(feature = "advanced")]
    #[must_use]
    pub fn key_is_computed(&self, key: &str) -> Result<bool> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let mut err: c_int = 0;
        let result = unsafe {
            eccodes_sys::codes_key_is_computed(self.handle, c_key.as_ptr(), &mut err)
        };
        EcCodesError::check(err)?;
        Ok(result != 0)
    }

    /// Get the i-th element of an array key.
    #[cfg(feature = "advanced")]
    #[must_use]
    pub fn get_element<T: GetElementKey>(&self, key: &str, index: usize) -> Result<T> {
        T::get_element_from_message(self, key, index)
    }

    /// Get multiple elements from an array key by index.
    #[cfg(feature = "advanced")]
    #[must_use]
    pub fn get_elements<T: GetElementKey>(&self, key: &str, indices: &[i32]) -> Result<Vec<T>> {
        T::get_elements_from_message(self, key, indices)
    }

    // ── Set (basic) ──────────────────────────────────────────────────────

    /// Set a value on a key. Type determined by `T`.
    pub fn set<T: SetKey + ?Sized>(&mut self, key: &str, value: &T) -> Result<()> {
        value.set_on_message(self, key)
    }

    /// Set a key to 'missing'.
    pub fn set_missing(&mut self, key: &str) -> Result<()> {
        let c_key = CString::new(key).map_err(|_| EcCodesError::InvalidArgument)?;
        let err = unsafe { eccodes_sys::codes_set_missing(self.handle, c_key.as_ptr()) };
        EcCodesError::check(err)
    }

    // ── Set (advanced) ───────────────────────────────────────────────────

    /// Force-set an array value, bypassing read-only checks.
    #[cfg(feature = "advanced")]
    pub fn set_force<T: SetForceKey + ?Sized>(&mut self, key: &str, value: &T) -> Result<()> {
        value.set_force_on_message(self, key)
    }

    // ── Write (basic) ────────────────────────────────────────────────────

    /// Write this message to a file (creates or truncates).
    pub fn write_to_file(&self, path: &str) -> Result<()> {
        let c_path = CString::new(path).map_err(|_| EcCodesError::InvalidArgument)?;
        let c_mode = CString::new("w").unwrap();
        let err = unsafe {
            eccodes_sys::codes_write_message(self.handle, c_path.as_ptr(), c_mode.as_ptr())
        };
        EcCodesError::check(err)
    }

    /// Append this message to a file (creates if not exists).
    pub fn append_to_file(&self, path: &str) -> Result<()> {
        let c_path = CString::new(path).map_err(|_| EcCodesError::InvalidArgument)?;
        let c_mode = CString::new("a").unwrap();
        let err = unsafe {
            eccodes_sys::codes_write_message(self.handle, c_path.as_ptr(), c_mode.as_ptr())
        };
        EcCodesError::check(err)
    }

    // ── Write (advanced) ─────────────────────────────────────────────────

    /// Get a copy of the raw encoded message bytes.
    #[cfg(feature = "advanced")]
    #[must_use]
    pub fn to_bytes(&self) -> Result<Vec<u8>> {
        let mut msg_ptr: *const std::ffi::c_void = ptr::null();
        let mut msg_len: usize = 0;
        let err = unsafe {
            eccodes_sys::codes_get_message(self.handle, &mut msg_ptr, &mut msg_len)
        };
        EcCodesError::check(err)?;
        // SAFETY: codes_get_message guarantees msg_ptr is valid for msg_len bytes
        let slice = unsafe { std::slice::from_raw_parts(msg_ptr as *const u8, msg_len) };
        Ok(slice.to_vec())
    }

    /// Get the byte offset of this message within its source file.
    #[cfg(feature = "advanced")]
    #[must_use]
    pub fn message_offset(&self) -> Result<i64> {
        let mut offset: i64 = 0;
        let err = unsafe {
            eccodes_sys::codes_get_message_offset(self.handle, &mut offset)
        };
        EcCodesError::check(err)?;
        Ok(offset)
    }

    /// Get the total size of the encoded message in bytes.
    #[cfg(feature = "advanced")]
    #[must_use]
    pub fn message_size(&self) -> Result<usize> {
        let mut size: usize = 0;
        let err = unsafe {
            eccodes_sys::codes_get_message_size(self.handle, &mut size)
        };
        EcCodesError::check(err)?;
        Ok(size)
    }

    // ── Keys iteration (basic) ───────────────────────────────────────────

    /// Iterate over all keys in this message.
    ///
    /// # Note
    /// The returned iterator borrows `self` mutably through a raw pointer
    /// internally. Do not create multiple iterators on the same message
    /// simultaneously.
    pub fn keys(&self) -> KeysIterator<'_> {
        KeysIterator::new(self, 0, None)
    }

    /// Iterate over keys with filter flags.
    pub fn keys_filtered(&self, flags: u64) -> KeysIterator<'_> {
        KeysIterator::new(self, flags, None)
    }

    /// Iterate over keys in a specific namespace.
    pub fn keys_in_namespace<'a>(&'a self, namespace: &str) -> KeysIterator<'a> {
        KeysIterator::new(self, 0, Some(namespace))
    }

    // ── Copy (advanced) ──────────────────────────────────────────────────

    /// Clone only the header (no data section).
    #[cfg(feature = "advanced")]
    pub fn clone_headers_only(&self) -> Result<Self> {
        let handle = unsafe { eccodes_sys::codes_handle_clone_headers_only(self.handle) };
        unsafe { Self::from_raw(handle) }
    }

    /// Copy keys from a namespace of another message into this one.
    #[cfg(feature = "advanced")]
    pub fn copy_namespace(&mut self, namespace: &str, src: &Message) -> Result<()> {
        let c_ns = CString::new(namespace).map_err(|_| EcCodesError::InvalidArgument)?;
        let err = unsafe {
            eccodes_sys::codes_copy_namespace(
                self.handle, c_ns.as_ptr(), src.handle as *mut _,
            )
        };
        EcCodesError::check(err)
    }

    // ── Geo (advanced) ───────────────────────────────────────────────────

    /// Create a geographic iterator for this GRIB message.
    #[cfg(feature = "advanced")]
    pub fn geo_iterator(&self) -> Result<crate::geo_iterator::GeoIterator<'_>> {
        crate::geo_iterator::GeoIterator::new(self)
    }

    /// Get all lat/lon/value data in one call.
    #[cfg(feature = "advanced")]
    #[must_use]
    pub fn get_data(&self) -> Result<Vec<crate::geo_iterator::GeoPoint>> {
        let num_points: i64 = self.get("numberOfPoints")?;
        let n = num_points as usize;
        let mut lats = vec![0.0f64; n];
        let mut lons = vec![0.0f64; n];
        let mut values = vec![0.0f64; n];
        let err = unsafe {
            eccodes_sys::codes_grib_get_data(
                self.handle, lats.as_mut_ptr(), lons.as_mut_ptr(), values.as_mut_ptr(),
            )
        };
        EcCodesError::check(err)?;
        Ok((0..n)
            .map(|i| crate::geo_iterator::GeoPoint {
                lat: lats[i], lon: lons[i], value: values[i],
            })
            .collect())
    }

    /// Create a nearest-neighbour search object.
    #[cfg(feature = "advanced")]
    pub fn nearest(&self) -> Result<crate::nearest::Nearest<'_>> {
        crate::nearest::Nearest::new(self)
    }
}

impl Clone for Message {
    fn clone(&self) -> Self {
        let handle = unsafe { eccodes_sys::codes_handle_clone(self.handle) };
        if handle.is_null() {
            panic!("codes_handle_clone returned NULL");
        }
        Message { handle }
    }
}

impl std::fmt::Debug for Message {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let mut dbg = f.debug_struct("Message");
        if let Ok(kind) = self.product_kind() {
            dbg.field("product", &kind);
        }
        if let Ok(name) = self.get::<String>("shortName") {
            dbg.field("shortName", &name);
        }
        if let Ok(level) = self.get::<i64>("level") {
            dbg.field("level", &level);
        }
        if let Ok(n) = self.get::<i64>("numberOfPoints") {
            dbg.field("numberOfPoints", &n);
        }
        dbg.finish()
    }
}

impl Drop for Message {
    fn drop(&mut self) {
        if !self.handle.is_null() {
            unsafe { eccodes_sys::codes_handle_delete(self.handle); }
        }
    }
}

// ---------------------------------------------------------------------------
// MessageIterator
// ---------------------------------------------------------------------------

/// Iterator over messages in a file.
///
/// # Thread safety
///
/// `MessageIterator` is `Send` but not `Sync`. The underlying C `FILE*`
/// and ecCodes context are not safe for concurrent access.
///
/// # Example
///
/// ```rust,no_run
/// use eccodes::MessageIterator;
///
/// for message in MessageIterator::from_grib_file("data.grib2").unwrap() {
///     let message = message.unwrap();
///     let name: String = message.get("shortName").unwrap();
///     println!("{name}");
/// }
/// ```
pub struct MessageIterator {
    file: *mut eccodes_sys::FILE,
    product: ProductKind,
    done: bool,
}

// SAFETY: `MessageIterator` exclusively owns the `FILE*`. Transferring
// ownership to another thread is safe. `Sync` is intentionally not
// implemented because concurrent reads on the same FILE* are UB.
unsafe impl Send for MessageIterator {}

impl MessageIterator {
    /// Open a file and iterate over messages of any product type.
    pub fn from_file(path: &str, product: ProductKind) -> Result<Self> {
        let c_path = CString::new(path).map_err(|_| EcCodesError::InvalidArgument)?;
        let c_mode = CString::new("rb").unwrap();
        let file = unsafe { eccodes_sys::fopen(c_path.as_ptr(), c_mode.as_ptr()) };
        if file.is_null() {
            return Err(EcCodesError::FileNotFound);
        }
        Ok(MessageIterator { file, product, done: false })
    }

    /// Open a GRIB file and iterate over its messages.
    pub fn from_grib_file(path: &str) -> Result<Self> {
        Self::from_file(path, ProductKind::Grib)
    }

    /// Open a BUFR file and iterate over its messages.
    pub fn from_bufr_file(path: &str) -> Result<Self> {
        Self::from_file(path, ProductKind::Bufr)
    }
}

impl Iterator for MessageIterator {
    type Item = Result<Message>;

    fn next(&mut self) -> Option<Self::Item> {
        if self.done {
            return None;
        }
        let mut err: c_int = 0;
        let handle = unsafe {
            eccodes_sys::codes_handle_new_from_file(
                ptr::null_mut(), self.file, self.product.to_sys(), &mut err,
            )
        };
        if handle.is_null() {
            self.done = true;
            if err == eccodes_sys::CODES_SUCCESS || err == eccodes_sys::CODES_END_OF_FILE {
                return None;
            }
            return Some(Err(EcCodesError::from_code(err)));
        }
        if err != eccodes_sys::CODES_SUCCESS {
            self.done = true;
            return Some(Err(EcCodesError::from_code(err)));
        }
        Some(unsafe { Message::from_raw(handle) })
    }
}

impl Drop for MessageIterator {
    fn drop(&mut self) {
        if !self.file.is_null() {
            unsafe { eccodes_sys::fclose(self.file); }
        }
    }
}
