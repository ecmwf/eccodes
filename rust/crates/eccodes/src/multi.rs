//! Multi-field GRIB messages — `Handle<GribMulti>`.

use std::ffi::c_int;
use std::io::Write;
use std::ptr::{self, NonNull};

use eccodes_sys as sys;

use crate::error::{Error, Result, check};
use crate::handle::{Handle, with_memstream};
use crate::kind::{Grib, GribMulti};

impl Handle<GribMulti> {
    /// Create an empty multi-field GRIB message
    /// (`codes_grib_multi_handle_new`).
    ///
    /// Side effect inherited from the C library: this enables multi-field
    /// support on the process-global context, as if
    /// [`Context::grib_multi_support(true)`](crate::Context::grib_multi_support)
    /// had been called — subsequent file reads split multi-field messages
    /// into one handle per field until it is turned off again.
    pub fn new() -> Result<Self> {
        // SAFETY: NULL context selects the default context.
        let raw = unsafe { sys::codes_grib_multi_handle_new(ptr::null_mut()) };
        NonNull::new(raw)
            .map(Self::from_raw)
            .ok_or(Error::NullHandle)
    }

    /// Append the sections of `src` from `start_section` onwards
    /// (`codes_grib_multi_handle_append`).
    pub fn append(&mut self, src: &Handle<Grib>, start_section: u32) -> Result<()> {
        let start_section = c_int::try_from(start_section).map_err(|_| Error::InvalidArgument)?;
        check!(sys::codes_grib_multi_handle_append(
            src.as_sys(),
            start_section,
            self.as_sys(),
        ))
    }

    /// Write the raw multi-field message to `w`
    /// (`codes_grib_multi_handle_write`).
    pub fn write_to(&self, w: &mut impl Write) -> Result<()> {
        with_memstream(w, |file| {
            check!(sys::codes_grib_multi_handle_write(self.as_sys(), file))
        })
    }
}
