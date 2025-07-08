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


#pragma once

#include <iosfwd>
#include <numeric>


struct grib_info;

namespace eckit {
class Fraction;
}


namespace mir::grib {


struct Fraction {
    // -- Types

    using value_type = long;

    // -- Exceptions
    // None

    // -- Constructors

    explicit Fraction(double);

    explicit Fraction(const eckit::Fraction&);

    Fraction(value_type _numerator, value_type _denominator) :
        num(_numerator), den(_denominator == 0 || _numerator == 0 ? 1 : _denominator) {}

    // -- Members

    const value_type num;
    const value_type den;
};


constexpr Fraction::value_type lcm(Fraction::value_type a) {
    return a;
}


template <typename... Values>
constexpr Fraction::value_type lcm(Fraction::value_type a, Values... bc) {
    return (std::lcm(a, lcm(bc...)));
}


constexpr Fraction::value_type gcd(Fraction::value_type a) {
    return a;
}


template <typename... Values>
constexpr Fraction::value_type gcd(Fraction::value_type a, Values... bc) {
    return (std::gcd(a, gcd(bc...)));
}


struct BasicAngle : Fraction {
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    using Fraction::Fraction;
    BasicAngle(Fraction, Fraction, Fraction, Fraction, Fraction, Fraction);
    explicit BasicAngle(const grib_info&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    void fillGrib(grib_info&) const;
    value_type numerator(const Fraction&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods

    static void list(std::ostream&);
};


}  // namespace mir::grib
