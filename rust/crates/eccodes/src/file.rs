//! Files of messages — [`MessageFile`] — and the iterators that read them.
//!
//! ```no_run
//! use eccodes::GribFile;
//!
//! # fn main() -> eccodes::Result<()> {
//! let file = GribFile::open("data.grib2")?;
//! println!("{} messages in {}", file.count()?, file.path().display());
//!
//! for message in &file {
//!     let message = message?;
//!     println!("{}", message.get::<String>("shortName")?);
//! }
//! # Ok(())
//! # }
//! ```
//!
//! A [`MessageFile`] is a path, not an open stream: it can be counted, then
//! iterated, then iterated again. To read from a stream you already have, use
//! [`Messages::from_file`]; to read messages out of memory,
//! [`Messages::from_bytes`].

use std::ffi::{c_int, c_void};
use std::fmt;
use std::iter::FusedIterator;
use std::marker::PhantomData;
use std::path::{Path, PathBuf};
use std::ptr::{self, NonNull};

use eccodes_sys as sys;

use crate::error::{Code, Error, ErrorContext, Result};
use crate::ffi;
use crate::index::Index;
use crate::kind::{Any, Bufr, Grib, MessageKind, product_of};
use crate::message::Message;

/// A file of messages of one product.
///
/// The product is chosen by naming the type — [`GribFile`], [`BufrFile`], or
/// `MessageFile` for a file that may hold anything.
pub struct MessageFile<K: MessageKind = Any> {
    path: PathBuf,
    _kind: PhantomData<K>,
}

/// A file of GRIB messages.
pub type GribFile = MessageFile<Grib>;

/// A file of BUFR messages.
pub type BufrFile = MessageFile<Bufr>;

impl<K: MessageKind> MessageFile<K> {
    /// Open the file at `path`.
    ///
    /// Fails now if the path is missing, unreadable or not a file, rather
    /// than on the first message.
    pub fn open(path: impl AsRef<Path>) -> Result<Self> {
        let path = path.as_ref();
        let metadata = std::fs::metadata(path).with_path(path)?;
        if !metadata.is_file() {
            return Err(Code::IoProblem).with_path(path);
        }
        Ok(Self {
            path: path.to_path_buf(),
            _kind: PhantomData,
        })
    }

    /// The path this file was opened from.
    #[must_use]
    pub fn path(&self) -> &Path {
        &self.path
    }

    /// How many messages of this product the file holds.
    ///
    /// A file with none — a BUFR file counted as GRIB, say — is `0`, not an
    /// error.
    pub fn count(&self) -> Result<usize> {
        Ok(self.scan()?.len())
    }

    /// Where each message of this product starts, in bytes from the start of
    /// the file.
    ///
    /// Useful for indexing a file you will come back to; empty when the file
    /// holds no message of this product.
    pub fn offsets(&self) -> Result<Vec<u64>> {
        self.scan()
    }

    /// Read the messages of this product, in file order.
    pub fn messages(&self) -> Result<Messages<'_, K>> {
        Ok(Messages {
            source: Source::Stream(ffi::CFile::open(&self.path)?),
            done: false,
            _kind: PhantomData,
        })
    }

    /// Index this file on `keys`, for selecting messages by value.
    ///
    /// ```no_run
    /// # fn main() -> eccodes::Result<()> {
    /// let file = eccodes::GribFile::open("data.grib2")?;
    /// let mut index = file.index(["shortName", "step"])?;
    /// # Ok(())
    /// # }
    /// ```
    pub fn index<I, S>(&self, keys: I) -> Result<Index>
    where
        I: IntoIterator<Item = S>,
        S: AsRef<str>,
    {
        Index::from_file(&self.path, keys)
    }

    /// The offsets of every message of this product (`codes_extract_offsets_malloc`).
    fn scan(&self) -> Result<Vec<u64>> {
        let cpath = ffi::cpath(&self.path)?;
        // Typed by the C signature (`off_t*`), so this module names no libc type.
        let mut offsets = ptr::null_mut();
        let mut count: c_int = 0;
        // SAFETY: a NULL context selects the default one; `cpath` is
        // NUL-terminated, and the out-pointers address locals. On success the
        // library hands us an array of `count` offsets to free.
        let status = unsafe {
            sys::codes_extract_offsets_malloc(
                ptr::null_mut(),
                cpath.as_ptr(),
                product_of::<K>(),
                &raw mut offsets,
                &raw mut count,
                0,
            )
        };

        // Take the array first, whatever the status: the library allocates
        // before it can fail, and never frees it for us.
        // SAFETY: `offsets` is NULL or the array the library allocated, sized
        // by `count`; a negative count means nothing was written.
        let scanned = unsafe { ffi::take_offsets(offsets, ffi::to_usize(count).unwrap_or(0)) };
        let scanned = Error::from_raw(status).and_then(|()| scanned);

        match scanned {
            // A file with no message of this product is empty, not broken:
            // the C call reports "invalid message" for an empty result, which
            // would otherwise make `count()` fail on a perfectly good file of
            // some other product.
            Err(err) if err.code() == Some(Code::InvalidMessage) => Ok(Vec::new()),
            other => other.with_path(&self.path),
        }
    }
}

