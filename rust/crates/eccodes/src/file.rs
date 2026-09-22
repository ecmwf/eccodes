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
//! iterated, then iterated again. Everything else is read through
//! [`MessageFile::messages_from`], which takes any [`Read`] — a socket, a
//! pipe, or a buffer in memory. Both hand back the same [`Messages`]
//! iterator, which is the only way to get one.
//!
//! A file is named, never handed over already open. The C library wants a
//! `FILE*` for every message it reads, and the reader that opened that stream
//! is the only one that can close it at the right moment — after handing back
//! the decoding state the library files under it, which is what `Messages`
//! does when it is dropped. It is also what upstream asks of its callers:
//! an application's own `FILE*` reaching the library is undefined wherever
//! the two were built against different C runtimes (ecmwf/eccodes#374).

use std::ffi::c_int;
use std::fmt;
use std::io::Read;
use std::iter::FusedIterator;
use std::marker::PhantomData;
use std::path::{Path, PathBuf};
use std::ptr::{self, NonNull};

use eccodes_sys as sys;

use crate::error::{Code, Error, ErrorContext, Result, check};
use crate::ffi;
use crate::index::Index;
use crate::kind::{Any, Bufr, Grib, MessageKind, product_of};
use crate::message::Message;
use crate::multi;

/// A file of messages of one product.
///
/// The product is chosen by naming the type — [`GribFile`], [`BufrFile`], or
/// `MessageFile` for a file that may hold anything.
pub struct MessageFile<K: MessageKind = Any> {
    path: PathBuf,
    /// Read one message per field of a multi-field GRIB message — see
    /// [`MessageFile::multi_field`].
    fields: bool,
    _kind: PhantomData<K>,
}

