//! `KeysIterator` over the official crate's [`Keys`](eccodes::Keys) iterator.

use std::fmt::{self, Debug};

use eccodes::{KeyFlags, Keys};
use fallible_iterator::FallibleIterator;

use crate::codes_message::CodesMessage;
use crate::errors::CodesError;

/// Iterates through the key names of a [`CodesMessage`].
pub struct KeysIterator<'a> {
    keys: Keys<'a>,
}

impl Debug for KeysIterator<'_> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("KeysIterator").finish_non_exhaustive()
    }
}

/// Flags selecting the subset of keys to iterate over; combine as needed.
#[derive(Copy, Clone, Eq, PartialEq, Ord, PartialOrd, Hash, Debug)]
#[allow(clippy::cast_possible_wrap)] // flag bits are small constants
pub enum KeysIteratorFlags {
    /// Iterate over all keys.
    AllKeys = KeyFlags::ALL_KEYS.bits() as isize,
    /// Iterate only over dump keys.
    DumpOnly = KeyFlags::DUMP_ONLY.bits() as isize,
    /// Exclude coded keys from iteration.
    SkipCoded = KeyFlags::SKIP_CODED.bits() as isize,
    /// Exclude computed keys from iteration.
    SkipComputed = KeyFlags::SKIP_COMPUTED.bits() as isize,
    /// Exclude function keys from iteration.
    SkipFunction = KeyFlags::SKIP_FUNCTION.bits() as isize,
    /// Exclude optional keys from iteration.
    SkipOptional = KeyFlags::SKIP_OPTIONAL.bits() as isize,
    /// Exclude read-only keys from iteration.
    SkipReadOnly = KeyFlags::SKIP_READ_ONLY.bits() as isize,
    /// Exclude duplicate keys from iteration.
    SkipDuplicates = KeyFlags::SKIP_DUPLICATES.bits() as isize,
    /// Exclude file-edition-specific keys from iteration.
    SkipEditionSpecific = KeyFlags::SKIP_EDITION_SPECIFIC.bits() as isize,
}

impl<P: Debug> CodesMessage<P> {
    /// [`KeysIterator`] with the given flags and namespace (`""` for all keys).
    pub fn new_keys_iterator<'a>(
        &'a mut self,
        flags: &[KeysIteratorFlags],
        namespace: &str,
    ) -> Result<KeysIterator<'a>, CodesError> {
        #[allow(clippy::cast_sign_loss)] // discriminants are the C flag bits
        let flags = flags.iter().fold(KeyFlags::empty(), |set, flag| {
            set | KeyFlags::from_bits_truncate(*flag as u32)
        });

        // The exclusive borrow makes any locking moot: nothing else can
        // touch the message while the iterator (lifetime 'a) is alive.
        let message = &*self.get_mut();
        let mut query = message.keys().flags(flags);
        if !namespace.is_empty() {
            query = query.namespace(namespace);
        }

        Ok(KeysIterator {
            keys: query.into_iter(),
        })
    }

    /// [`KeysIterator`] over all keys of the message.
    pub fn default_keys_iterator(&mut self) -> Result<KeysIterator<'_>, CodesError> {
        self.new_keys_iterator(&[KeysIteratorFlags::AllKeys], "")
    }
}

impl FallibleIterator for KeysIterator<'_> {
    type Item = String;
    type Error = CodesError;

    fn next(&mut self) -> Result<Option<Self::Item>, Self::Error> {
        match self.keys.next() {
            None => Ok(None),
            Some(Ok(name)) => Ok(Some(name)),
            Some(Err(err)) => Err(err.into()),
        }
    }
}
