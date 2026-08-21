//! [`Library`] — the eccodes C library itself: which build is loaded, where
//! it looks for its definitions, and the tables it can compute.
//!
//! ```no_run
//! use eccodes::{Library, Version};
//!
//! # fn main() -> eccodes::Result<()> {
//! println!("eccodes {}", Library::version());
//! if Library::version() < Version::new(2, 49, 0) {
//!     println!("older than this crate was written against");
//! }
//! Library::set_definitions_path("/usr/share/eccodes/definitions")?;
//! # Ok(())
//! # }
//! ```
//!
//! Everything here is process-wide: the C library keeps one context, and
//! these calls configure it for the whole program.

use std::ffi::{CStr, c_char, c_int};
use std::fmt;
use std::panic::{self, AssertUnwindSafe};
use std::path::{Path, PathBuf};
use std::ptr;
use std::sync::{Arc, PoisonError, RwLock};

use eccodes_sys as sys;

use crate::error::{Code, Error, Result, check};
use crate::ffi;

/// A released version of the eccodes C library.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Version {
    /// Major version.
    pub major: u32,
    /// Minor version.
    pub minor: u32,
    /// Patch version.
    pub patch: u32,
}

impl Version {
    /// A version from its three parts.
    #[must_use]
    pub const fn new(major: u32, minor: u32, patch: u32) -> Self {
        Self {
            major,
            minor,
            patch,
        }
    }

    /// Unpack the library's `MMmmpp` encoding, e.g. `24900` for 2.49.0.
    fn from_packed(packed: i64) -> Self {
        let part = |value: i64| u32::try_from(value).unwrap_or(0);
        Self {
            major: part(packed / 10_000),
            minor: part((packed / 100) % 100),
            patch: part(packed % 100),
        }
    }
}

impl fmt::Display for Version {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}.{}.{}", self.major, self.minor, self.patch)
    }
}

/// How urgent a diagnostic from the C library is (`CODES_LOG_*`).
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[non_exhaustive]
pub enum LogLevel {
    /// What the library is doing internally — only emitted with
    /// [`Library::set_debug`] on.
    Debug,
    /// A decision the library made that a caller might want to know about.
    Info,
    /// Something the caller probably did not intend, that was not fatal.
    Warning,
    /// A failure — usually, but not always, also returned as an [`Error`].
    Error,
    /// A failure the C library aborts the process on.
    Fatal,
}

impl LogLevel {
    /// The variant for a raw `CODES_LOG_*` value.
    #[allow(clippy::cast_sign_loss)] // a negative level simply matches nothing
    const fn from_raw(raw: c_int) -> Option<Self> {
        match raw as u32 {
            sys::CODES_LOG_DEBUG => Some(Self::Debug),
            sys::CODES_LOG_INFO => Some(Self::Info),
            sys::CODES_LOG_WARNING => Some(Self::Warning),
            sys::CODES_LOG_ERROR => Some(Self::Error),
            sys::CODES_LOG_FATAL => Some(Self::Fatal),
            _ => None,
        }
    }
}

/// Where the C library's own diagnostics go — see
/// [`Library::set_logging`].
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
#[non_exhaustive]
pub enum Logging {
    /// Straight to stderr, as the C library does when left alone.
    Stderr,
    /// Through the [`log`] crate, under the `eccodes` target: the application
    /// picks the backend (`env_logger`, `tracing-subscriber`, …) and gets
    /// nothing at all until it installs one.
    Rust,
    /// Dropped.
    Silent,
}

/// The `log` target the [`Logging::Rust`] bridge reports under.
const LOG_TARGET: &str = "eccodes";

/// What [`Library::set_log_handler`] installed, if anything. `None` while the
/// trampoline is the library's log procedure is [`Logging::Silent`].
type LogHandler = Arc<dyn Fn(LogLevel, &str) + Send + Sync>;

static HANDLER: RwLock<Option<LogHandler>> = RwLock::new(None);

/// The `codes_log_proc` given to the C library.
///
/// It holds the lock only long enough to take a reference to the handler, so
/// a handler that calls back into eccodes cannot deadlock against it.
unsafe extern "C" fn log_trampoline(
    _context: *const sys::codes_context,
    level: c_int,
    message: *const c_char,
) {
    let handler = HANDLER
        .read()
        .unwrap_or_else(PoisonError::into_inner)
        .clone();
    let Some(handler) = handler else {
        return;
    };
    if message.is_null() {
        return;
    }
    // SAFETY: the library passes a NUL-terminated string that outlives this
    // call, and no ownership of it.
    let message = unsafe { CStr::from_ptr(message) }.to_string_lossy();
    // Unwinding from here would cross back into C. A handler that panics
    // loses its message; it does not take the process with it.
    let _ = panic::catch_unwind(AssertUnwindSafe(|| {
        // A level this crate does not know is reported, not dropped.
        let level = LogLevel::from_raw(level).unwrap_or(LogLevel::Error);
        handler(level, &message);
    }));
}

/// The eccodes C library loaded into this process.
///
/// A handle to something global rather than something owned — hence
/// associated functions and no constructor.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Library;

impl Library {
    /// The library's version.
    #[must_use]
    pub fn version() -> Version {
        // SAFETY: takes no arguments and returns a packed integer.
        Version::from_packed(unsafe { sys::codes_get_api_version() })
    }

