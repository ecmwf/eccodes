//! Grid points of a GRIB message — coordinates, values, and iteration.
//!
//! ```no_run
//! # fn main() -> eccodes::Result<()> {
//! # let message: eccodes::GribMessage = unimplemented!();
//! // values alone, in grid order
//! let values = message.values()?;
//!
//! // values with their coordinates
//! for point in message.grid_points()? {
//!     println!("{:.3} {:.3} {}", point.position.lat, point.position.lon, point.value);
//! }
//!
//! // coordinates alone — the library skips decoding the data section
//! for position in message.grid_positions()? {
//!     println!("{:.3} {:.3}", position.lat, position.lon);
//! }
//! # Ok(())
//! # }
//! ```

use std::ffi::c_ulong;
use std::fmt;
use std::marker::PhantomData;
use std::ptr::NonNull;

use eccodes_sys as sys;

use crate::error::{Code, Error, Result, check};
use crate::message::GribMessage;

/// A position on the globe, in degrees.
#[derive(Debug, Clone, Copy, PartialEq, PartialOrd)]
pub struct LatLon {
    /// Latitude, degrees north.
    pub lat: f64,
    /// Longitude, degrees east.
    pub lon: f64,
}

impl LatLon {
    /// A position from latitude and longitude in degrees.
    #[must_use]
    pub const fn new(lat: f64, lon: f64) -> Self {
        Self { lat, lon }
    }
}

impl From<(f64, f64)> for LatLon {
    /// From a `(lat, lon)` pair.
    fn from((lat, lon): (f64, f64)) -> Self {
        Self { lat, lon }
    }
}

/// One grid point: where it is, and what the field holds there.
#[derive(Debug, Clone, Copy, PartialEq, PartialOrd)]
pub struct GeoPoint {
    /// Where the point is.
    pub position: LatLon,
    /// The field's value at that point.
    pub value: f64,
}

impl GribMessage {
    /// The field's values, in grid order.
    pub fn values(&self) -> Result<Vec<f64>> {
        self.get("values")
    }

    /// Replace the field's values.
    pub fn set_values(&mut self, values: &[f64]) -> Result<()> {
        self.set("values", values)
    }

    /// Every grid point with its value, decoded in one call
    /// (`codes_grib_get_data`).
    ///
    /// Allocates the whole field; [`grid_points`](Self::grid_points) streams
    /// the same information point by point.
    pub fn data_points(&self) -> Result<Vec<GeoPoint>> {
        let count = self.key_len("values")?;
        let mut lats = vec![0.0_f64; count];
        let mut lons = vec![0.0_f64; count];
        let mut values = vec![0.0_f64; count];
        check!(sys::codes_grib_get_data(
            self.as_ptr(),
            lats.as_mut_ptr(),
            lons.as_mut_ptr(),
            values.as_mut_ptr(),
        ))?;
        Ok(lats
            .into_iter()
            .zip(lons)
            .zip(values)
            .map(|((lat, lon), value)| GeoPoint {
                position: LatLon { lat, lon },
                value,
            })
            .collect())
    }

    /// Walk the grid points, value included.
    pub fn grid_points(&self) -> Result<GridPoints<'_>> {
        Ok(GridPoints {
            inner: Grid::new(self, false)?,
            _message: PhantomData,
        })
    }

    /// Walk the grid positions, leaving the data section undecoded.
    ///
    /// Faster than [`grid_points`](Self::grid_points) when only geometry
    /// matters — which is why the values are absent from the item type rather
    /// than present and meaningless.
    pub fn grid_positions(&self) -> Result<GridPositions<'_>> {
        Ok(GridPositions {
            inner: Grid::new(self, true)?,
            _message: PhantomData,
        })
    }
}

/// The C grid iterator, shared by both public iterators.
struct Grid {
    raw: NonNull<sys::codes_iterator>,
}

