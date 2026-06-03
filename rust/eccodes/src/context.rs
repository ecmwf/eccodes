//! Context — global ecCodes configuration.
//!
//! Requires `features = ["advanced"]`.

use std::ffi::CString;

/// The ecCodes context, managing global configuration.
///
/// The default context is obtained via [`Context::get_default()`].
/// Note: the default context is a singleton and should NOT be deleted.
pub struct Context {
    ctx: *mut eccodes_sys::codes_context,
    owned: bool,
}

unsafe impl Send for Context {}

impl Context {
    /// Get the default context (singleton, not freed on drop).
    pub fn get_default() -> Self {
        let ctx = unsafe { eccodes_sys::codes_context_get_default() };
        Context { ctx, owned: false }
    }

    /// Set the search path for definition files.
    pub fn set_definitions_path(&mut self, path: &str) {
        let c_path = CString::new(path).expect("Invalid path");
        unsafe {
            eccodes_sys::codes_context_set_definitions_path(self.ctx, c_path.as_ptr());
        }
    }

    /// Set the search path for sample files.
    pub fn set_samples_path(&mut self, path: &str) {
        let c_path = CString::new(path).expect("Invalid path");
        unsafe {
            eccodes_sys::codes_context_set_samples_path(self.ctx, c_path.as_ptr());
        }
    }

    /// Set debug mode.
    pub fn set_debug(&mut self, mode: i32) {
        unsafe {
            eccodes_sys::codes_context_set_debug(self.ctx, mode);
        }
    }

    /// Turn on multi-field support.
    pub fn multi_support_on(&mut self) {
        unsafe {
            eccodes_sys::codes_grib_multi_support_on(self.ctx);
        }
    }

    /// Turn off multi-field support.
    pub fn multi_support_off(&mut self) {
        unsafe {
            eccodes_sys::codes_grib_multi_support_off(self.ctx);
        }
    }
}

impl Drop for Context {
    fn drop(&mut self) {
        // Only delete if we own it (not the default singleton)
        if self.owned && !self.ctx.is_null() {
            unsafe {
                eccodes_sys::codes_context_delete(self.ctx);
            }
        }
    }
}
