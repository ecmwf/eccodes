//! Nearest-grid-point search on a GRIB message.
//!
//! ```no_run
//! use eccodes::LatLon;
//!
//! # fn main() -> eccodes::Result<()> {
//! # let message: eccodes::GribMessage = unimplemented!();
//! let mut nearest = message.nearest()?;
//!
//! for point in nearest.find(LatLon::new(51.5, -0.1))? {
//!     println!("{:.1} km away: {}", point.distance_km, point.value);
//! }
//!
//! let stations = [LatLon::new(51.5, -0.1), LatLon::new(48.9, 2.4)];
//! for point in nearest.find_each(&stations)? {
//!     println!("{:?} -> {}", point.position, point.value);
//! }
//! # Ok(())
//! # }
//! ```

use std::ffi::{c_int, c_ulong};
use std::fmt;
use std::ptr::NonNull;

use eccodes_sys as sys;

use crate::error::{Code, Error, Result, check};
use crate::ffi;
use crate::grid::LatLon;
use crate::message::GribMessage;

bitflags::bitflags! {
    /// What has not changed since the previous search, so the library can
    /// skip repeating that work (`CODES_NEAREST_*`).
    ///
    /// These are promises, not preferences: claiming `SAME_GRID` for a
    /// message with a different geometry gives wrong answers, not an error.
    #[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
    pub struct Reuse: u32 {
        /// The message's geometry is the one used last time.
        const SAME_GRID = sys::CODES_NEAREST_SAME_GRID;
        /// The message's values are the ones used last time.
        const SAME_DATA = sys::CODES_NEAREST_SAME_DATA;
        /// The query point is the one used last time.
        const SAME_POINT = sys::CODES_NEAREST_SAME_POINT;
    }
}

/// A grid point found by a search.
#[derive(Debug, Clone, Copy, PartialEq, PartialOrd)]
pub struct NearestPoint {
    /// Where the grid point is.
    pub position: LatLon,
    /// The field's value there.
    pub value: f64,
    /// How far it is from the point that was asked for, in kilometres.
    pub distance_km: f64,
    /// Its index in the field's `values`.
    pub index: usize,
}

impl GribMessage {
    /// Prepare a nearest-point search over this message's geometry.
    ///
    /// Reuse the returned [`Nearest`] across queries on the same message: it
    /// keeps the geometry it has already worked out.
    pub fn nearest(&self) -> Result<Nearest<'_>> {
        let mut status = 0;
        // SAFETY: valid handle; the search object borrows it, and the return
        // type ties that borrow to the message's lifetime.
        let raw = unsafe { sys::codes_grib_nearest_new(self.as_ptr(), &raw mut status) };
        Error::from_raw(status)?;
        NonNull::new(raw)
            .map(|raw| Nearest { raw, message: self })
            .ok_or_else(|| Error::from(Code::InvalidNearest))
    }
}

/// A nearest-point search over one message — see [`GribMessage::nearest`].
pub struct Nearest<'m> {
    raw: NonNull<sys::codes_nearest>,
    message: &'m GribMessage,
}

impl Nearest<'_> {
    /// The four grid points surrounding `point`, unordered.
    pub fn find(&mut self, point: impl Into<LatLon>) -> Result<[NearestPoint; 4]> {
        self.find_reusing(point, Reuse::empty())
    }

    /// The four grid points surrounding `point`, telling the library what has
    /// not changed since the previous call.
    pub fn find_reusing(
        &mut self,
        point: impl Into<LatLon>,
        unchanged: Reuse,
    ) -> Result<[NearestPoint; 4]> {
        let point = point.into();
        let mut lats = [0.0_f64; 4];
        let mut lons = [0.0_f64; 4];
        let mut values = [0.0_f64; 4];
        let mut distances = [0.0_f64; 4];
        let mut indexes = [0 as c_int; 4];
        let mut found: usize = 4;
        check!(sys::codes_grib_nearest_find(
            self.raw.as_ptr(),
            self.message.as_ptr(),
            point.lat,
            point.lon,
            c_ulong::from(unchanged.bits()),
            lats.as_mut_ptr(),
            lons.as_mut_ptr(),
            values.as_mut_ptr(),
            distances.as_mut_ptr(),
            indexes.as_mut_ptr(),
            &raw mut found,
        ))?;

        let mut points = [NearestPoint {
            position: LatLon::new(0.0, 0.0),
            value: 0.0,
            distance_km: 0.0,
            index: 0,
        }; 4];
        for (slot, index) in points.iter_mut().zip(0..4) {
            *slot = NearestPoint {
                position: LatLon::new(lats[index], lons[index]),
                value: values[index],
                distance_km: distances[index],
                index: ffi::to_usize(indexes[index])?,
            };
        }
        Ok(points)
    }

    /// The single nearest grid point to each of `points`.
    pub fn find_each(&mut self, points: &[LatLon]) -> Result<Vec<NearestPoint>> {
        self.find_multiple(points, false)
    }

    /// The single nearest *land* point to each of `points`, treating this
    /// message as a land-sea mask.
    ///
    /// Points where the mask reads below `0.5` are skipped in favour of the
    /// nearest one above it.
    pub fn find_each_land(&mut self, points: &[LatLon]) -> Result<Vec<NearestPoint>> {
        self.find_multiple(points, true)
    }

    fn find_multiple(
        &mut self,
        points: &[LatLon],
        is_land_sea_mask: bool,
    ) -> Result<Vec<NearestPoint>> {
        let count = points.len();
        let lats: Vec<f64> = points.iter().map(|point| point.lat).collect();
        let lons: Vec<f64> = points.iter().map(|point| point.lon).collect();
        let mut out_lats = vec![0.0_f64; count];
        let mut out_lons = vec![0.0_f64; count];
        let mut values = vec![0.0_f64; count];
        let mut distances = vec![0.0_f64; count];
        let mut indexes = vec![0 as c_int; count];
        check!(sys::codes_grib_nearest_find_multiple(
            self.message.as_ptr(),
            c_int::from(is_land_sea_mask),
            lats.as_ptr(),
            lons.as_ptr(),
            ffi::to_c_long(count)?,
            out_lats.as_mut_ptr(),
            out_lons.as_mut_ptr(),
            values.as_mut_ptr(),
            distances.as_mut_ptr(),
            indexes.as_mut_ptr(),
        ))?;

        (0..count)
            .map(|i| {
                Ok(NearestPoint {
                    position: LatLon::new(out_lats[i], out_lons[i]),
                    value: values[i],
                    distance_km: distances[i],
                    index: ffi::to_usize(indexes[i])?,
                })
            })
            .collect()
    }
}

impl Drop for Nearest<'_> {
    fn drop(&mut self) {
        // SAFETY: a valid search object owned by us, freed exactly once.
        unsafe { sys::codes_grib_nearest_delete(self.raw.as_ptr()) };
    }
}

impl fmt::Debug for Nearest<'_> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("Nearest").finish_non_exhaustive()
    }
}
