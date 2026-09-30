//! `CodesMessage` and key access, delegating to the official crate's [`GribMessage`].

use std::{
    cmp::Ordering,
    fmt::{self, Debug},
    fs::OpenOptions,
    io::Write,
    marker::PhantomData,
    path::Path,
    sync::{Mutex, MutexGuard, PoisonError},
};

use eccodes::{GribMessage, KeyType};

use crate::errors::CodesError;

/// Base structure for [`RefMessage`], [`ArcMessage`] and [`BufMessage`]:
/// one message of a GRIB file, a collection of key-value pairs.
pub struct CodesMessage<P: Debug> {
    _parent: P,
    // The official crate's message is Send but not Sync; the mutex restores
    // Sync for `ArcMessage` sharing, serialising reads instead of trusting
    // concurrent access to one C handle.
    inner: Mutex<GribMessage>,
}

/// Marker tying [`RefMessage`] to its parent file's lifetime.
#[derive(Debug, Hash, PartialEq, Eq, PartialOrd)]
#[doc(hidden)]
pub struct RefParent<'ch>(PhantomData<&'ch ()>);

/// Marker for the independent, editable [`BufMessage`].
#[derive(Debug, Hash, PartialEq, Eq, PartialOrd)]
#[doc(hidden)]
pub struct BufParent();

/// Marker recording the file source type of an [`ArcMessage`].
#[derive(Debug)]
#[doc(hidden)]
pub struct ArcParent<D: Debug>(PhantomData<D>);

/// [`CodesMessage`] with its lifetime tied to its parent `CodesFile`.
pub type RefMessage<'ch> = CodesMessage<RefParent<'ch>>;

/// [`CodesMessage`] that can be moved and shared across threads.
pub type ArcMessage<D> = CodesMessage<ArcParent<D>>;

/// [`CodesMessage`] independent of its parent, editable with [`KeyWrite`].
pub type BufMessage = CodesMessage<BufParent>;

impl RefMessage<'_> {
    pub(crate) const fn new(inner: GribMessage) -> Self {
        Self {
            _parent: RefParent(PhantomData),
            inner: Mutex::new(inner),
        }
    }
}

impl<D: Debug> ArcMessage<D> {
    pub(crate) const fn new(inner: GribMessage) -> Self {
        Self {
            _parent: ArcParent(PhantomData),
            inner: Mutex::new(inner),
        }
    }
}

impl BufMessage {
    pub(crate) const fn new(inner: GribMessage) -> Self {
        Self {
            _parent: BufParent(),
            inner: Mutex::new(inner),
        }
    }
}

impl<P: Debug> Debug for CodesMessage<P> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("CodesMessage").finish_non_exhaustive()
    }
}

impl<P: Debug> CodesMessage<P> {
    // A poisoned lock only means a reader panicked; the message is intact.
    pub(crate) fn lock(&self) -> MutexGuard<'_, GribMessage> {
        self.inner.lock().unwrap_or_else(PoisonError::into_inner)
    }

    pub(crate) fn lock_mut(&mut self) -> &mut GribMessage {
        self.inner.get_mut().unwrap_or_else(PoisonError::into_inner)
    }
}

/// A key's native storage type, as ecCodes reports it.
#[doc(hidden)]
#[derive(Copy, Eq, PartialEq, Clone, Ord, PartialOrd, Hash, Debug)]
pub enum NativeKeyType {
    Undefined,
    Long,
    Double,
    Str,
    Bytes,
    Section,
    Label,
    Missing,
}

/// Key metadata reads backing [`KeyRead`]; not part of the stable surface.
#[doc(hidden)]
pub trait KeyPropertiesRead {
    fn get_key_size(&self, key_name: &str) -> Result<usize, CodesError>;
    fn get_key_native_type(&self, key_name: &str) -> Result<NativeKeyType, CodesError>;
}

impl<P: Debug> KeyPropertiesRead for CodesMessage<P> {
    fn get_key_size(&self, key_name: &str) -> Result<usize, CodesError> {
        Ok(self.lock().key(key_name).len()?)
    }

    fn get_key_native_type(&self, key_name: &str) -> Result<NativeKeyType, CodesError> {
        let message = self.lock();
        let key_type = message.key(key_name).value_type()?;
        drop(message);
        Ok(match key_type {
            KeyType::Undefined => NativeKeyType::Undefined,
            KeyType::I64 => NativeKeyType::Long,
            KeyType::F64 => NativeKeyType::Double,
            KeyType::String => NativeKeyType::Str,
            KeyType::Bytes => NativeKeyType::Bytes,
            KeyType::Section => NativeKeyType::Section,
            KeyType::Label => NativeKeyType::Label,
            KeyType::Missing => NativeKeyType::Missing,
            // KeyType is non_exhaustive; no further variants exist today.
            _ => return Err(CodesError::UnrecognizedKeyTypeCode(0)),
        })
    }
}

/// GRIB key reading; implemented by [`CodesMessage`] for all key types.
pub trait KeyRead<T> {
    /// Reads the key, checking that its native type matches `T`.
    fn read_key(&self, name: &str) -> Result<T, CodesError>;

