/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


namespace eccodes::geo
{


void PolarStereographic_fillGrib(grib_info& info) const
{
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_POLAR_STEREOGRAPHIC;

    Point2 first   = { x().front(), y().front() };
    Point2 firstLL = grid().projection().lonlat(first);

    info.grid.latitudeOfFirstGridPointInDegrees = firstLL[LLCOORDS::LAT];
    info.grid.longitudeOfFirstGridPointInDegrees =
        writeLonPositive_ ? util::normalise_longitude(firstLL[LLCOORDS::LON], 0) : firstLL[LLCOORDS::LON];

    info.grid.Ni = static_cast<long>(x().size());
    info.grid.Nj = static_cast<long>(y().size());

    info.grid.uvRelativeToGrid = uvRelativeToGrid_ ? 1 : 0;

    info.extra_set("DxInMetres", std::abs(x().step()));
    info.extra_set("DyInMetres", std::abs(y().step()));
    info.extra_set("orientationOfTheGridInDegrees", util::normalise_longitude(orientationOfTheGridInDegrees_, 0));

    if (writeLaDInDegrees_) {
        info.extra_set("LaDInDegrees", LaDInDegrees_);
    }

    // some extra keys are edition-specific, so parent call is here
    RegularGrid_fillGrib(info);
}


}  // namespace eccodes::geo
