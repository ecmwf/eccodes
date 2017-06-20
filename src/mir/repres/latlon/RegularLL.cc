/*
 * (C) Copyright 1996-2016 ECMWF.
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


#include "mir/repres/latlon/RegularLL.h"

#include <iostream>
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"
#include "atlas/grid.h"
#include "mir/config/LibMir.h"
#include "mir/data/MIRField.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace latlon {


RegularLL::RegularLL(const param::MIRParametrisation &parametrisation):
    LatLon(parametrisation) {}


RegularLL::RegularLL(const util::BoundingBox &bbox,
                     const util::Increments &increments,
                     const util::Shift& shift) :
    LatLon(bbox, increments, shift) {
}


RegularLL::~RegularLL() {
}


// Called by RegularLL::crop()
const RegularLL *RegularLL::cropped(const util::BoundingBox &bbox) const {
    // eckit::Log::debug<LibMir>() << "Create cropped copy as RegularLL bbox=" << bbox << std::endl;
    return new RegularLL(bbox, increments_, shift_);
}


void RegularLL::print(std::ostream &out) const {
    out << "RegularLL[";
    LatLon::print(out);
    out << "]";
}


void RegularLL::makeName(std::ostream& out) const {
    LatLon::makeName(out);
}


bool RegularLL::sameAs(const Representation& other) const {
    const RegularLL* o = dynamic_cast<const RegularLL*>(&other);
    return o && LatLon::sameAs(other);
}


void RegularLL::fill(grib_info &info) const  {
    // See copy_spec_from_ksec.c in libemos for info
    // Warning: scanning mode not considered

    LatLon::fill(info);
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_LL;
}


void RegularLL::fill(api::MIRJob &job) const  {
    LatLon::fill(job);
}


atlas::Grid RegularLL::atlasGrid() const {

    // NOTE: for non-shifted/shifted grid, yspace uses bounding box
    // (this works together with the Atlas RectangularDomain cropping)
    const util::Domain dom = domain();

    using atlas::grid::StructuredGrid;
    using atlas::grid::LinearSpacing;
    StructuredGrid::XSpace xspace( LinearSpacing( dom.west().value(),  dom.east().value(),  long(ni_), !dom.isPeriodicEastWest() ));
    StructuredGrid::YSpace yspace( LinearSpacing( bbox_.north().value(), bbox_.south().value(), long(nj_) ));

    return StructuredGrid(xspace, yspace, StructuredGrid::Projection(), domain());
}


Representation* RegularLL::globalise(data::MIRField& field) const {
    ASSERT(field.representation() == this);

    if (isGlobal()) {
        return 0;
    }

    ASSERT(!shift_);

    // For now, we only use that function for the LAW model, so we only grow by the end (south pole)
    ASSERT(bbox_.north() == Latitude::NORTH_POLE);
    ASSERT(bbox_.west() == Longitude::GREENWICH);
    ASSERT(bbox_.east() + increments_.west_east() == Longitude::GLOBE);

    util::BoundingBox newbbox(bbox_.north(), bbox_.west(), -90, bbox_.east());

    eckit::ScopedPtr<RegularLL> newll(new RegularLL(newbbox, increments_, util::Shift(0, 0)));

    ASSERT(newll->nj_ > nj_);
    ASSERT(newll->ni_ == ni_);

    size_t n = ni_ * nj_;
    size_t newn = newll->ni_ * newll->nj_;
    double missingValue = field.missingValue();

    for (size_t i = 0; i < field.dimensions(); i++ ) {
        std::vector<double> newvalues(newn, missingValue);
        const std::vector<double> &values = field.direct(i);
        ASSERT(values.size() == n);

        for (size_t j = 0 ; j < n; ++j) {
            newvalues[j] = values[j];
        }

        field.update(newvalues, i);
    }

    field.hasMissing(true);

    return newll.release();
}


namespace {
static RepresentationBuilder<RegularLL> regularLL("regular_ll"); // Name is what is returned by grib_api
}


}  // namespace latlon
}  // namespace repres
}  // namespace mir