    /// The commit the library was built from.
    #[must_use]
    pub fn git_sha1() -> &'static str {
        // SAFETY: returns a static NUL-terminated string, never NULL.
        unsafe { ffi::static_str(sys::codes_get_git_sha1()) }.unwrap_or_default()
    }

    /// When the library was built.
    #[must_use]
    pub fn build_date() -> &'static str {
        // SAFETY: returns a static NUL-terminated string, never NULL.
        unsafe { ffi::static_str(sys::codes_get_build_date()) }.unwrap_or_default()
    }

    /// Where the library looks for sample messages — the names
    /// [`Message::from_sample`](crate::Message::from_sample) accepts.
    pub fn samples_path() -> Result<PathBuf> {
        // SAFETY: a NULL context selects the default one; the returned string
        // is owned by the library and outlives the call.
        let path = unsafe { ffi::static_str(sys::codes_samples_path(ptr::null())) };
        path.map(PathBuf::from)
            .ok_or_else(|| Error::from(Code::NullPointer))
    }

    /// Look for sample messages in `path` instead.
    pub fn set_samples_path(path: impl AsRef<Path>) -> Result<()> {
        let path = path.as_ref();
        let cpath = ffi::cpath(path)?;
        // SAFETY: a NULL context selects the default one; `cpath` is
        // NUL-terminated and copied by the library.
        unsafe { sys::codes_context_set_samples_path(ptr::null_mut(), cpath.as_ptr()) };
        Ok(())
    }

    /// Look for key definitions in `path` instead.
    pub fn set_definitions_path(path: impl AsRef<Path>) -> Result<()> {
        let path = path.as_ref();
        let cpath = ffi::cpath(path)?;
        // SAFETY: as for `set_samples_path`.
        unsafe { sys::codes_context_set_definitions_path(ptr::null_mut(), cpath.as_ptr()) };
        Ok(())
    }

    /// Print the library's debug output to stderr.
    pub fn set_debug(enabled: bool) {
        // SAFETY: a NULL context selects the default one.
        unsafe { sys::codes_context_set_debug(ptr::null_mut(), i32::from(enabled)) };
    }

    /// Send the C library's own diagnostics somewhere in particular.
    ///
    /// eccodes writes them itself — `ECCODES ERROR   :  ...` on stderr — and
    /// does so for conditions its own callers treat as ordinary: counting a
    /// product a file does not hold logs an error, then reports zero. Left
    /// alone, that text goes to the stderr of whatever program links this
    /// crate, whether or not the call it came from failed.
    ///
    /// ```no_run
    /// use eccodes::{Library, Logging};
    ///
    /// // Under the application's logger, like every other library it uses.
    /// Library::set_logging(Logging::Rust);
    /// ```
    ///
    /// Process-wide, and best set before other threads are reading: it
    /// replaces one function pointer on the library's global context.
    pub fn set_logging(destination: Logging) {
        match destination {
            Logging::Stderr => Self::install_log(None, None),
            Logging::Silent => Self::install_log(None, Some(log_trampoline)),
            Logging::Rust => Self::set_log_handler(|level, message| match level {
                LogLevel::Error | LogLevel::Fatal => log::error!(target: LOG_TARGET, "{message}"),
                LogLevel::Warning => log::warn!(target: LOG_TARGET, "{message}"),
                LogLevel::Info => log::info!(target: LOG_TARGET, "{message}"),
                LogLevel::Debug => log::debug!(target: LOG_TARGET, "{message}"),
            }),
        }
    }

    /// Send the C library's own diagnostics to `handler`.
    ///
    /// For anything [`Logging`] does not cover — counting them, failing a
    /// test on them, routing them per level. The handler is called on
    /// whichever thread the library logged from, so it must be ready for
    /// several at once.
    ///
    /// ```no_run
    /// # fn main() -> eccodes::Result<()> {
    /// eccodes::Library::set_log_handler(|level, message| {
    ///     eprintln!("eccodes {level:?}: {message}");
    /// });
    /// # Ok(())
    /// # }
    /// ```
    pub fn set_log_handler<F>(handler: F)
    where
        F: Fn(LogLevel, &str) + Send + Sync + 'static,
    {
        Self::install_log(Some(Arc::new(handler)), Some(log_trampoline));
    }

    /// Record `handler` and point the C library at `procedure` — NULL for its
    /// own, which is what restores stderr.
    fn install_log(handler: Option<LogHandler>, procedure: sys::codes_log_proc) {
        *HANDLER.write().unwrap_or_else(PoisonError::into_inner) = handler;
        // SAFETY: a NULL context selects the default one; a NULL procedure
        // puts the library's own back (`grib_context_set_logging_proc`).
        unsafe { sys::codes_context_set_logging_proc(ptr::null_mut(), procedure) };
    }

    /// The `2 * n` Gaussian latitudes of a global Gaussian grid of order `n`,
    /// from north to south.
    ///
    /// `n` is the Gaussian number — 640 for O640, and so on.
    pub fn gaussian_latitudes(n: usize) -> Result<Vec<f64>> {
        let mut latitudes = vec![0.0_f64; n.saturating_mul(2)];
        check!(sys::codes_get_gaussian_latitudes(
            ffi::to_c_long(n)?,
            latitudes.as_mut_ptr()
        ))?;
        Ok(latitudes)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn version_unpacks_the_librarys_encoding() {
        // The C API reports 2.49.0 as the integer 24900; the point of `Version`
        // is that callers never see that.
        assert_eq!(Version::from_packed(24_900), Version::new(2, 49, 0));
        assert_eq!(Version::from_packed(21_007), Version::new(2, 10, 7));
        assert_eq!(Version::from_packed(0), Version::new(0, 0, 0));
    }

    #[test]
    fn versions_order_by_significance() {
        // Ordering is what callers actually do with a version.
        assert!(Version::new(2, 49, 0) > Version::new(2, 9, 9));
        assert!(Version::new(2, 49, 0) < Version::new(10, 0, 0));
    }

    #[test]
    fn version_displays_as_three_parts() {
        assert_eq!(Version::new(2, 49, 0).to_string(), "2.49.0");
    }
}
