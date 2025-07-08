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


#include "mir/repres/regular/Mercator.h"

#include "mir/util/Exceptions.h"


namespace mir::repres::regular {


static const RepresentationBuilder<Mercator> __builder("mercator");


Mercator::Mercator(const param::MIRParametrisation& param) : RegularGrid(param, make_proj_spec(param)) {}


void Mercator::fillGrib(grib_info& /*info*/) const {
    NOTIMP;
}


}  // namespace mir::repres::regular
