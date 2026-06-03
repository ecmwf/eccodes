//! Nearest-neighbour search for GRIB messages.
//!
//! Requires `features = ["advanced"]`.

use crate::error::{EcCodesError, Result};
use crate::message::Message;
use std::marker::PhantomData;
use std::os::raw::c_int;

/// A nearest-neighbour result point.
#[derive(Debug, Clone, Copy)]
pub struct NearestPoint {
    pub lat: f64,
    pub lon: f64,
    pub value: f64,
    pub distance_km: f64,
    pub index: i32,
}

/// Nearest-neighbour search object for a GRIB message.
///
/// The lifetime `'a` ties this object to the `Message` it was created from,
/// ensuring the handle remains valid.
pub struct Nearest<'a> {
    nearest: *mut eccodes_sys::codes_nearest,
    handle: *const eccodes_sys::codes_handle,
    _marker: PhantomData<&'a Message>,
}

impl<'a> Nearest<'a> {
    pub(crate) fn new(message: &'a Message) -> Result<Self> {
        let mut err: c_int = 0;
        let nearest = unsafe {
            eccodes_sys::codes_grib_nearest_new(message.as_ptr(), &mut err)
        };
        EcCodesError::check(err)?;
        if nearest.is_null() {
            return Err(EcCodesError::InternalError);
        }
        Ok(Nearest {
            nearest,
            handle: message.as_ptr(),
            _marker: PhantomData,
        })
    }

    /// Find the 4 nearest points to a given lat/lon.
    pub fn find(&mut self, lat: f64, lon: f64) -> Result<[NearestPoint; 4]> {
        let mut outlats = [0.0f64; 4];
        let mut outlons = [0.0f64; 4];
        let mut values = [0.0f64; 4];
        let mut distances = [0.0f64; 4];
        let mut indexes = [0i32; 4];
        let mut len: usize = 4;
        let err = unsafe {
            eccodes_sys::codes_grib_nearest_find(
                self.nearest,
                self.handle,
                lat, lon,
                0, // flags
                outlats.as_mut_ptr(),
                outlons.as_mut_ptr(),
                values.as_mut_ptr(),
                distances.as_mut_ptr(),
                indexes.as_mut_ptr() as *mut c_int,
                &mut len,
            )
        };
        EcCodesError::check(err)?;
        Ok(std::array::from_fn(|i| NearestPoint {
            lat: outlats[i],
            lon: outlons[i],
            value: values[i],
            distance_km: distances[i],
            index: indexes[i],
        }))
    }
}

impl<'a> Drop for Nearest<'a> {
    fn drop(&mut self) {
        if !self.nearest.is_null() {
            unsafe { eccodes_sys::codes_grib_nearest_delete(self.nearest); }
        }
    }
}
