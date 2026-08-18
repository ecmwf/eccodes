//! Multi-field GRIB messages — several fields written as one message.
//!
//! ```no_run
//! use eccodes::{GribFile, GribMultiField};
//!
//! # fn main() -> eccodes::Result<()> {
//! let mut multi = GribMultiField::new()?;
//! for message in &GribFile::open("fields.grib2")? {
//!     multi.push(&message?)?;
//! }
//! multi.write_to(std::fs::File::create("combined.grib2")?)?;
//! # Ok(())
//! # }
//! ```
//!
//! # Process-wide effect
//!
//! Creating a `GribMultiField` switches on the C library's global multi-field
//! support, exactly as
//! [`Library::set_grib_multi_support(true)`](crate::Library::set_grib_multi_support)
//! would, and nothing switches it back. While it is on:
//!
//! - reading a GRIB file yields one message per *field* rather than per
//!   message, and
//! - [`MessageFile::<Grib>::count`](crate::MessageFile::count) fails with
//!   [`Code::NotImplemented`](crate::Code::NotImplemented), because the C
//!   counter refuses to count multi-field GRIBs.
//!
//! Turn it off with `Library::set_grib_multi_support(false)` once the
//! multi-field message has been written.

use std::ffi::c_int;
use std::fmt;
use std::io::Write;
use std::ptr::{self, NonNull};

use eccodes_sys as sys;

use crate::error::{Code, Error, Result, check};
use crate::ffi;
use crate::message::GribMessage;

/// A multi-field GRIB message under construction.
///
/// Fields are appended one message at a time and written out together.
pub struct GribMultiField {
    raw: NonNull<sys::codes_multi_handle>,
}

/// The section a field's own data starts at: GRIB2's Product Definition
/// Section, which is where `grib_multi_write` and the C examples start.
const DEFAULT_START_SECTION: u8 = 4;

// SAFETY: the multi-field message owns its C object exclusively and may move
// between threads. Not `Sync`: appending mutates it.
unsafe impl Send for GribMultiField {}

impl GribMultiField {
    /// Start an empty multi-field message.
    ///
    /// This switches the C library's multi-field support on process-wide —
    /// see [`Library::set_grib_multi_support`](crate::Library::set_grib_multi_support)
    /// and the notes on [`GribMultiField`] for what that changes.
    pub fn new() -> Result<Self> {
        // SAFETY: a NULL context selects the default one.
        let raw = unsafe { sys::codes_grib_multi_handle_new(ptr::null_mut()) };
        NonNull::new(raw)
            .map(|raw| Self { raw })
            .ok_or_else(|| Error::from(Code::NullHandle))
    }

    /// Append a field, taking its sections from the Product Definition
    /// Section onwards and inheriting the earlier sections already in the
    /// message.
    pub fn push(&mut self, message: &GribMessage) -> Result<()> {
        self.push_from_section(message, DEFAULT_START_SECTION)
    }

    /// Append a field, taking its sections from `section` onwards.
    ///
    /// Sections before `section` come from what is already in the multi-field
    /// message, so a lower number repeats more of the field's own metadata.
    pub fn push_from_section(&mut self, message: &GribMessage, section: u8) -> Result<()> {
        check!(sys::codes_grib_multi_handle_append(
            message.as_ptr(),
            c_int::from(section),
            self.raw.as_ptr(),
        ))
    }

    /// Write the multi-field message to `out`.
    pub fn write_to(&self, out: impl Write) -> Result<()> {
        ffi::with_memstream(out, |stream| {
            check!(sys::codes_grib_multi_handle_write(
                self.raw.as_ptr(),
                stream
            ))
        })
    }

    /// The multi-field message's bytes.
    pub fn to_vec(&self) -> Result<Vec<u8>> {
        let mut bytes = Vec::new();
        self.write_to(&mut bytes)?;
        Ok(bytes)
    }
}

impl Drop for GribMultiField {
    fn drop(&mut self) {
        // SAFETY: a valid multi-field handle owned by us, freed exactly once.
        unsafe { sys::codes_grib_multi_handle_delete(self.raw.as_ptr()) };
    }
}

impl fmt::Debug for GribMultiField {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("GribMultiField").finish_non_exhaustive()
    }
}
