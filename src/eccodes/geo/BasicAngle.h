/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include "eccodes/geo/Geo.h"


namespace eckit
{
class Fraction;
}


namespace eccodes::geo
{


struct Fraction
{
    using value_type = long;

    explicit Fraction() :
        num(0), den(1) {}

    explicit Fraction(double);
    explicit Fraction(const ::eckit::Fraction&);

    Fraction(value_type _numerator, value_type _denominator);

    const value_type num;
    const value_type den;
};


struct BasicAngle : Fraction
{
    using Fraction::Fraction;

    BasicAngle(Fraction, Fraction, Fraction, Fraction, Fraction, Fraction);

    explicit BasicAngle(const grib_info&);

    void fillGrib(grib_info&) const;

    value_type numerator(const Fraction&) const;
};


}  // namespace eccodes::geo
