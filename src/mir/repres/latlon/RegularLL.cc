/*
 * (C) Copyright 1996- ECMWF.
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

#include "mir/config/LibMir.h"
#include "mir/iterator/detail/RegularIterator.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"

namespace mir {
namespace repres {
namespace latlon {

RegularLL::RegularLL(const param::MIRParametrisation& parametrisation) : LatLon(parametrisation) {}

RegularLL::RegularLL(const util::Increments& increments, const util::BoundingBox& bbox, const PointLatLon& reference)
    : LatLon(increments, bbox, reference) {}

RegularLL::~RegularLL() = default;

Iterator* RegularLL::iterator() const {

    class RegularLLIterator : protected LatLonIterator, public Iterator {
        void print(std::ostream& out) const {
            out << "RegularLLIterator[";
            Iterator::print(out);
            out << ",";
            LatLonIterator::print(out);
            out << "]";
        }
        bool next(Latitude& lat, Longitude& lon) { return LatLonIterator::next(lat, lon); }

    public:
        RegularLLIterator(size_t ni, size_t nj, Latitude north, Longitude west, const util::Increments& increments)
            : LatLonIterator(ni, nj, north, west, increments) {}
    };

    return new RegularLLIterator(ni_, nj_, bbox_.north(), bbox_.west(), increments_);
}

void RegularLL::print(std::ostream& out) const {
    out << "RegularLL[";
    LatLon::print(out);
    out << "]";
}

atlas::Grid RegularLL::atlasGrid() const {

    // NOTE: for non-shifted/shifted grid, yspace uses bounding box
    // (this works together with the Atlas RectangularDomain cropping)
    const util::Domain dom = domain();
    double n = bbox_.north().value();
    double s = bbox_.south().value();
    double w = dom.west().value();
    double e = dom.east().value();

    using atlas::StructuredGrid;
    using atlas::grid::LinearSpacing;
    StructuredGrid::XSpace xspace(LinearSpacing(w, e, long(ni_), !dom.isPeriodicWestEast()));
    StructuredGrid::YSpace yspace(LinearSpacing(n, s, long(nj_)));

    return StructuredGrid(xspace, yspace, StructuredGrid::Projection(), dom);
}

void RegularLL::fill(grib_info& info) const {
    LatLon::fill(info);

    // See copy_spec_from_ksec.c in libemos for info
    // Warning: scanning mode not considered
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_LL;
}

void RegularLL::fill(api::MIRJob& job) const {
    LatLon::fill(job);
}

void RegularLL::makeName(std::ostream& out) const {
    LatLon::makeName(out);
}

bool RegularLL::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const RegularLL*>(&other);
    return o && LatLon::sameAs(other);
}

const RegularLL* RegularLL::croppedRepresentation(const util::BoundingBox& bbox) const {
    // Called by AreaCropper::execute and Gridded2GriddedInterpolation::execute

    const PointLatLon reference(bbox_.south(), bbox_.west());

    return new RegularLL(increments_, bbox, reference);
}

util::BoundingBox RegularLL::extendedBoundingBox(const util::BoundingBox& bbox) const {
    using iterator::detail::RegularIterator;

    const PointLatLon reference(bbox_.south(), bbox_.west());

    auto sn = increments_.south_north().latitude().fraction();
    auto we = increments_.west_east().longitude().fraction();
    ASSERT(sn > 0);
    ASSERT(we > 0);

    auto shift_sn = (reference.lat().fraction() / sn).decimalPart() * sn;
    auto shift_we = (reference.lon().fraction() / we).decimalPart() * we;

    // adjust West/East to include bbox's West/East ('outwards')
    Longitude w = bbox.west();
    if (increments_.isPeriodic()) {
        w = shift_we + RegularIterator::adjust(bbox.west().fraction() - shift_we, we, false);
    }
    Longitude e = shift_we + RegularIterator::adjust(bbox.east().fraction() - shift_we, we, true);


    // adjust South/North to include bbox's South/North ('outwards')
    auto s = shift_sn + RegularIterator::adjust(bbox.south().fraction() - shift_sn, sn, false);
    if (s < Latitude::SOUTH_POLE.fraction()) {
        s = shift_sn + RegularIterator::adjust(Latitude::SOUTH_POLE.fraction() - shift_sn, sn, true);
    }
    auto n = shift_sn + RegularIterator::adjust(bbox.north().fraction() - shift_sn, sn, true);
    if (n > Latitude::NORTH_POLE.fraction()) {
        n = shift_sn + RegularIterator::adjust(Latitude::NORTH_POLE.fraction() - shift_sn, sn, false);
    }

    // set bounding box
    const util::BoundingBox extended(n, w, s, e);
    ASSERT(extended.contains(bbox));

    return extended;
}

namespace {
static RepresentationBuilder<RegularLL> regularLL("regular_ll"); // Name is what is returned by grib_api
}

} // namespace latlon
} // namespace repres
} // namespace mir