    /// Reads the key without the native-type check, letting ecCodes convert.
    fn read_key_unchecked(&self, name: &str) -> Result<T, CodesError>;
}

macro_rules! key_size_check {
    (scalar, $size_var:ident) => {
        match $size_var.cmp(&1) {
            Ordering::Greater => return Err(CodesError::WrongRequestedKeySize),
            Ordering::Less => return Err(CodesError::IncorrectKeySize),
            Ordering::Equal => (),
        }
    };
    (array, $size_var:ident) => {
        if $size_var < 1 {
            return Err(CodesError::IncorrectKeySize);
        }
    };
}

macro_rules! impl_key_read {
    ($key_sizing:ident, $key_type:pat, $gen_type:ty) => {
        impl<P: Debug> KeyRead<$gen_type> for CodesMessage<P> {
            fn read_key(&self, key_name: &str) -> Result<$gen_type, CodesError> {
                let message = self.lock();
                let key = message.key(key_name);
                match key.value_type()? {
                    $key_type => (),
                    _ => return Err(CodesError::WrongRequestedKeyType),
                }
                let key_size = key.len()?;
                key_size_check!($key_sizing, key_size);
                Ok(message.get(key_name)?)
            }

            fn read_key_unchecked(&self, key_name: &str) -> Result<$gen_type, CodesError> {
                Ok(self.lock().get(key_name)?)
            }
        }
    };
}

impl_key_read!(scalar, KeyType::I64, i64);
impl_key_read!(scalar, KeyType::F64, f32);
impl_key_read!(scalar, KeyType::F64, f64);
impl_key_read!(array, KeyType::String, String);
impl_key_read!(array, KeyType::Bytes, Vec<u8>);
impl_key_read!(array, KeyType::I64, Vec<i64>);
impl_key_read!(array, KeyType::F64, Vec<f32>);
impl_key_read!(array, KeyType::F64, Vec<f64>);

/// A key value with its type known only at runtime.
#[derive(Clone, Debug, PartialEq)]
pub enum DynamicKeyType {
    #[allow(missing_docs)]
    Float(f64),
    #[allow(missing_docs)]
    Int(i64),
    #[allow(missing_docs)]
    FloatArray(Vec<f64>),
    #[allow(missing_docs)]
    IntArray(Vec<i64>),
    #[allow(missing_docs)]
    Str(String),
    #[allow(missing_docs)]
    Bytes(Vec<u8>),
}

impl<P: Debug> CodesMessage<P> {
    /// Reads the key in its native type, falling back to bytes when that fails.
    #[allow(clippy::significant_drop_tightening)] // guard spans the fallback read
    pub fn read_key_dynamic(&self, key_name: &str) -> Result<DynamicKeyType, CodesError> {
        let message = self.lock();
        let key = message.key(key_name);
        let key_type = key.value_type()?;
        let key_size = key.len()?;

        let read = match key_type {
            KeyType::I64 => {
                if key_size == 1 {
                    message.get(key_name).map(DynamicKeyType::Int)
                } else if key_size >= 2 {
                    message.get(key_name).map(DynamicKeyType::IntArray)
                } else {
                    return Err(CodesError::IncorrectKeySize);
                }
            }
            KeyType::F64 => {
                if key_size == 1 {
                    message.get(key_name).map(DynamicKeyType::Float)
                } else if key_size >= 2 {
                    message.get(key_name).map(DynamicKeyType::FloatArray)
                } else {
                    return Err(CodesError::IncorrectKeySize);
                }
            }
            KeyType::Bytes => message.get(key_name).map(DynamicKeyType::Bytes),
            KeyType::Missing => return Err(CodesError::MissingKey),
            _ => message.get(key_name).map(DynamicKeyType::Str),
        };

        read.or_else(|_| message.get(key_name).map(DynamicKeyType::Bytes))
            .map_err(CodesError::from)
    }

    /// Writes this message to `file_path`; `append` adds instead of replacing.
    #[allow(clippy::significant_drop_tightening)] // written bytes borrow the guard
    pub fn write_to_file<Q: AsRef<Path>>(
        &self,
        file_path: Q,
        append: bool,
    ) -> Result<(), CodesError> {
        let message = self.lock();
        let buf = message.as_bytes()?;
        let mut file = OpenOptions::new()
            .write(true)
            .create(true)
            .append(append)
            .open(file_path)?;
        file.write_all(buf)?;
        Ok(())
    }

    /// Clones this message into an independent, editable [`BufMessage`].
    ///
    /// Reads the whole message into memory — mind the size of large grids.
    pub fn try_clone(&self) -> Result<BufMessage, CodesError> {
        // The only C-side failure mode of a handle clone is a null result.
        let inner = self
            .lock()
            .try_clone()
            .map_err(|_| CodesError::CloneFailed)?;
        Ok(BufMessage::new(inner))
    }
}

/// GRIB key writing; implemented by [`BufMessage`] only.
pub trait KeyWrite<T> {
    /// Overwrites the key with `value`, with no checks on the Rust side.
    fn write_key_unchecked(&mut self, name: &str, value: T) -> Result<&mut Self, CodesError>;
}

