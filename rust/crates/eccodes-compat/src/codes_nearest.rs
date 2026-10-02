//! `CodesNearest` over the official crate's [`Nearest`] search.

use std::fmt::{self, Debug};

use eccodes::{LatLon, Nearest, NearestPoint};

use crate::codes_message::CodesMessage;
use crate::errors::CodesError;

/// Finds nearest gridpoints in a [`CodesMessage`].
pub struct CodesNearest<'a, P: Debug> {
    inner: Search<'a, P>,
}

enum Search<'a, P: Debug> {
    /// `RefMessage`: one official search object lives across calls, keeping
    /// the C-side geometry cache.
    Cached(Nearest<'a>),
    /// Locked messages: the search cannot borrow through the mutex, so it
    /// is rebuilt per call.
    PerCall(&'a CodesMessage<P>),
}

impl<P: Debug> Debug for CodesNearest<'_, P> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("CodesNearest").finish_non_exhaustive()
    }
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
    pub fn codes_nearest(&self) -> Result<CodesNearest<'_, P>, CodesError> {
        let inner = match self.direct() {
            Some(message) => Search::Cached(message.nearest()?),
            None => Search::PerCall(self),
        };
        Ok(CodesNearest { inner })
    }
}

impl<P: Debug> CodesNearest<'_, P> {
    /// The four gridpoints nearest to the given coordinates, in degrees
    /// north and degrees east.
    pub fn find_nearest(
        &mut self,
        lat: f64,
        lon: f64,
    ) -> Result<[NearestGridpoint; 4], CodesError> {
        let point = LatLon::new(lat, lon);
        let found = match &mut self.inner {
            Search::Cached(nearest) => nearest.find(point)?,
            Search::PerCall(message) => message.read(|m| m.nearest()?.find(point))?,
        };

        let mut output_points = [NearestGridpoint::default(); 4];
        for (out, point) in output_points.iter_mut().zip(found) {
            *out = convert(point)?;
        }
        Ok(output_points)
    }
}

fn convert(point: NearestPoint) -> Result<NearestGridpoint, CodesError> {
    Ok(NearestGridpoint {
        index: i32::try_from(point.index).map_err(|_| CodesError::NearestFindFailed)?,
        lat: point.position.lat,
        lon: point.position.lon,
        distance: point.distance_km,
        value: point.value,
    })
}
