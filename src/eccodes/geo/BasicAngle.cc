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


#include "eccodes/geo/BasicAngle.h"

#include <limits>
#include <ostream>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/types/Fraction.h"


namespace eccodes::geo
{


static inline double normalise_longitude(const double& lon, const double& minimum)
{
    return ::eckit::geo::PointLonLat::normalise_angle_to_minimum(lon, minimum);
}


Fraction::Fraction(double d) :
    Fraction(eckit::Fraction(d)) {}


Fraction::Fraction(const eckit::Fraction& frac) :
    num(frac.numerator()), den(frac.denominator())
{
    constexpr auto min = std::numeric_limits<value_type>::lowest();
    constexpr auto max = std::numeric_limits<value_type>::max();
    ASSERT(frac.denominator() != 0);
    ASSERT(min <= frac.denominator() && frac.denominator() <= max);
    ASSERT(min <= frac.numerator() && frac.numerator() <= max);
}


BasicAngle::BasicAngle(Fraction a, Fraction b, Fraction c, Fraction d, Fraction e, Fraction f) :
    Fraction(gcd(a.num, b.num, c.num, d.num, e.num, f.num), lcm(a.den, b.den, c.den, d.den, e.den, f.den))
{
    ASSERT(den != 0);
}


BasicAngle::BasicAngle(const grib_info& info) :
    BasicAngle(Fraction(info.grid.latitudeOfFirstGridPointInDegrees),
               Fraction(normalise_longitude(info.grid.longitudeOfFirstGridPointInDegrees, 0.)),
               Fraction(info.grid.latitudeOfLastGridPointInDegrees),
               Fraction(normalise_longitude(info.grid.longitudeOfLastGridPointInDegrees, 0.)),
               Fraction(info.grid.iDirectionIncrementInDegrees), Fraction(info.grid.jDirectionIncrementInDegrees)) {}


void BasicAngle::fillGrib(grib_info& info) const
{
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
    fill("longitudeOfFirstGridPoint", normalise_longitude(info.grid.longitudeOfFirstGridPointInDegrees, 0.));
    fill("latitudeOfLastGridPoint", info.grid.latitudeOfLastGridPointInDegrees);
    fill("longitudeOfLastGridPoint", normalise_longitude(info.grid.longitudeOfLastGridPointInDegrees, 0.));
    fill("iDirectionIncrement", info.grid.iDirectionIncrementInDegrees);
    fill("jDirectionIncrement", info.grid.jDirectionIncrementInDegrees);
}


Fraction::value_type BasicAngle::numerator(const Fraction& f) const
{
    Fraction x(f.num * den, f.den * num);
    const auto div = gcd(x.num, x.den);

    ASSERT(x.den == div);  // if BasicAngle is adequate
    return x.num / div;
}


void BasicAngle::list(std::ostream& out)
{
    out << "as-input, decimal, fraction";
}


}  // namespace eccodes::geo
