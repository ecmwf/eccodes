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

#include "mir/api/mir_config.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Atlas.h"
#include "mir/util/Domain.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"


namespace mir::repres::latlon {

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

        size_t index() const override { return count_; }

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
#if mir_HAVE_ATLAS
    // NOTE: yspace uses bounding box and not the domain
    // (this works together with the Atlas RectangularDomain cropping)
    const util::Domain dom = domain();

    atlas::StructuredGrid::XSpace xspace(
        atlas::grid::LinearSpacing(dom.west().value(), dom.east().value(), long(ni_), !dom.isPeriodicWestEast()));

    atlas::StructuredGrid::YSpace yspace(
        atlas::grid::LinearSpacing(bbox_.north().value(), bbox_.south().value(), long(nj_)));

    atlas::StructuredGrid grid(xspace, yspace, {}, dom);
    return rotate_atlas_grid(rotation_.rotation(), grid);
#else
    NOTIMP;
#endif
}

void RotatedLL::fillGrib(grib_info& info) const {
    LatLon::fillGrib(info);

    info.grid.grid_type = CODES_UTIL_GRID_SPEC_ROTATED_LL;
    rotation_.fillGrib(info);
}

void RotatedLL::fillJob(api::MIRJob& job) const {
    LatLon::fillJob(job);
    rotation_.fillJob(job);
}

void RotatedLL::makeName(std::ostream& out) const {
    LatLon::makeName(out);
    rotation_.makeName(out);
}

bool RotatedLL::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const RotatedLL*>(&other);
    return (o != nullptr) && (rotation_ == o->rotation_) && LatLon::sameAs(other);
}

const RotatedLL* RotatedLL::croppedRepresentation(const util::BoundingBox& bbox) const {
    return new RotatedLL(increments_, rotation_, bbox, reference_);
}


std::string RotatedLL::factory() const {
    return "rotated_ll";
}


static const RepresentationBuilder<RotatedLL> rotatedLL("rotated_ll");


}  // namespace mir::repres::latlon
