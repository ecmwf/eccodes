//! Listing the keys of a message — [`Message::keys`](crate::Message::keys).
//!
//! [`KeysQuery`] is a description of *which* keys, narrowed before you iterate:
//!
//! ```no_run
//! # fn main() -> eccodes::Result<()> {
//! # let message: eccodes::Message = unimplemented!();
//! for key in message.keys().namespace("ls") {
//!     let key = key?;
//!     let value: String = message.get(&key)?;
//!     println!("{key} = {value}");
//! }
//!
//! // only what the message actually codes, each name once
//! for key in message.keys().coded_only().skip_duplicates() {
//!     println!("{}", key?);
//! }
//! # Ok(())
//! # }
//! ```

use std::ffi::{CStr, c_ulong};
use std::iter::FusedIterator;
use std::marker::PhantomData;
use std::ptr::NonNull;

use eccodes_sys as sys;

use crate::error::{Code, Error, Result, check};
use crate::ffi;

bitflags::bitflags! {
    /// Which keys a [`KeysQuery`] visits (`CODES_KEYS_ITERATOR_*`).
    ///
    /// The named methods on [`KeysQuery`] cover the common combinations;
    /// reach for these when you need an exact set.
    #[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
    pub struct KeyFlags: u32 {
        /// Every key.
        const ALL_KEYS = sys::CODES_KEYS_ITERATOR_ALL_KEYS;
        /// Skip keys that cannot be written.
        const SKIP_READ_ONLY = sys::CODES_KEYS_ITERATOR_SKIP_READ_ONLY;
        /// Skip optional keys.
        const SKIP_OPTIONAL = sys::CODES_KEYS_ITERATOR_SKIP_OPTIONAL;
        /// Skip keys specific to one edition.
        const SKIP_EDITION_SPECIFIC = sys::CODES_KEYS_ITERATOR_SKIP_EDITION_SPECIFIC;
        /// Skip keys coded in the message.
        const SKIP_CODED = sys::CODES_KEYS_ITERATOR_SKIP_CODED;
        /// Skip keys computed by the definitions.
        const SKIP_COMPUTED = sys::CODES_KEYS_ITERATOR_SKIP_COMPUTED;
        /// Yield each name once.
        const SKIP_DUPLICATES = sys::CODES_KEYS_ITERATOR_SKIP_DUPLICATES;
        /// Skip function keys.
        const SKIP_FUNCTION = sys::CODES_KEYS_ITERATOR_SKIP_FUNCTION;
        /// Only the keys a dump would print.
        const DUMP_ONLY = sys::CODES_KEYS_ITERATOR_DUMP_ONLY;
    }
}

/// Which keys of a message to list — build it with
/// [`Message::keys`](crate::Message::keys), then iterate.
///
/// Iterating yields key *names*; read values through
/// [`Message::get`](crate::Message::get).
#[derive(Debug, Clone)]
pub struct KeysQuery<'m> {
    handle: *mut sys::codes_handle,
    flags: KeyFlags,
    namespace: Option<String>,
    _message: PhantomData<&'m ()>,
}

impl<'m> KeysQuery<'m> {
    pub(crate) const fn new(handle: *mut sys::codes_handle) -> Self {
        Self {
            handle,
            flags: KeyFlags::ALL_KEYS,
            namespace: None,
            _message: PhantomData,
        }
    }

    /// Restrict to one namespace, e.g. `"ls"`, `"mars"` or `"time"`.
    #[must_use]
    pub fn namespace(mut self, namespace: impl Into<String>) -> Self {
        self.namespace = Some(namespace.into());
        self
    }

    /// Set the filter flags outright, replacing any set so far.
    #[must_use]
    pub const fn flags(mut self, flags: KeyFlags) -> Self {
        self.flags = flags;
        self
    }

    /// Only keys the message codes — the ones that occupy bytes.
    #[must_use]
    pub fn coded_only(self) -> Self {
        self.with(KeyFlags::SKIP_COMPUTED)
    }

