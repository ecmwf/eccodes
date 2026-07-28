//! `Context` — configuration of the process-global eccodes context.

use std::ffi::{CStr, CString};
use std::path::Path;
use std::ptr;

use eccodes_sys as sys;

use crate::error::{Error, Result};

/// The default eccodes context.
///
/// The context is process-global and owned by the C library, so this type is
/// zero-sized and only exposes the configuration entry points. All handles
/// created by this crate use the default context.
pub struct Context;

impl Context {
    /// Override the definitions path (`codes_context_set_definitions_path`).
    pub fn set_definitions_path(path: impl AsRef<Path>) -> Result<()> {
        let path = CString::new(path.as_ref().as_os_str().as_encoded_bytes())?;
        // SAFETY: NULL context selects the default context; `path` is a valid
        // NUL-terminated string, copied by the library.
        unsafe { sys::codes_context_set_definitions_path(ptr::null_mut(), path.as_ptr()) };
        Ok(())
    }

    /// Override the samples path (`codes_context_set_samples_path`).
    pub fn set_samples_path(path: impl AsRef<Path>) -> Result<()> {
        let path = CString::new(path.as_ref().as_os_str().as_encoded_bytes())?;
        // SAFETY: as for `set_definitions_path`.
        unsafe { sys::codes_context_set_samples_path(ptr::null_mut(), path.as_ptr()) };
        Ok(())
    }

    /// The samples path in effect (`codes_samples_path`).
    pub fn samples_path() -> Result<String> {
        // SAFETY: NULL context selects the default context.
        let ptr = unsafe { sys::codes_samples_path(ptr::null()) };
        if ptr.is_null() {
            return Err(Error::NullPointer);
        }
        // SAFETY: non-null NUL-terminated string owned by the library.
        Ok(unsafe { CStr::from_ptr(ptr) }.to_str()?.to_owned())
    }

    /// Toggle debug output (`codes_context_set_debug`).
    pub fn set_debug(enabled: bool) {
        // SAFETY: NULL context selects the default context.
        unsafe { sys::codes_context_set_debug(ptr::null_mut(), i32::from(enabled)) };
    }

    /// Toggle transparent handling of multi-field GRIB messages when reading
    /// from files (`codes_grib_multi_support_on`/`_off`).
    pub fn grib_multi_support(enabled: bool) {
        // SAFETY: NULL context selects the default context.
        unsafe {
            if enabled {
                sys::codes_grib_multi_support_on(ptr::null_mut());
            } else {
                sys::codes_grib_multi_support_off(ptr::null_mut());
            }
        }
    }
}