/// A file of messages of any product.
///
/// The same type as a bare `MessageFile`, named so that the product is
/// spelled out at a call site: `AnyFile::open(path)` rather than
/// `MessageFile::<Any>::open(path)`.
pub type AnyFile = MessageFile<Any>;

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
            fields: false,
            _kind: PhantomData,
        })
    }

    /// Read the messages of this product from anything that reads bytes — a
    /// socket, a pipe, standard input, a buffer in memory.
    ///
    /// Messages are decoded one at a time, as the reader yields them, and
    /// each owns its bytes. Products other than this one are skipped, as they
    /// are when reading a file of this product.
    ///
    /// The library looks for the start of a message a byte at a time, so hand
    /// it a [`BufReader`](std::io::BufReader) around anything whose reads cost
    /// a syscall.
    ///
    /// A multi-field GRIB message reads as the one message it is: the C
    /// library keeps the state for splitting one into fields against the
    /// `FILE*` it came from, so that is offered on files only — see
    /// [`MessageFile::multi_field`].
    ///
    /// ```no_run
    /// use eccodes::GribFile;
    ///
    /// # fn main() -> eccodes::Result<()> {
    /// for message in GribFile::messages_from(std::io::stdin().lock()) {
    ///     println!("{}", message?.get::<String>("shortName")?);
    /// }
    ///
    /// let bytes = std::fs::read("data.grib2")?;
    /// println!("{} messages", GribFile::messages_from(&bytes[..]).count());
    /// # Ok(())
    /// # }
    /// ```
    #[must_use]
    pub fn messages_from<'src>(reader: impl Read + 'src) -> Messages<'src, K> {
        Messages {
            source: Source::Reader(ffi::ReadStream::new(reader)),
            fields: false,
            done: false,
            _kind: PhantomData,
        }
    }

    /// The path this file was opened from.
    #[must_use]
    pub fn path(&self) -> &Path {
        &self.path
    }

    /// How many messages of this product the file holds — as many as
    /// [`messages`](Self::messages) will yield.
    ///
    /// A file with none — a BUFR file counted as GRIB, say — is `0`, not an
    /// error.
    pub fn count(&self) -> Result<usize> {
        if self.fields {
            // Fields are counted by decoding them: the file frames a
            // multi-field message as one message, and only the decoder knows
            // how many fields it holds. This is what the C counter does
            // internally with the switch on, minus its refusal to count a
            // multi-field GRIB at all.
            let mut count = 0;
            for message in self.messages()? {
                message?;
                count += 1;
            }
            return Ok(count);
        }
        if K::EXPECTED.is_some() {
            return Ok(self.scan()?.len());
        }
        // Counting any product goes through the C counter rather than the
        // offsets scan, which reports only what it can frame.
        let cpath = ffi::cpath(&self.path)?;
        let mut count: c_int = 0;
        multi::reading(false, || {
            check!(sys::codes_count_in_filename(
                ptr::null_mut(),
                cpath.as_ptr(),
                &raw mut count
            ))
        })
        .with_path(&self.path)?;
        ffi::to_usize(count)
    }

    /// Where each message of this product starts, in bytes from the start of
    /// the file.
    ///
    /// Useful for indexing a file you will come back to; empty when the file
    /// holds no message of this product.
    ///
    /// These are the messages as the file frames them, always: a multi-field
    /// GRIB message is one message here however the file is read, so with
    /// [`multi_field`](MessageFile::multi_field) on there are fewer offsets
    /// than [`count`](Self::count) reports.
    pub fn offsets(&self) -> Result<Vec<u64>> {
        self.scan()
    }

    /// Read the messages of this product, in file order.
    ///
    /// The reader owns its own stream, so it does not borrow this file: the
    /// same file can be read twice at once, and
    /// `GribFile::open(path)?.messages()?` needs no binding to keep alive.
    pub fn messages(&self) -> Result<Messages<'static, K>> {
        Ok(Messages {
            source: Source::Stream(ffi::CFile::open(&self.path)?),
            fields: self.fields,
            done: false,
            _kind: PhantomData,
        })
    }

    /// Read the messages of this product, in file order, reporting a failure
    /// to open the file as the first item.
    ///
    /// The same as iterating `&file`; [`messages`](Self::messages) reports
    /// that failure up front instead.
    #[must_use]
    pub fn iter(&self) -> Messages<'static, K> {
        self.into_iter()
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
    ///
    /// Framing only, so it runs with the multi-field switch off whatever this
    /// file was asked for: the C call refuses a GRIB file outright while the
    /// switch is on.
    fn scan(&self) -> Result<Vec<u64>> {
        let cpath = ffi::cpath(&self.path)?;
        // Typed by the C signature (`off_t*`), so this module names no libc type.
        let mut offsets = ptr::null_mut();
        let mut count: c_int = 0;
        // SAFETY: a NULL context selects the default one; `cpath` is
        // NUL-terminated, and the out-pointers address locals. On success the
        // library hands us an array of `count` offsets to free.
        let status = multi::reading(false, || unsafe {
            sys::codes_extract_offsets_malloc(
                ptr::null_mut(),
                cpath.as_ptr(),
                product_of::<K>(),
                &raw mut offsets,
                &raw mut count,
                0,
            )
        });

        // Take the array first, whatever the status: the library allocates
        // before it can fail, and never frees it for us.
        // SAFETY: `offsets` is NULL or the array the library allocated, sized
        // by `count`; a negative count means nothing was written.
        let scanned = unsafe { ffi::take_offsets(offsets, ffi::to_usize(count).unwrap_or(0)) };
        let scanned = Error::from_raw(status).and(scanned);

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

impl MessageFile<Grib> {
    /// Read a multi-field GRIB message as one message per field.
    ///
    /// A multi-field message repeats sections 4-8 per field. Off — the
    /// default — such a message reads as the single message it is, keyed by
    /// its first field; on, it reads as one message per field, and
    /// [`count`](Self::count) counts fields.
    ///
    /// ```no_run
    /// # fn main() -> eccodes::Result<()> {
    /// let file = eccodes::GribFile::open("combined.grib2")?.multi_field(true);
    /// for field in &file {
    ///     println!("{}", field?.get::<i64>("step")?);
    /// }
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// This is a property of the reader. The C library takes it from a switch
    /// on its global context, which this crate turns on only for the length
    /// of the calls that read it, so asking one file for fields never changes
    /// what another reader — or another thread — sees.
    #[must_use]
    pub const fn multi_field(mut self, enabled: bool) -> Self {
        self.fields = enabled;
        self
    }
}

impl<K: MessageKind> Clone for MessageFile<K> {
    fn clone(&self) -> Self {
        Self {
            path: self.path.clone(),
            fields: self.fields,
            _kind: PhantomData,
        }
    }
}

impl<K: MessageKind> fmt::Debug for MessageFile<K> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "MessageFile<{}>({})", K::NAME, self.path.display())
    }
}

impl<K: MessageKind> IntoIterator for &MessageFile<K> {
    type Item = Result<Message<K>>;
    type IntoIter = Messages<'static, K>;

    /// Read the file's messages, reporting a failure to open it as the first
    /// item — see [`MessageFile::messages`] to handle that up front.
    fn into_iter(self) -> Messages<'static, K> {
        match self.messages() {
            Ok(messages) => messages,
            Err(err) => Messages {
                source: Source::Failed(Some(err)),
                fields: self.fields,
                done: false,
                _kind: PhantomData,
            },
        }
    }
}

