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


void LambertAzimuthalEqualArea_fillGrib(grib_info& info) const
{
    info.grid.grid_type        = CODES_UTIL_GRID_SPEC_LAMBERT_AZIMUTHAL_EQUAL_AREA;
    info.packing.editionNumber = 2;

    Point2 reference = grid().projection().lonlat({ 0., 0. });
    Point2 firstLL   = grid().projection().lonlat({ x().front(), y().front() });

    info.grid.Ni = static_cast<long>(x().size());
    info.grid.Nj = static_cast<long>(y().size());

    info.grid.latitudeOfFirstGridPointInDegrees  = firstLL[LLCOORDS::LAT];
    info.grid.longitudeOfFirstGridPointInDegrees = firstLL[LLCOORDS::LON];

    info.extra_set("DxInMetres", std::abs(x().step()));
    info.extra_set("DyInMetres", std::abs(y().step()));
    info.extra_set("standardParallelInDegrees", reference[LLCOORDS::LAT]);
    info.extra_set("centralLongitudeInDegrees", reference[LLCOORDS::LON]);

    // some extra keys are edition-specific, so parent call is here
    RegularGrid_fillGrib(info);
}


}  // namespace eccodes::geo
