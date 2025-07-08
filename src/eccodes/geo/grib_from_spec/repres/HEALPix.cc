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


#include "mir/repres/HEALPix.h"

#include <ostream>
#include <regex>

#include "eckit/geo/grid/HEALPix.h"
#include "eckit/log/JSON.h"
#include "eckit/types/FloatCompare.h"

#include "mir/api/MIRJob.h"
#include "mir/iterator/UnstructuredIterator.h"
#include "mir/key/grid/GridPattern.h"
#include "mir/key/grid/NamedGrid.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Atlas.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/GridBox.h"
#include "mir/util/MeshGeneratorParameters.h"
#include "mir/util/Reorder.h"

#if mir_HAVE_ATLAS
#include "atlas/interpolation/method/knn/GridBox.h"
#endif


namespace mir::repres {


namespace {


class NamedHEALPix : public key::grid::NamedGrid {
public:
    // -- Constructors

    NamedHEALPix(const std::string& name, size_t Nside, const std::string& order) :
        NamedGrid(name), Nside_(Nside), order_(order) {}

protected:
    // -- Overridden methods

    void print(std::ostream& out) const override { out << "NamedHEALPix[key=" << key_ << "]"; }
    size_t gaussianNumber() const override { return 2 * Nside_; }
    const repres::Representation* representation() const override { return new repres::HEALPix(Nside_, order_); }
    const repres::Representation* representation(const util::Rotation&) const override { NOTIMP; }

private:
    // -- Members

    const size_t Nside_;
    const std::string order_;
};


class HEALPixPattern : public key::grid::GridPattern {
public:
    // -- Constructors

    explicit HEALPixPattern(const std::string& pattern, bool nested) : GridPattern(pattern), nested_(nested) {}

private:
    static size_t Nside(const std::string& name) {
        static const std::regex rex("[1-9][0-9]*");

        std::smatch match;
        ASSERT(std::regex_search(name, match, rex));

        return std::stoul(match.str());
    }

    // -- Overridden methods

    void print(std::ostream& out) const override { out << "HEALPixPattern[pattern=" << pattern_ << "]"; }

    const key::grid::Grid* make(const std::string& name) const override {
        return new NamedHEALPix(name, Nside(name), nested_ ? "nested" : "ring");
    }

    std::string canonical(const std::string& name, const param::MIRParametrisation&) const override {
        return "H" + std::to_string(Nside(name)) + (nested_ ? "n" : "");
    }

    // -- Members

    const bool nested_;
};


const HEALPixPattern HEALPIX_N("^[hH]([nN][1-9][0-9]*|[1-9][0-9]*([nN]|_[nN][eE][sS][tT][eE][dD]))$", true);
const HEALPixPattern HEALPIX_R("^[hH]([rR][1-9][0-9]*|[1-9][0-9]*([rR]|_[rR][iI][nN][gG])?)$", false);


const RepresentationBuilder<HEALPix> HEALPIX("healpix");


}  // namespace


HEALPix::HEALPix(size_t Nside, const std::string& order) : grid_(new eckit::geo::grid::HEALPix(Nside, order)) {
    ASSERT(grid_);
}


HEALPix::HEALPix(const param::MIRParametrisation& param) :
    HEALPix(
        [&param]() {
            double lon1 = 0.;
            ASSERT(param.get("longitudeOfFirstGridPointInDegrees", lon1));
            ASSERT(eckit::types::is_approximately_equal(lon1, 45.));

            size_t Nside = 0;
            ASSERT(param.get("Nside", Nside));
            return Nside;
        }(),
        [&param]() {
            std::string order;
            ASSERT(param.get("orderingConvention", order));
            return order;
        }()) {}


std::string HEALPix::name() const {
    return "H" + std::to_string(grid_->Nside()) + (grid_->order() == "nested" ? "n" : "");
}


bool HEALPix::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const HEALPix*>(&other);
    return (o != nullptr) && *grid_ == *(o->grid_);
}


std::pair<std::vector<double>, std::vector<double>>& HEALPix::to_latlons() const {
    if (points_.first.empty() || points_.second.empty()) {
        ASSERT(points_.first.empty() && points_.second.empty());

        points_ = grid_->to_latlons();
        ASSERT(points_.first.size() == points_.second.size());
        ASSERT(points_.first.size() == numberOfPoints());
    }

    return points_;
}


size_t HEALPix::numberOfPoints() const {
    return grid_->size();
}


void HEALPix::makeName(std::ostream& out) const {
    out << name();
}


void HEALPix::fillGrib(grib_info& info) const {
    info.grid.grid_type                          = GRIB_UTIL_GRID_SPEC_HEALPIX;
    info.grid.N                                  = static_cast<long>(grid_->Nside());
    info.grid.longitudeOfFirstGridPointInDegrees = 45.;

    info.extra_set("orderingConvention", grid_->order().c_str());
}


void HEALPix::fillMeshGen(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = grid_->order() == "nested" ? "delaunay" : "structured";
    }
}


void HEALPix::fillJob(api::MIRJob& job) const {
    job.set("grid", name());
}


void HEALPix::json(eckit::JSON& j) const {
    static_cast<eckit::geo::Grid&>(*grid_).spec().json(j);
}


void HEALPix::print(std::ostream& out) const {
    out << "HEALPix[name=" << name() << "]";
}


void HEALPix::validate(const MIRValuesVector& values) const {
    ASSERT_VALUES_SIZE_EQ_ITERATOR_COUNT("HEALPix", values.size(), numberOfPoints());
}


Iterator* HEALPix::iterator() const {
    const auto& [lats, lons] = to_latlons();
    return new iterator::UnstructuredIterator(lats, lons);
}


std::vector<util::GridBox> HEALPix::gridBoxes() const {
#if mir_HAVE_ATLAS
    const auto Nside  = grid_->Nside();
    const auto& order = grid_->order();


    // forcing order=ring, reordering later
    atlas::HealpixGrid grid_r(static_cast<int>(Nside), "ring");

    ::atlas::interpolation::method::GridBoxes boxes_a(grid_r, false);
    ASSERT(boxes_a.size() == numberOfPoints());

    auto ren = std::unique_ptr<util::Reorder>(
                   util::Reorder::build(order == "nested" ? "healpix-ring-to-nested" : "identity", numberOfPoints()))
                   ->reorder();
    ASSERT(ren.size() == numberOfPoints());

    std::vector<util::GridBox> boxes(boxes_a.size());
    for (size_t i = 0; i < boxes_a.size(); ++i) {
        boxes[ren[i]] = util::GridBox{boxes_a[i].north(), boxes_a[i].west(), boxes_a[i].south(), boxes_a[i].east()};
    }

    return boxes;
#else
    NOTIMP;
#endif
}


atlas::Grid HEALPix::atlasGrid() const {
#if mir_HAVE_ATLAS
    if (grid_->order() == "nested") {
        const auto& [lats, lons] = to_latlons();

        auto* points = new std::vector<::atlas::PointXY>;
        ASSERT(points != nullptr);

        points->reserve(numberOfPoints());
        for (size_t i = 0; i < numberOfPoints(); ++i) {
            points->emplace_back(lons[i], lats[i]);
        }

        return ::atlas::UnstructuredGrid(points);  // takes ownership
    }

    return atlas::HealpixGrid(static_cast<int>(grid_->Nside()), grid_->order());
#else
    NOTIMP;
#endif
}


}  // namespace mir::repres