/// Messages read one after another from a file or a reader.
///
/// Handed out by [`MessageFile::messages`] and
/// [`MessageFile::messages_from`] — a message source is always named as a
/// file of some product, whatever the bytes then come from.
///
/// Yields `Err` at most once: after a failure, or at the end of the source,
/// the iterator is done.
pub struct Messages<'src, K: MessageKind = Any> {
    source: Source<'src>,
    /// One message per field, for the multi-field GRIB messages in the
    /// source — set by [`MessageFile::multi_field`], off for every reader
    /// built here.
    fields: bool,
    done: bool,
    _kind: PhantomData<K>,
}

enum Source<'src> {
    Stream(ffi::CFile),
    Reader(ffi::ReadStream<'src>),
    /// The source could not be opened; the error is yielded once, then taken.
    Failed(Option<Error>),
}

impl<K: MessageKind> Messages<'_, K> {
    /// The next message in a stream (`codes_handle_new_from_file`), or its
    /// next field when `fields` is set.
    fn next_in_stream(stream: &ffi::CFile, fields: bool) -> Option<Result<Message<K>>> {
        let mut status: c_int = 0;
        // SAFETY: a NULL context selects the default one; the stream is open,
        // and the status out-pointer addresses a local.
        let raw = multi::reading(fields, || unsafe {
            sys::codes_handle_new_from_file(
                ptr::null_mut(),
                stream.as_ptr(),
                product_of::<K>(),
                &raw mut status,
            )
        });
        let Some(raw) = NonNull::new(raw) else {
            // NULL with a success status is the end of the file.
            return Error::from_raw(status).err().map(Err);
        };
        Some(Ok(Message::from_raw(raw)))
    }

    /// The next message a reader yields (`codes_handle_new_from_stream`).
    ///
    /// The C reader takes no product: it decodes whatever framing comes next,
    /// so messages of other products are skipped here, leaving the same
    /// stream of messages a file of this product would yield.
    fn next_in_reader(stream: &mut ffi::ReadStream<'_>) -> Option<Result<Message<K>>> {
        loop {
            let mut status: c_int = 0;
            // SAFETY: a NULL context selects the default one; `stream` is
            // borrowed for the call, and `read_stream` is its own callback.
            let raw = unsafe {
                sys::codes_handle_new_from_stream(
                    ptr::null_mut(),
                    stream.as_data(),
                    Some(ffi::read_stream),
                    &raw mut status,
                )
            };
            let Some(raw) = NonNull::new(raw) else {
                // The reader's own failure is reported to C as the end of the
                // stream, and says more than the status does.
                return stream
                    .take_failure()
                    .or_else(|| Error::from_raw(status).err())
                    .map(Err);
            };

            let message = Message::<Any>::from_raw(raw);
            let Some(expected) = K::EXPECTED else {
                return Some(Ok(message.retag()));
            };
            match message.kind() {
                Ok(kind) if kind == expected => return Some(Ok(message.retag())),
                // Another product: read on to the next message.
                Ok(_) => {}
                Err(err) => return Some(Err(err)),
            }
        }
    }
}

impl<K: MessageKind> Iterator for Messages<'_, K> {
    type Item = Result<Message<K>>;

    fn next(&mut self) -> Option<Self::Item> {
        if self.done {
            return None;
        }
        let next = match &mut self.source {
            Source::Stream(stream) => Self::next_in_stream(stream, self.fields),
            Source::Reader(stream) => Self::next_in_reader(stream),
            Source::Failed(err) => err.take().map(Err),
        };
        if next.as_ref().is_none_or(Result::is_err) {
            self.done = true;
        }
        next
    }
}

impl<K: MessageKind> Drop for Messages<'_, K> {
    /// Hand back the C library's half-decoded multi-field state for this
    /// stream before the stream closes, so that the next reader to be given
    /// its address does not resume this one's message. Only a reader that
    /// asked for fields can leave any, and only one stopped between two
    /// fields actually does.
    fn drop(&mut self) {
        if let (true, Source::Stream(stream)) = (self.fields, &self.source) {
            multi::forget(stream.as_ptr());
        }
    }
}

impl<K: MessageKind> FusedIterator for Messages<'_, K> {}

impl<K: MessageKind> fmt::Debug for Messages<'_, K> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let source = match self.source {
            Source::Stream(_) => "stream",
            Source::Reader(_) => "reader",
            Source::Failed(_) => "failed",
        };
        write!(
            f,
            "Messages<{}> {{ {source}, done: {} }}",
            K::NAME,
            self.done
        )
    }
}