macro_rules! impl_key_write {
    ($gen_type:ty) => {
        impl KeyWrite<$gen_type> for BufMessage {
            fn write_key_unchecked(
                &mut self,
                name: &str,
                value: $gen_type,
            ) -> Result<&mut Self, CodesError> {
                self.lock_mut().set(name, value)?;
                Ok(self)
            }
        }
    };
}

impl_key_write!(i64);
impl_key_write!(f64);
impl_key_write!(&[i64]);
impl_key_write!(&[f64]);
impl_key_write!(&[u8]);
impl_key_write!(&str);

#[cfg(feature = "ndarray")]
mod ndarray_convert {
    use super::{CodesMessage, KeyRead};
    use crate::errors::{CodesError, MessageNdarrayError};
    use ndarray::{Array2, Array3, s};
    use num_traits::Float;
    use std::fmt::Debug;

    /// Collocated coordinate and value arrays returned by
    /// [`to_lons_lats_values()`](CodesMessage::to_lons_lats_values).
    #[derive(Clone, PartialEq, Debug, Default)]
    pub struct RustyCodesMessage {
        /// Longitudes in degrees.
        pub longitudes: Array2<f64>,
        /// Latitudes in degrees.
        pub latitudes: Array2<f64>,
        /// Values in native GRIB units.
        pub values: Array2<f64>,
    }

    impl<P: Debug> CodesMessage<P> {
        /// The message's values as a 2D `[lat, lon]` array.
        pub fn to_ndarray<T>(&self) -> Result<Array2<T>, CodesError>
        where
            T: Float,
            Self: KeyRead<Vec<T>> + KeyRead<i64>,
        {
            let ni: i64 = self.read_key("Ni")?;
            let ni = usize::try_from(ni).map_err(MessageNdarrayError::from)?;

            let nj: i64 = self.read_key("Nj")?;
            let nj = usize::try_from(nj).map_err(MessageNdarrayError::from)?;

            let vals: Vec<T> = self.read_key("values")?;

            let expected_vals_len = ni.checked_mul(nj).ok_or(CodesError::TooMuchValues)?;
            if vals.len() != expected_vals_len {
                return Err(MessageNdarrayError::UnexpectedValuesLength(
                    vals.len(),
                    expected_vals_len,
                )
                .into());
            }

            let j_scanning: i64 = self.read_key("jPointsAreConsecutive")?;
            if ![0, 1].contains(&j_scanning) {
                return Err(MessageNdarrayError::UnexpectedKeyValue(
                    "jPointsAreConsecutive".to_owned(),
                )
                .into());
            }
            let j_scanning = j_scanning != 0;

            let shape = if j_scanning { (ni, nj) } else { (nj, ni) };
            let vals = Array2::from_shape_vec(shape, vals).map_err(MessageNdarrayError::from)?;

            if j_scanning {
                Ok(vals.reversed_axes())
            } else {
                Ok(vals)
            }
        }

        /// Like [`to_ndarray()`](CodesMessage::to_ndarray), with the
        /// longitudes and latitudes alongside the values.
        pub fn to_lons_lats_values(&self) -> Result<RustyCodesMessage, CodesError> {
            let ni: i64 = self.read_key("Ni")?;
            let ni = usize::try_from(ni).map_err(MessageNdarrayError::from)?;

            let nj: i64 = self.read_key("Nj")?;
            let nj = usize::try_from(nj).map_err(MessageNdarrayError::from)?;

            let latlonvals: Vec<f64> = self.read_key("latLonValues")?;

            let expected_vals_len = ni
                .checked_mul(nj)
                .ok_or(CodesError::TooMuchValues)?
                .checked_mul(3)
                .ok_or(CodesError::TooMuchValues)?;
            if latlonvals.len() != expected_vals_len {
                return Err(MessageNdarrayError::UnexpectedValuesLength(
                    latlonvals.len(),
                    expected_vals_len,
                )
                .into());
            }

            let j_scanning: i64 = self.read_key("jPointsAreConsecutive")?;
            if ![0, 1].contains(&j_scanning) {
                return Err(MessageNdarrayError::UnexpectedKeyValue(
                    "jPointsAreConsecutive".to_owned(),
                )
                .into());
            }
            let j_scanning = j_scanning != 0;

            let shape = if j_scanning {
                (ni, nj, 3_usize)
            } else {
                (nj, ni, 3_usize)
            };

            let mut latlonvals =
                Array3::from_shape_vec(shape, latlonvals).map_err(MessageNdarrayError::from)?;

            if j_scanning {
                latlonvals.swap_axes(0, 1);
            }

            let (lats, lons, vals) = latlonvals.view_mut().multi_slice_move((
                s![.., .., 0],
                s![.., .., 1],
                s![.., .., 2],
            ));

            Ok(RustyCodesMessage {
                longitudes: lons.into_owned(),
                latitudes: lats.into_owned(),
                values: vals.into_owned(),
            })
        }
    }
}

#[cfg(feature = "ndarray")]
pub use ndarray_convert::RustyCodesMessage;
