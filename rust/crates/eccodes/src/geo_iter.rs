//! `GeoIterator` — iteration over the grid points of a GRIB message.

use std::ffi::c_ulong;
use std::marker::PhantomData;
use std::ptr::NonNull;

use eccodes_sys as sys;

use crate::error::{Error, Result, check};
use crate::handle::Handle;
use crate::kind::Grib;

bitflags::bitflags! {
    /// Flags for [`GeoIterator`] (`CODES_GEOITERATOR_*`).
    #[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
    pub struct GeoFlags: u32 {
        /// Skip decoding the data values (positions only, faster).
        const NO_VALUES = sys::CODES_GEOITERATOR_NO_VALUES;
    }
}

/// One grid point yielded by [`GeoIterator`].
#[derive(Debug, Clone, Copy, PartialEq)]
pub struct GeoPoint {
    /// Latitude in degrees.
    pub lat: f64,
    /// Longitude in degrees.
    pub lon: f64,
    /// Data value at this point (0.0 with [`GeoFlags::NO_VALUES`]).
    pub value: f64,
}

/// Iterator over the grid points of a GRIB message — see
/// [`Handle::geo_iter`].
pub struct GeoIterator<'h> {
    raw: NonNull<sys::codes_iterator>,
    _handle: PhantomData<&'h Handle<Grib>>,
}

impl Handle<Grib> {
    /// Iterate over the grid points of this message
    /// (`codes_grib_iterator_new`).
    pub fn geo_iter(&self, flags: GeoFlags) -> Result<GeoIterator<'_>> {
        let mut err = 0;
        // SAFETY: valid handle; the iterator borrows it for `'h`.
        let raw = unsafe {
            sys::codes_grib_iterator_new(self.as_sys(), c_ulong::from(flags.bits()), &raw mut err)
        };
        Error::from_code(err)?;
        NonNull::new(raw)
            .map(|raw| GeoIterator {
                raw,
                _handle: PhantomData,
            })
            .ok_or(Error::InvalidIterator)
    }
}

impl GeoIterator<'_> {
    /// Step back to the previous grid point (`codes_grib_iterator_previous`).
    pub fn previous(&mut self) -> Option<GeoPoint> {
        let mut point = GeoPoint {
            lat: 0.0,
            lon: 0.0,
            value: 0.0,
        };
        // SAFETY: valid iterator with out-pointers to locals; returns
        // non-zero while a point is available.
        let more = unsafe {
            sys::codes_grib_iterator_previous(
                self.raw.as_ptr(),
                &raw mut point.lat,
                &raw mut point.lon,
                &raw mut point.value,
            )
        };
        (more != 0).then_some(point)
    }

    /// Whether another grid point is available
    /// (`codes_grib_iterator_has_next`).
    #[must_use]
    pub fn has_next(&self) -> bool {
        // SAFETY: valid iterator.
        let more = unsafe { sys::codes_grib_iterator_has_next(self.raw.as_ptr()) };
        more != 0
    }

    /// Restart the iteration (`codes_grib_iterator_reset`).
    pub fn reset(&mut self) -> Result<()> {
        check!(sys::codes_grib_iterator_reset(self.raw.as_ptr()))
    }
}

impl Drop for GeoIterator<'_> {
    fn drop(&mut self) {
        // SAFETY: `raw` is a valid iterator owned by us; freed exactly once.
        unsafe { sys::codes_grib_iterator_delete(self.raw.as_ptr()) };
    }
}

impl Iterator for GeoIterator<'_> {
    type Item = GeoPoint;

    fn next(&mut self) -> Option<Self::Item> {
        let mut point = GeoPoint {
            lat: 0.0,
            lon: 0.0,
            value: 0.0,
        };
        // SAFETY: valid iterator with out-pointers to locals; returns
        // non-zero while a point is available.
        let more = unsafe {
            sys::codes_grib_iterator_next(
                self.raw.as_ptr(),
                &raw mut point.lat,
                &raw mut point.lon,
                &raw mut point.value,
            )
        };
        (more != 0).then_some(point)
    }
}
