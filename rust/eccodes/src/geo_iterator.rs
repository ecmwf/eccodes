//! Geographic iterator — iterate over lat/lon/value triplets in a GRIB message.
//!
//! Requires `features = ["advanced"]`.

use crate::error::{EcCodesError, Result};
use crate::message::Message;
use std::marker::PhantomData;
use std::os::raw::c_int;

/// A geographic point with latitude, longitude and data value.
#[derive(Debug, Clone, Copy)]
pub struct GeoPoint {
    pub lat: f64,
    pub lon: f64,
    pub value: f64,
}

/// Iterator over geographic points in a GRIB message.
pub struct GeoIterator<'a> {
    iter: *mut eccodes_sys::codes_iterator,
    _marker: PhantomData<&'a Message>,
}

impl<'a> GeoIterator<'a> {
    pub(crate) fn new(message: &'a Message) -> Result<Self> {
        let mut err: c_int = 0;
        let iter = unsafe {
            eccodes_sys::codes_grib_iterator_new(message.as_ptr(), 0, &mut err)
        };
        EcCodesError::check(err)?;
        if iter.is_null() {
            return Err(EcCodesError::InternalError);
        }
        Ok(GeoIterator {
            iter,
            _marker: PhantomData,
        })
    }

    /// Reset the iterator to the beginning.
    pub fn reset(&mut self) -> Result<()> {
        let err = unsafe { eccodes_sys::codes_grib_iterator_reset(self.iter) };
        EcCodesError::check(err)
    }

    /// Move to the previous point.
    pub fn previous(&mut self) -> Option<GeoPoint> {
        let mut lat = 0.0f64;
        let mut lon = 0.0f64;
        let mut value = 0.0f64;
        let ret = unsafe {
            eccodes_sys::codes_grib_iterator_previous(self.iter, &mut lat, &mut lon, &mut value)
        };
        if ret > 0 {
            Some(GeoPoint { lat, lon, value })
        } else {
            None
        }
    }
}

impl<'a> Iterator for GeoIterator<'a> {
    type Item = GeoPoint;

    fn next(&mut self) -> Option<Self::Item> {
        let mut lat = 0.0f64;
        let mut lon = 0.0f64;
        let mut value = 0.0f64;
        let ret = unsafe {
            eccodes_sys::codes_grib_iterator_next(self.iter, &mut lat, &mut lon, &mut value)
        };
        if ret > 0 {
            Some(GeoPoint { lat, lon, value })
        } else {
            None
        }
    }
}

impl<'a> Drop for GeoIterator<'a> {
    fn drop(&mut self) {
        if !self.iter.is_null() {
            unsafe {
                eccodes_sys::codes_grib_iterator_delete(self.iter);
            }
        }
    }
}
