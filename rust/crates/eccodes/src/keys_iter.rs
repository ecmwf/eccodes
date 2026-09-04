//! `KeysIterator` — iteration over the key names of a message.

use std::ffi::{CStr, c_ulong};
use std::marker::PhantomData;
use std::ptr::NonNull;

use eccodes_sys as sys;

use crate::error::{Error, Result, check};
use crate::handle::ckey;

bitflags::bitflags! {
    /// Filter flags for [`KeysIterator`] (`CODES_KEYS_ITERATOR_*`).
    ///
    /// Combine with `|`, e.g. `KeyFlags::SKIP_READ_ONLY | KeyFlags::SKIP_CODED`.
    #[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
    pub struct KeyFlags: u32 {
        /// Iterate over all keys.
        const ALL_KEYS = sys::CODES_KEYS_ITERATOR_ALL_KEYS;
        /// Skip read-only keys.
        const SKIP_READ_ONLY = sys::CODES_KEYS_ITERATOR_SKIP_READ_ONLY;
        /// Skip optional keys.
        const SKIP_OPTIONAL = sys::CODES_KEYS_ITERATOR_SKIP_OPTIONAL;
        /// Skip edition-specific keys.
        const SKIP_EDITION_SPECIFIC = sys::CODES_KEYS_ITERATOR_SKIP_EDITION_SPECIFIC;
        /// Skip keys coded in the message.
        const SKIP_CODED = sys::CODES_KEYS_ITERATOR_SKIP_CODED;
        /// Skip computed keys.
        const SKIP_COMPUTED = sys::CODES_KEYS_ITERATOR_SKIP_COMPUTED;
        /// Skip duplicate key names.
        const SKIP_DUPLICATES = sys::CODES_KEYS_ITERATOR_SKIP_DUPLICATES;
        /// Skip function keys.
        const SKIP_FUNCTION = sys::CODES_KEYS_ITERATOR_SKIP_FUNCTION;
        /// Only dump-relevant keys.
        const DUMP_ONLY = sys::CODES_KEYS_ITERATOR_DUMP_ONLY;
    }
}

/// Iterator over the key names of a message — see
/// [`Handle::keys`](crate::Handle::keys) and
/// [`Handle::keys_in_namespace`](crate::Handle::keys_in_namespace).
///
/// Yields key names; fetch values through
/// [`Handle::get`](crate::Handle::get).
pub struct KeysIterator<'h> {
    raw: NonNull<sys::codes_keys_iterator>,
    _handle: PhantomData<&'h ()>,
}

impl KeysIterator<'_> {
    pub(crate) fn new(
        handle: *mut sys::codes_handle,
        flags: KeyFlags,
        namespace: Option<&str>,
    ) -> Result<Self> {
        let namespace = namespace.map(ckey).transpose()?;
        // SAFETY: valid handle; namespace is either NULL (all keys) or a
        // NUL-terminated string, copied by the library.
        let raw = unsafe {
            sys::codes_keys_iterator_new(
                handle,
                c_ulong::from(flags.bits()),
                namespace
                    .as_ref()
                    .map_or(std::ptr::null(), |ns| ns.as_ptr()),
            )
        };
        NonNull::new(raw)
            .map(|raw| Self {
                raw,
                _handle: PhantomData,
            })
            .ok_or(Error::InvalidKeysIterator)
    }

    /// Restart the iteration (`codes_keys_iterator_rewind`).
    pub fn rewind(&mut self) -> Result<()> {
        check!(sys::codes_keys_iterator_rewind(self.raw.as_ptr()))
    }
}

impl Drop for KeysIterator<'_> {
    fn drop(&mut self) {
        // SAFETY: `raw` is a valid iterator owned by us; freed exactly once.
        unsafe { sys::codes_keys_iterator_delete(self.raw.as_ptr()) };
    }
}

impl Iterator for KeysIterator<'_> {
    type Item = Result<String>;

    fn next(&mut self) -> Option<Self::Item> {
        // SAFETY: `raw` is a valid iterator; returns non-zero while there is
        // a next key.
        if unsafe { sys::codes_keys_iterator_next(self.raw.as_ptr()) } == 0 {
            return None;
        }
        // SAFETY: after a successful `next`, the name is a non-null
        // NUL-terminated string owned by the iterator.
        let name = unsafe { sys::codes_keys_iterator_get_name(self.raw.as_ptr()) };
        if name.is_null() {
            return Some(Err(Error::NullPointer));
        }
        // SAFETY: see above.
        Some(match unsafe { CStr::from_ptr(name) }.to_str() {
            Ok(s) => Ok(s.to_owned()),
            Err(e) => Err(e.into()),
        })
    }
}
