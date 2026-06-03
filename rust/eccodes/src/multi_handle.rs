//! Multi-field GRIB handle (GRIB edition 2 only).
//!
//! Requires `features = ["advanced"]`.

use crate::error::{EcCodesError, Result};
use crate::message::Message;
use std::ffi::CString;
use std::os::raw::c_int;
use std::ptr;

/// A multi-field GRIB handle for building multi-field messages.
pub struct MultiHandle {
    handle: *mut eccodes_sys::codes_multi_handle,
}

unsafe impl Send for MultiHandle {}

impl MultiHandle {
    /// Create a new empty multi-field handle.
    pub fn new() -> Result<Self> {
        let handle = unsafe { eccodes_sys::codes_grib_multi_handle_new(ptr::null_mut()) };
        if handle.is_null() {
            return Err(EcCodesError::InternalError);
        }
        Ok(MultiHandle { handle })
    }

    /// Append sections from a message starting at `start_section`.
    pub fn append(&mut self, message: &mut Message, start_section: i32) -> Result<()> {
        let err = unsafe {
            eccodes_sys::codes_grib_multi_handle_append(
                message.as_mut_ptr(),
                start_section as c_int,
                self.handle,
            )
        };
        EcCodesError::check(err)
    }

    /// Write the multi-field message to a file.
    pub fn write(&self, path: &str) -> Result<()> {
        let c_path = CString::new(path).map_err(|_| EcCodesError::InvalidArgument)?;
        let c_mode = CString::new("wb").unwrap();
        let file = unsafe { eccodes_sys::fopen(c_path.as_ptr(), c_mode.as_ptr()) };
        if file.is_null() {
            return Err(EcCodesError::IoProblem);
        }
        let err = unsafe {
            eccodes_sys::codes_grib_multi_handle_write(self.handle, file)
        };
        unsafe { eccodes_sys::fclose(file) };
        EcCodesError::check(err)
    }
}

impl Drop for MultiHandle {
    fn drop(&mut self) {
        if !self.handle.is_null() {
            unsafe { eccodes_sys::codes_grib_multi_handle_delete(self.handle); }
        }
    }
}

impl Default for MultiHandle {
    /// Create a new empty multi-field handle.
    ///
    /// # Panics
    /// Panics if the C library fails to allocate the handle.
    fn default() -> Self {
        Self::new().expect("failed to create MultiHandle")
    }
}
