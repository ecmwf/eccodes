//! Keys iterator — iterate over key names in a message.
//!
//! # Safety note
//!
//! The underlying `codes_keys_iterator_new` requires a `*mut codes_handle`.
//! `KeysIterator` is created from `&Message` (shared reference) but
//! internally casts to `*mut` because the C API does not distinguish
//! const / mutable handles for this call.  **Do not create multiple
//! `KeysIterator`s on the same `Message` simultaneously** — the ecCodes
//! library does not support concurrent iteration on one handle.

use crate::message::Message;
use std::ffi::{CStr, CString};
use std::marker::PhantomData;
use std::os::raw::c_ulong;

/// Iterator over the keys of a [`Message`].
///
/// Created via [`Message::keys`], [`Message::keys_filtered`], or
/// [`Message::keys_in_namespace`].
///
/// **Important:** only one `KeysIterator` should exist per `Message` at a
/// time.  Creating multiple iterators on the same message leads to
/// undefined behaviour in the underlying C library.
pub struct KeysIterator<'a> {
    kiter: *mut eccodes_sys::codes_keys_iterator,
    _marker: PhantomData<&'a Message>,
}

impl<'a> KeysIterator<'a> {
    pub(crate) fn new(
        message: &'a Message,
        flags: u64,
        namespace: Option<&str>,
    ) -> Self {
        let ns_cstr = namespace.map(|ns| CString::new(ns).unwrap());
        let ns_ptr = ns_cstr.as_ref().map_or(std::ptr::null(), |cs| cs.as_ptr());
        // SAFETY: codes_keys_iterator_new requires *mut but does not modify the
        // handle in a way that invalidates other reads. We hold &'a Message which
        // prevents mutable access to the Message for the lifetime of this iterator.
        let kiter = unsafe {
            eccodes_sys::codes_keys_iterator_new(
                message.handle as *mut _,
                flags as c_ulong,
                ns_ptr,
            )
        };
        KeysIterator {
            kiter,
            _marker: PhantomData,
        }
    }
}

impl<'a> Iterator for KeysIterator<'a> {
    type Item = String;

    fn next(&mut self) -> Option<Self::Item> {
        if self.kiter.is_null() {
            return None;
        }
        let has_next = unsafe { eccodes_sys::codes_keys_iterator_next(self.kiter) };
        if has_next == 0 {
            return None;
        }
        let name_ptr = unsafe { eccodes_sys::codes_keys_iterator_get_name(self.kiter) };
        if name_ptr.is_null() {
            return None;
        }
        let name = unsafe { CStr::from_ptr(name_ptr) };
        Some(name.to_string_lossy().into_owned())
    }
}

impl<'a> Drop for KeysIterator<'a> {
    fn drop(&mut self) {
        if !self.kiter.is_null() {
            unsafe { eccodes_sys::codes_keys_iterator_delete(self.kiter); }
        }
    }
}
