//! `CodesNearest` over the official crate's [`Nearest`](eccodes::Nearest) search.

use std::fmt::Debug;

use eccodes::LatLon;

use crate::codes_message::CodesMessage;
use crate::errors::CodesError;

/// Finds nearest gridpoints in a [`CodesMessage`].
#[derive(Debug)]
pub struct CodesNearest<'a, P: Debug> {
    // The official search object borrows the message, which sits behind the
    // compat mutex; it is therefore rebuilt per find_nearest() call.
    parent_message: &'a CodesMessage<P>,
}

/// One gridpoint returned by [`CodesNearest::find_nearest()`].
#[derive(Copy, Clone, PartialEq, Debug, Default)]
pub struct NearestGridpoint {
    /// Index of this gridpoint.
    pub index: i32,
    /// Latitude of this gridpoint in degrees north.
    pub lat: f64,
    /// Longitude of this gridpoint in degrees east.
    pub lon: f64,
    /// Distance from the requested point in kilometers.
    pub distance: f64,
    /// Value of the message's parameter at this gridpoint.
    pub value: f64,
}

impl<P: Debug> CodesMessage<P> {
    /// A [`CodesNearest`] for this message.
    pub const fn codes_nearest(&self) -> Result<CodesNearest<'_, P>, CodesError> {
        Ok(CodesNearest {
            parent_message: self,
        })
    }
}

impl<P: Debug> CodesNearest<'_, P> {
    /// The four gridpoints nearest to the given coordinates, in degrees
    /// north and degrees east.
    #[allow(clippy::significant_drop_tightening)] // guard spans the whole search
    pub fn find_nearest(
        &mut self,
        lat: f64,
        lon: f64,
    ) -> Result<[NearestGridpoint; 4], CodesError> {
        let message = self.parent_message.lock();
        let found = message.nearest()?.find(LatLon::new(lat, lon))?;

        let mut output_points = [NearestGridpoint::default(); 4];
        for (out, point) in output_points.iter_mut().zip(found) {
            *out = NearestGridpoint {
                index: i32::try_from(point.index).map_err(|_| CodesError::NearestFindFailed)?,
                lat: point.position.lat,
                lon: point.position.lon,
                distance: point.distance_km,
                value: point.value,
            };
        }
        Ok(output_points)
    }
}
