//! `CodesFile` and its message iterators over the official crate's [`Messages`] stream.

use std::{fmt::Debug, fs::File, io::Cursor, marker::PhantomData, path::Path};

use eccodes::{GribFile, Messages, kind::Grib};
use fallible_iterator::FallibleIterator;

use crate::codes_message::{ArcMessage, RefMessage};
use crate::errors::CodesError;

/// The kind of product inside the handled file.
#[derive(Copy, Clone, Eq, PartialEq, Ord, PartialOrd, Hash, Debug)]
pub enum ProductKind {
    #[allow(missing_docs)]
    GRIB,
}

/// Access to a GRIB file; messages are read through
/// [`ref_message_iter()`](CodesFile::ref_message_iter) or
/// [`arc_message_iter()`](CodesFile::arc_message_iter).
///
/// `D` records what the data came from (`File` or `Vec<u8>`), as in the
/// original crate; the stream state lives in the official crate either way.
pub struct CodesFile<D: Debug> {
    // The one C-side cursor over the file: every iterator advances it, so
    // successive `ref_message_iter()` calls continue where the last stopped.
    messages: Messages<'static, Grib>,
    _data: PhantomData<D>,
}

impl<D: Debug> Debug for CodesFile<D> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("CodesFile").finish_non_exhaustive()
    }
}

impl CodesFile<File> {
    /// Opens the file at `file_path` as `product_kind`.
    pub fn new_from_file<P: AsRef<Path> + Debug>(
        file_path: P,
        product_kind: ProductKind,
    ) -> Result<Self, CodesError> {
        let ProductKind::GRIB = product_kind;
        Ok(Self {
            messages: GribFile::open(file_path)?.messages()?,
            _data: PhantomData,
        })
    }
}

impl CodesFile<Vec<u8>> {
    /// Opens the data in `file_data` as `product_kind`.
    pub fn new_from_memory(
        file_data: Vec<u8>,
        product_kind: ProductKind,
    ) -> Result<Self, CodesError> {
        let ProductKind::GRIB = product_kind;
        Ok(Self {
            messages: GribFile::messages_from(Cursor::new(file_data)),
            _data: PhantomData,
        })
    }
}

impl<D: Debug> CodesFile<D> {
    /// Iterator yielding messages as [`RefMessage`] tied to this file.
    pub const fn ref_message_iter(&mut self) -> RefMessageIter<'_, D> {
        RefMessageIter { file: self }
    }

    /// Iterator yielding messages as [`ArcMessage`], consuming the file.
    #[must_use]
    pub const fn arc_message_iter(self) -> ArcMessageIter<D> {
        ArcMessageIter { file: self }
    }

    fn next_message(&mut self) -> Result<Option<eccodes::GribMessage>, CodesError> {
        match self.messages.next() {
            None => Ok(None),
            Some(Ok(message)) => Ok(Some(message)),
            Some(Err(err)) => Err(err.into()),
        }
    }
}

/// Iterator over messages returning [`RefMessage`] with lifetime tied to the `CodesFile`.
#[derive(Debug)]
pub struct RefMessageIter<'a, D: Debug> {
    file: &'a mut CodesFile<D>,
}

impl<'ch, D: Debug> FallibleIterator for RefMessageIter<'ch, D> {
    type Item = RefMessage<'ch>;
    type Error = CodesError;

    fn next(&mut self) -> Result<Option<Self::Item>, Self::Error> {
        Ok(self.file.next_message()?.map(RefMessage::new))
    }
}

/// Iterator over messages returning [`ArcMessage`], which can cross threads.
#[derive(Debug)]
pub struct ArcMessageIter<D: Debug> {
    file: CodesFile<D>,
}

impl<D: Debug> FallibleIterator for ArcMessageIter<D> {
    type Item = ArcMessage<D>;
    type Error = CodesError;

    fn next(&mut self) -> Result<Option<Self::Item>, Self::Error> {
        Ok(self.file.next_message()?.map(ArcMessage::new))
    }
}
