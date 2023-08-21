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


#include "mir/repres/latlon/RegularLL.h"

#include <ostream>

#include "eckit/log/JSON.h"

#include "mir/iterator/detail/RegularIterator.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Atlas.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"
#include "mir/util/GridBox.h"


namespace mir::repres::latlon {

RegularLL::RegularLL(const param::MIRParametrisation& parametrisation) : LatLon(parametrisation) {}

RegularLL::RegularLL(const util::Increments& increments, const util::BoundingBox& bbox, const PointLatLon& reference) :
    LatLon(increments, bbox, reference) {}

RegularLL::~RegularLL() = default;

Iterator* RegularLL::iterator() const {

    class RegularLLIterator : protected LatLonIterator, public Iterator {
        void print(std::ostream& out) const override {
            out << "RegularLLIterator[";
            Iterator::print(out);
            out << ",";
            LatLonIterator::print(out);
            out << "]";
        }
        bool next(Latitude& lat, Longitude& lon) override { return LatLonIterator::next(lat, lon); }

        size_t index() const override { return count_; }

    public:
        RegularLLIterator(size_t ni, size_t nj, Latitude north, Longitude west, const util::Increments& increments) :
            LatLonIterator(ni, nj, north, west, increments) {}
    };

    return new RegularLLIterator(ni_, nj_, bbox_.north(), bbox_.west(), increments_);
}

void RegularLL::print(std::ostream& out) const {
    out << "RegularLL[";
    LatLon::print(out);
    out << "]";
}

void RegularLL::json(eckit::JSON& json) const {
    json.startObject();
    json << "type"
         << "regular_ll";
    LatLon::json(json);
    json.endObject();
}

atlas::Grid RegularLL::atlasGrid() const {
    // NOTE: yspace uses bounding box and not the domain
    // (this works together with the Atlas RectangularDomain cropping)
    const auto dom = domain();

    atlas::StructuredGrid::XSpace xspace(
        atlas::grid::LinearSpacing(dom.west().value(), dom.east().value(), long(ni_), !dom.isPeriodicWestEast()));

    atlas::StructuredGrid::YSpace yspace(
        atlas::grid::LinearSpacing(bbox_.north().value(), bbox_.south().value(), long(nj_)));

    return atlas::StructuredGrid(xspace, yspace, {}, dom);
}

void RegularLL::fillGrib(grib_info& info) const {
    LatLon::fillGrib(info);

    // See copy_spec_from_ksec.c in libemos for info
    // Warning: scanning mode not considered
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REGULAR_LL;
}

void RegularLL::fillJob(api::MIRJob& job) const {
    LatLon::fillJob(job);
}

void RegularLL::makeName(std::ostream& out) const {
    LatLon::makeName(out);
}

bool RegularLL::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const RegularLL*>(&other);
    return (o != nullptr) && LatLon::sameAs(other);
}

const RegularLL* RegularLL::croppedRepresentation(const util::BoundingBox& bbox) const {
    return new RegularLL(increments_, bbox, reference_);
}

util::BoundingBox RegularLL::extendBoundingBox(const util::BoundingBox& bbox) const {
    using iterator::detail::RegularIterator;

    auto sn = increments_.south_north().latitude().fraction();
    auto we = increments_.west_east().longitude().fraction();
    ASSERT(sn > 0);
    ASSERT(we > 0);

    auto shift_sn = (reference_.lat().fraction() / sn).decimalPart() * sn;
    auto shift_we = (reference_.lon().fraction() / we).decimalPart() * we;

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

std::vector<util::GridBox> RegularLL::gridBoxes() const {

    auto dom   = domain();
    auto north = dom.north().value();
    auto south = dom.south().value();

    auto lat0 = bbox_.north();
    auto lon0 = bbox_.west();
    auto sn   = increments_.south_north().latitude();
    auto we   = increments_.west_east().longitude().fraction();

    eckit::Fraction half(1, 2);


    // latitude edges
    std::vector<double> latEdges(nj_ + 1);

    latEdges[0] = (lat0 + sn / 2).value();
    for (size_t j = 0; j < nj_; ++j) {
        latEdges[j + 1] = (lat0 - (j + half) * sn.fraction()).value();
    }

    latEdges.front() = std::min(north, std::max(south, latEdges.front()));
    latEdges.back()  = std::min(north, std::max(south, latEdges.back()));


    // longitude edges
    std::vector<double> lonEdges(ni_ + 1);
    lonEdges[0] = (lon0 - we / 2).value();
    for (size_t i = 0; i < ni_; ++i) {
        lonEdges[i + 1] = (lon0 + (i + half) * we).value();
    }

    bool periodic = isPeriodicWestEast();
    if (!periodic) {
        lonEdges.front() = std::max(lonEdges.front(), lon0.value());
        lonEdges.back()  = std::min(lonEdges.back(), bbox_.east().value());
    }


    // grid boxes
    std::vector<util::GridBox> r;
    r.reserve(ni_ * nj_);

    for (size_t j = 0; j < nj_; ++j) {
        Longitude lon1 = lon0;

        for (size_t i = 0; i < ni_; ++i) {
            auto l = lon1;
            lon1   = l + we * (i + half);
            r.emplace_back(util::GridBox(latEdges[j], lonEdges[i], latEdges[j + 1], lonEdges[i + 1]));
        }

        ASSERT(periodic ? lon0 == lon1.normalise(lon0) : lon0 < lon1.normalise(lon0));
    }

    ASSERT(r.size() == numberOfPoints());
    return r;
}

std::string RegularLL::factory() const {
    return "regular_ll";
}


static const RepresentationBuilder<RegularLL> regularLL("regular_ll");


}  // namespace mir::repres::latlon