impl Grid {
    fn new(message: &GribMessage, positions_only: bool) -> Result<Self> {
        let flags = if positions_only {
            c_ulong::from(sys::CODES_GEOITERATOR_NO_VALUES)
        } else {
            0
        };
        let mut status = 0;
        // SAFETY: valid handle; the iterator borrows it, and the caller ties
        // that borrow to the message's lifetime.
        let raw = unsafe { sys::codes_grib_iterator_new(message.as_ptr(), flags, &raw mut status) };
        Error::from_raw(status)?;
        NonNull::new(raw)
            .map(|raw| Self { raw })
            .ok_or_else(|| Error::from(Code::InvalidIterator))
    }

    fn step(&mut self) -> Option<GeoPoint> {
        let (mut lat, mut lon, mut value) = (0.0_f64, 0.0_f64, 0.0_f64);
        // SAFETY: valid iterator, out-pointers to locals; returns non-zero
        // while a point remains.
        let more = unsafe {
            sys::codes_grib_iterator_next(
                self.raw.as_ptr(),
                &raw mut lat,
                &raw mut lon,
                &raw mut value,
            )
        };
        (more != 0).then_some(GeoPoint {
            position: LatLon { lat, lon },
            value,
        })
    }

    fn step_back(&mut self) -> Option<GeoPoint> {
        let (mut lat, mut lon, mut value) = (0.0_f64, 0.0_f64, 0.0_f64);
        // SAFETY: as for `step`.
        let more = unsafe {
            sys::codes_grib_iterator_previous(
                self.raw.as_ptr(),
                &raw mut lat,
                &raw mut lon,
                &raw mut value,
            )
        };
        (more != 0).then_some(GeoPoint {
            position: LatLon { lat, lon },
            value,
        })
    }

    fn has_next(&self) -> bool {
        // SAFETY: valid iterator.
        unsafe { sys::codes_grib_iterator_has_next(self.raw.as_ptr()) != 0 }
    }

    fn reset(&mut self) -> Result<()> {
        check!(sys::codes_grib_iterator_reset(self.raw.as_ptr()))
    }
}

impl Drop for Grid {
    fn drop(&mut self) {
        // SAFETY: a valid iterator owned by us, freed exactly once.
        unsafe { sys::codes_grib_iterator_delete(self.raw.as_ptr()) };
    }
}

/// Grid points with their values — see [`GribMessage::grid_points`].
pub struct GridPoints<'m> {
    inner: Grid,
    _message: PhantomData<&'m GribMessage>,
}

impl GridPoints<'_> {
    /// Step back to the previous point.
    pub fn previous(&mut self) -> Option<GeoPoint> {
        self.inner.step_back()
    }

    /// Whether another point remains.
    #[must_use]
    pub fn has_next(&self) -> bool {
        self.inner.has_next()
    }

    /// Start again from the first point.
    pub fn reset(&mut self) -> Result<()> {
        self.inner.reset()
    }
}

impl Iterator for GridPoints<'_> {
    type Item = GeoPoint;

    fn next(&mut self) -> Option<GeoPoint> {
        self.inner.step()
    }
}

impl fmt::Debug for GridPoints<'_> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("GridPoints").finish_non_exhaustive()
    }
}

/// Grid positions, without values — see [`GribMessage::grid_positions`].
pub struct GridPositions<'m> {
    inner: Grid,
    _message: PhantomData<&'m GribMessage>,
}

impl GridPositions<'_> {
    /// Step back to the previous position.
    pub fn previous(&mut self) -> Option<LatLon> {
        self.inner.step_back().map(|point| point.position)
    }

    /// Whether another position remains.
    #[must_use]
    pub fn has_next(&self) -> bool {
        self.inner.has_next()
    }

    /// Start again from the first position.
    pub fn reset(&mut self) -> Result<()> {
        self.inner.reset()
    }
}

impl Iterator for GridPositions<'_> {
    type Item = LatLon;

    fn next(&mut self) -> Option<LatLon> {
        self.inner.step().map(|point| point.position)
    }
}

impl fmt::Debug for GridPositions<'_> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("GridPositions").finish_non_exhaustive()
    }
}
