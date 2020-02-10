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


#include "mir/repres/unsupported/SpaceView.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"


namespace mir {
namespace repres {


SpaceView::SpaceView(const param::MIRParametrisation& parametrisation) {}


SpaceView::SpaceView() {}


SpaceView::~SpaceView() = default;


void SpaceView::print(std::ostream& out) const {
    out << "SpaceView["
        << "]";
}


void SpaceView::fill(grib_info& info) const {
    NOTIMP;
}


static RepresentationBuilder<SpaceView> spaceView("space_view");  // Name is what is returned by grib_api


}  // namespace repres
}  // namespace mir