    /// Only keys the definitions compute, such as `shortName`.
    #[must_use]
    pub fn computed_only(self) -> Self {
        self.with(KeyFlags::SKIP_CODED)
    }

    /// Only keys that can be written.
    #[must_use]
    pub fn writable_only(self) -> Self {
        self.with(KeyFlags::SKIP_READ_ONLY)
    }

    /// Yield each name once, however many accessors carry it.
    #[must_use]
    pub fn skip_duplicates(self) -> Self {
        self.with(KeyFlags::SKIP_DUPLICATES)
    }

    fn with(mut self, flags: KeyFlags) -> Self {
        self.flags |= flags;
        self
    }
}

impl<'m> IntoIterator for KeysQuery<'m> {
    type Item = Result<String>;
    type IntoIter = Keys<'m>;

    fn into_iter(self) -> Keys<'m> {
        Keys {
            state: State::Pending(self),
            _message: PhantomData,
        }
    }
}

/// The keys of a message, one name at a time — see [`KeysQuery`].
pub struct Keys<'m> {
    state: State<'m>,
    _message: PhantomData<&'m ()>,
}

enum State<'m> {
    /// The C iterator is built on first use, so `into_iter` need not fail.
    Pending(KeysQuery<'m>),
    Running(NonNull<sys::codes_keys_iterator>),
    Done,
}

impl<'m> Keys<'m> {
    fn start(query: &KeysQuery<'m>) -> Result<NonNull<sys::codes_keys_iterator>> {
        let namespace = query.namespace.as_deref().map(ffi::cstring).transpose()?;
        // SAFETY: valid handle; a NULL namespace means "all of them", and a
        // non-NULL one is NUL-terminated and copied by the library.
        let raw = unsafe {
            sys::codes_keys_iterator_new(
                query.handle,
                c_ulong::from(query.flags.bits()),
                namespace
                    .as_ref()
                    .map_or(std::ptr::null(), |namespace| namespace.as_ptr()),
            )
        };
        NonNull::new(raw).ok_or_else(|| Error::from(Code::InvalidKeysIterator))
    }

    /// Release the C iterator, whether it ran out or is being dropped early.
    fn finish(&mut self) {
        if let State::Running(iterator) = std::mem::replace(&mut self.state, State::Done) {
            // SAFETY: a valid iterator owned by us, freed exactly once — the
            // state is `Done` from here on, so no path reaches it again.
            unsafe { sys::codes_keys_iterator_delete(iterator.as_ptr()) };
        }
    }

    fn next_name(iterator: NonNull<sys::codes_keys_iterator>) -> Option<Result<String>> {
        // SAFETY: valid iterator; returns non-zero while a key remains.
        if unsafe { sys::codes_keys_iterator_next(iterator.as_ptr()) } == 0 {
            return None;
        }
        // SAFETY: after a successful step the name belongs to the iterator and
        // stays valid until it advances again.
        let name = unsafe { sys::codes_keys_iterator_get_name(iterator.as_ptr()) };
        if name.is_null() {
            return Some(Err(Error::from(Code::NullPointer)));
        }
        // SAFETY: non-null and NUL-terminated, as above.
        Some(match unsafe { CStr::from_ptr(name) }.to_str() {
            Ok(name) => Ok(name.to_owned()),
            Err(err) => Err(Error::from(err)),
        })
    }
}

impl Iterator for Keys<'_> {
    type Item = Result<String>;

    fn next(&mut self) -> Option<Self::Item> {
        let iterator = match &self.state {
            State::Done => return None,
            State::Running(iterator) => *iterator,
            State::Pending(query) => match Self::start(query) {
                Ok(iterator) => {
                    self.state = State::Running(iterator);
                    iterator
                }
                Err(err) => {
                    self.state = State::Done;
                    return Some(Err(err));
                }
            },
        };

        let name = Self::next_name(iterator);
        if name.is_none() {
            self.finish();
        }
        name
    }
}

impl FusedIterator for Keys<'_> {}

impl Drop for Keys<'_> {
    fn drop(&mut self) {
        self.finish();
    }
}
