/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @date Apr 2015


#include <iostream>

#include "eckit/exception/Exceptions.h"

#include "mir/param/MIRParametrisation.h"

#include "mir/repres/unsupported/SpaceView.h"


namespace mir {
namespace repres {


SpaceView::SpaceView(const param::MIRParametrisation &parametrisation) {
}


SpaceView::SpaceView() {
}


SpaceView::~SpaceView() {
}


void SpaceView::print(std::ostream &out) const {
    out << "SpaceView["
        << "]";
}


void SpaceView::fill(grib_info &info) const  {
    NOTIMP;
}


namespace {
static RepresentationBuilder<SpaceView> spaceView("space_view"); // Name is what is returned by grib_api
}


}  // namespace repres
}  // namespace mir

