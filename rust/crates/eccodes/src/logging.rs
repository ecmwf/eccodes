//! Where the C library's own diagnostics go.
//!
//! eccodes narrates its work itself — `ECCODES ERROR   :  ...` on stderr —
//! and does so for conditions its own callers treat as ordinary: counting a
//! product a file does not hold logs an error, then reports zero. Left alone,
//! that text lands on the stderr of whatever program links this crate,
//! whether or not the call it came from failed.
//!
//! ```no_run
//! use eccodes::{Library, Logging};
//!
//! // Under the application's logger, like every other library it uses.
//! Library::set_logging(Logging::Rust);
//! ```
//!
//! Process-wide, like everything else on [`Library`]: it replaces one
//! function pointer on the C library's global context.

use std::ffi::{CStr, c_char, c_int};
use std::panic::{self, AssertUnwindSafe};
use std::ptr;
use std::sync::{Arc, PoisonError, RwLock};

use eccodes_sys as sys;

use crate::library::Library;

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
    /// A failure — usually, but not always, also returned as an
    /// [`Error`](crate::Error).
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

/// Where the C library's own diagnostics go — see [`Library::set_logging`].
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
unsafe extern "C" fn trampoline(
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

/// Record `handler` and point the C library at `procedure` — NULL for its
/// own, which is what restores stderr.
fn install(handler: Option<LogHandler>, procedure: sys::codes_log_proc) {
    *HANDLER.write().unwrap_or_else(PoisonError::into_inner) = handler;
    // SAFETY: a NULL context selects the default one; a NULL procedure puts
    // the library's own back (`grib_context_set_logging_proc`).
    unsafe { sys::codes_context_set_logging_proc(ptr::null_mut(), procedure) };
}

impl Library {
    /// Send the C library's own diagnostics somewhere in particular.
    ///
    /// See the [module documentation](self) for what it is that gets sent.
    ///
    /// ```no_run
    /// use eccodes::{Library, Logging};
    ///
    /// Library::set_logging(Logging::Rust);
    /// ```
    ///
    /// Process-wide, and best set before other threads start reading.
    pub fn set_logging(destination: Logging) {
        match destination {
            Logging::Stderr => install(None, None),
            Logging::Silent => install(None, Some(trampoline)),
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
    /// eccodes::Library::set_log_handler(|level, message| {
    ///     eprintln!("eccodes {level:?}: {message}");
    /// });
    /// ```
    pub fn set_log_handler<F>(handler: F)
    where
        F: Fn(LogLevel, &str) + Send + Sync + 'static,
    {
        install(Some(Arc::new(handler)), Some(trampoline));
    }
}
