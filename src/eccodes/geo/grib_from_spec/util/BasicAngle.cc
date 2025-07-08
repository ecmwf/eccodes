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


namespace eccodes::geo::util {


void BasicAngle_fillGrib(grib_info& info) const {
    // FIXME limited functionality
    ASSERT(info.grid.grid_type == CODES_UTIL_GRID_SPEC_REGULAR_LL);

    auto fill = [this, &info](const char* key, double value) {
        Fraction f(value);
        ASSERT(f.den != 0);
        if (f.num != 0) {
            info.extra_set(key, numerator(f));
        }
    };

    info.extra_set("basicAngleOfTheInitialProductionDomain", num);
    info.extra_set("subdivisionsOfBasicAngle", den);

    fill("latitudeOfFirstGridPoint", info.grid.latitudeOfFirstGridPointInDegrees);
    fill("longitudeOfFirstGridPoint", util::normalise_longitude(info.grid.longitudeOfFirstGridPointInDegrees, 0.));
    fill("latitudeOfLastGridPoint", info.grid.latitudeOfLastGridPointInDegrees);
    fill("longitudeOfLastGridPoint", util::normalise_longitude(info.grid.longitudeOfLastGridPointInDegrees, 0.));
    fill("iDirectionIncrement", info.grid.iDirectionIncrementInDegrees);
    fill("jDirectionIncrement", info.grid.jDirectionIncrementInDegrees);
}


Fraction::value_type BasicAngle::numerator(const Fraction& f) const {
    Fraction x(f.num * den, f.den * num);
    const auto div = gcd(x.num, x.den);

    ASSERT(x.den == div);  // if BasicAngle is adequate
    return x.num / div;
}


void BasicAngle::list(std::ostream& out) {
    out << "as-input, decimal, fraction";
}


}  // namespace mir::grib
