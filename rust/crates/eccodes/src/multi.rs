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
//! Reading one back field by field is asked of the reader — see
//! [`MessageFile::multi_field`](crate::MessageFile::multi_field) — and
//! affects nothing else in the process:
//!
//! ```no_run
//! # fn main() -> eccodes::Result<()> {
//! let fields = eccodes::GribFile::open("combined.grib2")?.multi_field(true);
//! println!("{} fields", fields.count()?);
//! # Ok(())
//! # }
//! ```

use std::ffi::c_int;
use std::fmt;
use std::io::Write;
use std::ptr::{self, NonNull};
use std::sync::{PoisonError, RwLock};

use eccodes_sys as sys;

use crate::error::{Code, Error, Result, check};
use crate::ffi;
use crate::message::GribMessage;

/// Serializes the C library's multi-field switch against the calls that read
/// it.
///
/// The switch is a flag on the library's global context, and it is read only
/// while a handle is being created: `grib_new_from_file`,
/// `grib_handle_new_from_multi_message` and the two counters consult it, and
/// nothing consults it afterwards. So it need not be process-wide state. A
/// read that wants fields turns it on for the length of one C call while
/// holding this lock exclusively; every other call that would consult it
/// holds the lock shared, and so is guaranteed the switch is off.
///
/// Without that, one file asked for fields would decide how every other
/// thread in the process reads and counts — which is what the C API's global
/// switch does, and what [`reading`] exists to contain.
static SWITCH: RwLock<()> = RwLock::new(());

/// Turns the switch back off when it goes out of scope — including when the
/// call it wraps panics.
struct SwitchedOn;

impl Drop for SwitchedOn {
    fn drop(&mut self) {
        // SAFETY: a NULL context selects the default one.
        unsafe { sys::codes_grib_multi_support_off(ptr::null_mut()) };
    }
}

/// Make a C call that reads the multi-field switch, with the switch set to
/// `fields`.
///
/// Every call in the crate that consults the switch goes through here, so it
/// is never on outside one of them.
pub fn reading<T>(fields: bool, call: impl FnOnce() -> T) -> T {
    if !fields {
        let _shared = SWITCH.read().unwrap_or_else(PoisonError::into_inner);
        return call();
    }
    let _exclusive = SWITCH.write().unwrap_or_else(PoisonError::into_inner);
    // SAFETY: a NULL context selects the default one.
    unsafe { sys::codes_grib_multi_support_on(ptr::null_mut()) };
    let _off = SwitchedOn;
    call()
}

/// Make a C call that turns the switch on for itself, and turn it back off.
fn restoring<T>(call: impl FnOnce() -> T) -> T {
    let _exclusive = SWITCH.write().unwrap_or_else(PoisonError::into_inner);
    let _off = SwitchedOn;
    call()
}

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
    pub fn new() -> Result<Self> {
        // `grib_multi_handle_new` turns the C library's multi-field switch on
        // for itself, although nothing on the write path reads it: appending
        // and writing never consult it. Left on, it would change how every
        // reader in the process decodes and counts, so `restoring` puts it
        // back.
        // SAFETY: a NULL context selects the default one.
        let raw = restoring(|| unsafe { sys::codes_grib_multi_handle_new(ptr::null_mut()) });
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

#[cfg(test)]
mod tests {
    use std::path::{Path, PathBuf};

    use super::*;
    use crate::file::GribFile;

    /// The in-repo sample, or `None` when not building from the repo
    /// checkout — the test skips then, as the integration tests do.
    fn sample() -> Option<PathBuf> {
        let path = Path::new(env!("CARGO_MANIFEST_DIR")).join("../../../samples/GRIB2.tmpl");
        path.exists().then_some(path)
    }

    #[test]
    fn a_panicking_read_still_switches_it_back() {
        let Some(path) = sample() else {
            return;
        };

        // Nothing in the crate unwinds out of a C call today, but the switch
        // being on is exactly the state that must not outlive the call that
        // wanted it: were it left on, every count below — in any thread, for
        // the rest of the process — would fail with `NotImplemented`.
        let hook = std::panic::take_hook();
        std::panic::set_hook(Box::new(|_| {}));
        let panicked = std::panic::catch_unwind(|| reading(true, || panic!("a read gave up")));
        std::panic::set_hook(hook);
        assert!(panicked.is_err(), "the panic is what is being recovered");

        // The panic also poisoned the lock, which must not stop the next
        // reader either.
        let file = GribFile::open(&path).expect("the sample opens");
        assert_eq!(file.count().expect("counting after the panic"), 1);
    }
}
