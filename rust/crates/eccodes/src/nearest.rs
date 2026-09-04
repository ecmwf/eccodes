//! `Nearest` — nearest-grid-point search on a GRIB message.

use std::ffi::{c_int, c_long, c_ulong};
use std::ptr::NonNull;

use eccodes_sys as sys;

use crate::error::{Error, Result, check};
use crate::handle::Handle;
use crate::kind::Grib;

bitflags::bitflags! {
    /// Speed-up flags for [`Nearest::find`] (`CODES_NEAREST_*`).
    ///
    /// Promise that the grid, data or query point is unchanged between calls
    /// so the search can reuse its previous state.
    #[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
    pub struct NearestFlags: u32 {
        /// The grid is the same as in the previous call.
        const SAME_GRID = sys::CODES_NEAREST_SAME_GRID;
        /// The data is the same as in the previous call.
        const SAME_DATA = sys::CODES_NEAREST_SAME_DATA;
        /// The query point is the same as in the previous call.
        const SAME_POINT = sys::CODES_NEAREST_SAME_POINT;
    }
}

/// One point returned by a nearest-neighbour search.
#[derive(Debug, Clone, Copy, PartialEq)]
pub struct NearestPoint {
    /// Latitude in degrees.
    pub lat: f64,
    /// Longitude in degrees.
    pub lon: f64,
    /// Data value at this point.
    pub value: f64,
    /// Distance from the query point in kilometres.
    pub distance_km: f64,
    /// Zero-based index of this point in the `values` array.
    pub index: usize,
}

/// Nearest-neighbour search object — see [`Handle::nearest`].
pub struct Nearest<'h> {
    raw: NonNull<sys::codes_nearest>,
    handle: &'h Handle<Grib>,
}

impl Handle<Grib> {
    /// Create a nearest-neighbour search using this message's geometry
    /// (`codes_grib_nearest_new`).
    pub fn nearest(&self) -> Result<Nearest<'_>> {
        let mut err = 0;
        // SAFETY: valid handle; the search object borrows it for `'h`.
        let raw = unsafe { sys::codes_grib_nearest_new(self.as_sys(), &raw mut err) };
        Error::from_code(err)?;
        NonNull::new(raw)
            .map(|raw| Nearest { raw, handle: self })
            .ok_or(Error::InvalidNearest)
    }

    /// For each `(lats[i], lons[i])` query point, the single nearest grid
    /// point (`codes_grib_nearest_find_multiple`).
    ///
    /// With `is_lsm` this message is treated as a land-sea mask and the
    /// nearest *land* point (mask value >= 0.5) is preferred.
    pub fn find_nearest_multiple(
        &self,
        lats: &[f64],
        lons: &[f64],
        is_lsm: bool,
    ) -> Result<Vec<NearestPoint>> {
        if lats.len() != lons.len() {
            return Err(Error::WrongArraySize);
        }
        let n = lats.len();
        let mut out_lats = vec![0.0_f64; n];
        let mut out_lons = vec![0.0_f64; n];
        let mut values = vec![0.0_f64; n];
        let mut distances = vec![0.0_f64; n];
        let mut indexes = vec![0 as c_int; n];
        check!(sys::codes_grib_nearest_find_multiple(
            self.as_sys(),
            i32::from(is_lsm),
            lats.as_ptr(),
            lons.as_ptr(),
            c_long::try_from(n).map_err(|_| Error::InvalidArgument)?,
            out_lats.as_mut_ptr(),
            out_lons.as_mut_ptr(),
            values.as_mut_ptr(),
            distances.as_mut_ptr(),
            indexes.as_mut_ptr(),
        ))?;
        (0..n)
            .map(|i| {
                Ok(NearestPoint {
                    lat: out_lats[i],
                    lon: out_lons[i],
                    value: values[i],
                    distance_km: distances[i],
                    index: usize::try_from(indexes[i]).map_err(|_| Error::InternalError)?,
                })
            })
            .collect()
    }
}

impl Nearest<'_> {
    /// The four grid points nearest to `(lat, lon)`, unordered
    /// (`codes_grib_nearest_find`).
    pub fn find(&mut self, lat: f64, lon: f64, flags: NearestFlags) -> Result<[NearestPoint; 4]> {
        let mut lats = [0.0_f64; 4];
        let mut lons = [0.0_f64; 4];
        let mut values = [0.0_f64; 4];
        let mut distances = [0.0_f64; 4];
        let mut indexes = [0 as c_int; 4];
        let mut len: usize = 4;
        check!(sys::codes_grib_nearest_find(
            self.raw.as_ptr(),
            self.handle.as_sys(),
            lat,
            lon,
            c_ulong::from(flags.bits()),
            lats.as_mut_ptr(),
            lons.as_mut_ptr(),
            values.as_mut_ptr(),
            distances.as_mut_ptr(),
            indexes.as_mut_ptr(),
            &raw mut len,
        ))?;
        let mut points = [NearestPoint {
            lat: 0.0,
            lon: 0.0,
            value: 0.0,
            distance_km: 0.0,
            index: 0,
        }; 4];
        for (i, point) in points.iter_mut().enumerate() {
            *point = NearestPoint {
                lat: lats[i],
                lon: lons[i],
                value: values[i],
                distance_km: distances[i],
                index: usize::try_from(indexes[i]).map_err(|_| Error::InternalError)?,
            };
        }
        Ok(points)
    }
}

impl Drop for Nearest<'_> {
    fn drop(&mut self) {
        // SAFETY: `raw` is a valid nearest object owned by us; freed exactly
        // once.
        unsafe { sys::codes_grib_nearest_delete(self.raw.as_ptr()) };
    }
}
