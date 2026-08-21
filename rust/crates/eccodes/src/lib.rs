//! Read and write GRIB and BUFR messages, through ECMWF's eccodes library.
//!
//! ```no_run
//! use eccodes::GribFile;
//!
//! # fn main() -> eccodes::Result<()> {
//! let file = GribFile::open("data.grib2")?;
//! println!("{} messages", file.count()?);
//!
//! for message in &file {
//!     let message = message?;
//!     let name: String = message.get("shortName")?;
//!     let values = message.values()?;
//!     println!("{name}: {} points", values.len());
//! }
//! # Ok(())
//! # }
//! ```
//!
//! # The shape of the API
//!
//! - A [`MessageFile`] is a file of messages: [`count`](MessageFile::count)
//!   it, [`index`](MessageFile::index) it, or iterate it. [`GribFile`] and
//!   [`BufrFile`] fix the product; plain `MessageFile` accepts any.
//! - A [`Message`] is one message. Keys are read and written by name, typed
//!   by what you ask for — see [`get`](Message::get) and [`set`](Message::set).
//!   [`GribMessage`] adds grids and nearest-point search; [`BufrMessage`]
//!   adds [`unpack`](BufrMessage::unpack).
//! - Products are types, not arguments: [`kind`] holds the markers, and
//!   [`Kind`] reports what a message actually is.
//! - [`Library`] is the C library itself — version, search paths, debug
//!   output.
//!
//! # Missing values
//!
//! Ask for an [`Option`] and a key that is absent, or coded as missing, is
//! `None`:
//!
//! ```no_run
//! # fn main() -> eccodes::Result<()> {
//! # let message: eccodes::Message = unimplemented!();
//! let level: Option<i64> = message.get("levelist")?;
//! # Ok(())
//! # }
//! ```
//!
//! Inside a `values` array, individual missing points are coded with a bitmap
//! and a sentinel instead — see [`missing`].
//!
//! # Errors
//!
//! [`Error`] says what failed and what it was working on:
//! `eccodes::NotFound: Key/value not found (key "shortNam")`. Match on
//! [`Error::code`], which context never changes.
//!
//! The C library also writes diagnostics of its own, to stderr, for failures
//! this crate handles and for some it does not report at all — see
//! [`logging`] to put them under the application's logger.
//!
//! # Threads
//!
//! Messages, files and indexes are [`Send`] but not [`Sync`]: the C library
//! caches decoded state inside a message even when reading, so two threads
//! must not share one. Move a message to a thread, or read it there from its
//! own file.
//!
//! # Features
//!
//! - `vendored` (default) — build and link eccodes from source.
//! - `system` — link an installed eccodes.
//! - `raw` — expose `Message::as_raw` and re-export `eccodes_sys` as `sys`,
//!   for handing a message to another library built on eccodes.

#![deny(missing_docs)]

mod datetime;
mod error;
mod ffi;
mod file;
mod grid;
mod index;
mod key;
mod keys;
pub mod kind;
mod library;
pub mod logging;
mod message;
pub mod missing;
mod multi;
mod nearest;

pub use datetime::JulianDay;
pub use error::{Code, Error, Result};
pub use file::{AnyFile, BufrFile, GribFile, MessageFile, Messages};
pub use grid::{GeoPoint, GridPoints, GridPositions, LatLon};
pub use index::{Index, IndexMessages, IndexSelect, IndexValue};
pub use key::{KeyElement, KeyForce, KeyGet, KeySet, KeyType};
pub use keys::{KeyFlags, Keys, KeysQuery};
pub use kind::{Kind, MessageKind};
pub use library::{Library, Version};
pub use logging::{LogLevel, Logging};
pub use message::{BufrMessage, GribMessage, Message, WrongKind};
pub use multi::GribMultiField;
pub use nearest::{Nearest, NearestPoint, Reuse};

/// Dates and times in this crate's API are [`time`] types — re-exported so
/// callers need not track the version themselves.
pub use time;

/// The raw FFI bindings, for handing a [`Message`] to another library built
/// on eccodes. See [`Message::as_raw`].
#[cfg(feature = "raw")]
pub use eccodes_sys as sys;
