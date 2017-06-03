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
                     const util::Increments &increments) :
    LatLon(bbox, increments) {
}


RegularLL::~RegularLL() {
}


// Called by RegularLL::crop()
const RegularLL *RegularLL::cropped(const util::BoundingBox &bbox) const {
    // eckit::Log::debug<LibMir>() << "Create cropped copy as RegularLL bbox=" << bbox << std::endl;
    return new RegularLL(bbox, increments_);
}


void RegularLL::print(std::ostream &out) const {
    out << "RegularLL[";
    LatLon::print(out);
    out << "]";
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
    const util::Domain dom = domain();

    // use bounding box for non-shifted/shifted grid (it is the best we have)
    bool isPeriodicEastWest = dom.isPeriodicEastWest();
    double north = bbox_.north();
    double south = bbox_.south();
    double west = bbox_.west();
    double east = isPeriodicEastWest? west + 360 : bbox_.east();

    using atlas::grid::StructuredGrid;
    using atlas::grid::LinearSpacing;
    StructuredGrid::XSpace xspace( LinearSpacing( west,  east,  long(ni_), !isPeriodicEastWest ));
    StructuredGrid::YSpace yspace( LinearSpacing( north, south, long(nj_) ));

    atlas::RectangularDomain domain({ dom.west(), dom.east() }, { dom.south(), dom.north() });
    return StructuredGrid(xspace, yspace, StructuredGrid::Projection(), domain);
}


Representation* RegularLL::globalise(data::MIRField& field) const {
    ASSERT(field.representation() == this);

    if (domain().isGlobal()) {
        return 0;
    }

    // For now, we only use that function for the LAW model, so we only grow by the end (south pole)
    ASSERT(eckit::Fraction(bbox_.north()) == 90);
    ASSERT(eckit::Fraction(bbox_.west()) == 0);
    ASSERT(eckit::Fraction(bbox_.east()) + eckit::Fraction(increments_.west_east()) == 360);

    util::BoundingBox newbbox(bbox_.north(), bbox_.west(), -90, bbox_.east());

    eckit::ScopedPtr<RegularLL> newll(new RegularLL(newbbox, increments_));

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

Representation* RegularLL::subset(data::MIRField& field,
                                  const util::Increments& increments) const {

    ASSERT(field.representation() == this);

    // Increments must match

    if (!increments.multipleOf(increments_)) {
        std::ostringstream oss;
        oss << "RegularLL::subset " << increments << " is not a multiple  of " << increments_;
        throw eckit::UserError(oss.str());
    }


    size_t skipI = 0;
    size_t skipJ = 0;
    increments.ratio(increments_, skipI, skipJ);

    eckit::ScopedPtr<RegularLL> newll(new RegularLL(bbox_, increments));

    size_t n = ni_ * nj_;
    size_t newn = newll->ni_ * newll->nj_;

    for (size_t f = 0; f < field.dimensions(); f++ ) {
        std::vector<double> newvalues(newn);
        const std::vector<double> &values = field.direct(f);
        ASSERT(values.size() == n);

        size_t k = 0;
        for (size_t j = 0; j < nj_; j += skipJ) {
            for (size_t i = 0; i < ni_; i += skipI) {
                ASSERT(k < newn);
                ASSERT(j*ni_ +i < n);
                newvalues[k++] = values[j * ni_ + i];
            }
        }

        ASSERT(k == newvalues.size());
        field.update(newvalues, f);
    }
    return newll.release();
}

namespace {
static RepresentationBuilder<RegularLL> regularLL("regular_ll"); // Name is what is returned by grib_api
}


}  // namespace latlon
}  // namespace repres
}  // namespace mir

