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


#include "mir/repres/latlon/RotatedLL.h"

#include <ostream>

#include "mir/repres/Iterator.h"
#include "mir/util/Atlas.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace latlon {

RotatedLL::RotatedLL(const param::MIRParametrisation& parametrisation) :
    LatLon(parametrisation), rotation_(parametrisation) {}

RotatedLL::RotatedLL(const util::Increments& increments, const util::Rotation& rotation, const util::BoundingBox& bbox,
                     const PointLatLon& reference) :
    LatLon(increments, bbox, reference), rotation_(rotation) {}

RotatedLL::~RotatedLL() = default;

Iterator* RotatedLL::iterator() const {

    class RotatedLLIterator : protected LatLonIterator, public Iterator {
        void print(std::ostream& out) const override {
            out << "RotatedLLIterator[";
            Iterator::print(out);
            out << ",";
            LatLonIterator::print(out);
            out << "]";
        }
        bool next(Latitude& lat, Longitude& lon) override { return LatLonIterator::next(lat, lon); }

    public:
        RotatedLLIterator(size_t ni, size_t nj, Latitude north, Longitude west, const util::Increments& increments,
                          const util::Rotation& rotation) :
            LatLonIterator(ni, nj, north, west, increments), Iterator(rotation) {}
    };

    return new RotatedLLIterator(ni_, nj_, bbox_.north(), bbox_.west(), increments_, rotation_);
}

void RotatedLL::print(std::ostream& out) const {
    out << "RotatedLL[";
    LatLon::print(out);
    out << ",rotation=" << rotation_ << "]";
}

atlas::Grid RotatedLL::atlasGrid() const {

    // NOTE: for non-shifted/shifted grid, yspace uses bounding box
    // (this works together with the Atlas RectangularDomain cropping)
    const util::Domain dom = domain();
    double n               = bbox_.north().value();
    double s               = bbox_.south().value();
    double w               = dom.west().value();
    double e               = dom.east().value();

    using atlas::StructuredGrid;
    using atlas::grid::LinearSpacing;
    StructuredGrid::XSpace xspace(LinearSpacing(w, e, long(ni_), !dom.isPeriodicWestEast()));
    StructuredGrid::YSpace yspace(LinearSpacing(n, s, long(nj_)));

    StructuredGrid unrotatedGrid(xspace, yspace, StructuredGrid::Projection(), dom);
    return rotation_.rotate(unrotatedGrid);
}

void RotatedLL::fill(grib_info& info) const {
    LatLon::fill(info);

    info.grid.grid_type = CODES_UTIL_GRID_SPEC_ROTATED_LL;
    rotation_.fill(info);
}

void RotatedLL::fill(api::MIRJob& job) const {
    LatLon::fill(job);
    rotation_.fill(job);
}

void RotatedLL::makeName(std::ostream& out) const {
    LatLon::makeName(out);
    rotation_.makeName(out);
}

bool RotatedLL::sameAs(const Representation& other) const {

    auto o = dynamic_cast<const RotatedLL*>(&other);
    return (o != nullptr) && (rotation_ == o->rotation_) && LatLon::sameAs(other);
}

const RotatedLL* RotatedLL::croppedRepresentation(const util::BoundingBox& bbox) const {
    // Called by AreaCropper::execute and Gridded2GriddedInterpolation::execute

    const PointLatLon reference(bbox_.south(), bbox_.west());

    return new RotatedLL(increments_, rotation_, bbox, reference);
}


std::string RotatedLL::factory() const {
    return "rotated_ll";
}


static RepresentationBuilder<RotatedLL> rotatedLL("rotated_ll");


}  // namespace latlon
}  // namespace repres
}  // namespace mir
