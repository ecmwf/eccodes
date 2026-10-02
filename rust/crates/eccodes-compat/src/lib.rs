//! Compatibility layer exposing the `ScaleWeather` `eccodes` 0.15 API on top of
//! the official [`eccodes`] crate.
//!
//! For existing users of the unofficial bindings: depend on this crate under
//! the old name and keep your code unchanged.
//!
//! ```toml
//! [dependencies]
//! eccodes = { package = "eccodes-compat", version = "0.1" }
//! ```
//!
//! New code should use the official [`eccodes`] crate directly; this layer
//! exists to make the transition gradual, and its API stays frozen at the
//! 0.15 surface. The [`migration`] module maps every 0.15 call to its
//! official equivalent.

/// How to move from these bindings to the official `eccodes` crate.
#[doc = include_str!("../MIGRATION.md")]
#[cfg(doc)]
pub mod migration {}

pub mod codes_file;
pub mod codes_message;
pub mod codes_nearest;
pub mod errors;
pub mod keys_iterator;

pub use codes_file::{ArcMessageIter, CodesFile, ProductKind, RefMessageIter};
pub use codes_message::{ArcMessage, BufMessage, DynamicKeyType, KeyRead, KeyWrite, RefMessage};
pub use codes_nearest::{CodesNearest, NearestGridpoint};
pub use errors::CodesError;
pub use fallible_iterator::{FallibleIterator, IntoFallibleIterator};
pub use keys_iterator::{KeysIterator, KeysIteratorFlags};
