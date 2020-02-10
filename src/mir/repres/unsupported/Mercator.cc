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


#include "mir/repres/unsupported/Mercator.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"


namespace mir {
namespace repres {


Mercator::Mercator(const param::MIRParametrisation& parametrisation) {}


Mercator::Mercator() {}


Mercator::~Mercator() = default;


void Mercator::print(std::ostream& out) const {
    out << "Mercator["
        << "]";
}


void Mercator::fill(grib_info& info) const {
    NOTIMP;
}


static RepresentationBuilder<Mercator> mercator("mercator");  // Name is what is returned by grib_api


}  // namespace repres
}  // namespace mir