impl<K: MessageKind> Clone for MessageFile<K> {
    fn clone(&self) -> Self {
        Self {
            path: self.path.clone(),
            _kind: PhantomData,
        }
    }
}

impl<K: MessageKind> fmt::Debug for MessageFile<K> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "MessageFile<{}>({})", K::NAME, self.path.display())
    }
}

impl<'f, K: MessageKind> IntoIterator for &'f MessageFile<K> {
    type Item = Result<Message<K>>;
    type IntoIter = Messages<'f, K>;

    /// Read the file's messages, reporting a failure to open it as the first
    /// item — see [`MessageFile::messages`] to handle that up front.
    fn into_iter(self) -> Messages<'f, K> {
        match self.messages() {
            Ok(messages) => messages,
            Err(err) => Messages {
                source: Source::Failed(Some(err)),
                done: false,
                _kind: PhantomData,
            },
        }
    }
}

/// Messages read one after another from a file, a stream or memory.
///
/// Yields `Err` at most once: after a failure, or at the end of the source,
/// the iterator is done.
pub struct Messages<'src, K: MessageKind = Any> {
    source: Source<'src>,
    done: bool,
    _kind: PhantomData<K>,
}

enum Source<'src> {
    Stream(ffi::CFile),
    Bytes(&'src [u8]),
    /// The source could not be opened; the error is yielded once, then taken.
    Failed(Option<Error>),
}

impl<K: MessageKind> Messages<'static, K> {
    /// Read messages from an open file.
    ///
    /// The [`File`](std::fs::File) is consumed: reading messages moves its
    /// cursor, so no second owner may hold it.
    pub fn from_file(file: std::fs::File) -> Result<Self> {
        Ok(Self {
            source: Source::Stream(ffi::CFile::from_file(file)?),
            done: false,
            _kind: PhantomData,
        })
    }
}

impl<'src> Messages<'src, Grib> {
    /// Read GRIB messages out of a buffer, single- and multi-field alike.
    ///
    /// Each message owns its bytes, so the messages may outlive `bytes`.
    #[must_use]
    pub const fn from_bytes(bytes: &'src [u8]) -> Self {
        Self {
            source: Source::Bytes(bytes),
            done: false,
            _kind: PhantomData,
        }
    }
}

impl<K: MessageKind> Messages<'_, K> {
    /// The next message in a stream (`codes_handle_new_from_file`).
    fn next_in_stream(stream: &ffi::CFile) -> Option<Result<Message<K>>> {
        let mut status: c_int = 0;
        // SAFETY: a NULL context selects the default one; the stream is open,
        // and the status out-pointer addresses a local.
        let raw = unsafe {
            sys::codes_handle_new_from_file(
                ptr::null_mut(),
                stream.as_ptr(),
                product_of::<K>(),
                &raw mut status,
            )
        };
        match NonNull::new(raw) {
            Some(raw) => Some(Ok(Message::from_raw(raw))),
            // NULL with a success status is the end of the file.
            None => Error::from_raw(status).err().map(Err),
        }
    }

    /// The next GRIB message in a buffer, advancing `bytes` past it
    /// (`codes_grib_handle_new_from_multi_message`).
    fn next_in_bytes(bytes: &mut &[u8]) -> Option<Result<Message<K>>> {
        if bytes.is_empty() {
            return None;
        }
        let mut cursor = bytes.as_ptr().cast::<c_void>().cast_mut();
        let mut left = bytes.len();
        let mut status: c_int = 0;
        // SAFETY: a NULL context selects the default one; `cursor`/`left`
        // describe the unread tail of the buffer and are advanced by the
        // library past the message it decoded.
        let raw = unsafe {
            sys::codes_grib_handle_new_from_multi_message(
                ptr::null_mut(),
                &raw mut cursor,
                &raw mut left,
                &raw mut status,
            )
        };
        *bytes = &bytes[bytes.len() - left..];

        let raw = NonNull::new(raw)?;
        // The decoded message points into the caller's buffer; cloning it
        // gives the caller a message that owns its bytes.
        let borrowed: Message<K> = Message::from_raw(raw);
        Some(Error::from_raw(status).and_then(|()| borrowed.try_clone()))
    }
}

impl<K: MessageKind> Iterator for Messages<'_, K> {
    type Item = Result<Message<K>>;

    fn next(&mut self) -> Option<Self::Item> {
        if self.done {
            return None;
        }
        let next = match &mut self.source {
            Source::Stream(stream) => Self::next_in_stream(stream),
            Source::Bytes(bytes) => Self::next_in_bytes(bytes),
            Source::Failed(err) => err.take().map(Err),
        };
        if next.as_ref().is_none_or(Result::is_err) {
            self.done = true;
        }
        next
    }
}

impl<K: MessageKind> FusedIterator for Messages<'_, K> {}

impl<K: MessageKind> fmt::Debug for Messages<'_, K> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let source = match self.source {
            Source::Stream(_) => "stream",
            Source::Bytes(_) => "bytes",
            Source::Failed(_) => "failed",
        };
        write!(f, "Messages<{}> {{ {source}, done: {} }}", K::NAME, self.done)
    }
}
