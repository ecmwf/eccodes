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


#include "mir/repres/FESOM.h"

#include <cctype>
#include <ostream>
#include <vector>

#include "eckit/geo/grid/unstructured/FESOM.h"

#include "mir/api/MIRJob.h"
#include "mir/api/mir_config.h"
#include "mir/iterator/UnstructuredIterator.h"
#include "mir/key/grid/GridPattern.h"
#include "mir/key/grid/NamedGrid.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir::repres {


namespace {


class NamedFESOM : public key::grid::NamedGrid {
public:
    // -- Constructors

    explicit NamedFESOM(const std::string& key) : NamedGrid(key) {}

protected:
    // -- Overridden methods

    void print(std::ostream& out) const override { out << "NamedFESOM[key=" << key_ << "]"; }
    size_t gaussianNumber() const override { return default_gaussian_number(); }
    const repres::Representation* representation() const override { return new FESOM(key_); }
    const repres::Representation* representation(const util::Rotation&) const override { NOTIMP; }
};


const std::string PATTERN("^([cC][oO][rR][eE]2|[dD][aA][rR][tT]|[nN][gG]5|[pP][iI])(_[cCnN])?$");


class FESOMPattern : public key::grid::GridPattern {
public:
    // -- Constructors

    explicit FESOMPattern(const std::string& pattern) : GridPattern(pattern) {}

private:
    // -- Overridden methods

    void print(std::ostream& out) const override { out << "FESOMPattern[pattern=" << pattern_ << "]"; }

    const key::grid::Grid* make(const std::string& name) const override { return new NamedFESOM(name); }

    std::string canonical(const std::string& name, const param::MIRParametrisation& param) const override {
        ASSERT(!name.empty());

        static const std::regex rex(PATTERN);

        std::smatch match;
        ASSERT(std::regex_search(name, match, rex) && match.size() == 4);

        auto e(match[1].str());
        auto n(match[2].str());
        auto a(match[3].str());

        if (e.size() == 1) {
            e = static_cast<char>(std::tolower(e.back()));
        }

        if (a.empty()) {
            a = "T";  // arbitrary choice (to review)
            param.get("orca-arrangement", a);
        }
        else if (a.size() == 2) {
            a = static_cast<char>(std::toupper(a.back()));
        }
        ASSERT(a.size() == 1);

        return e + "FESOM" + n + "_" + a;
    }
};


const FESOMPattern __FESOM(PATTERN);


}  // namespace


FESOM::FESOM(const std::string& grid) :
    grid_([&grid]() {
        eckit::geo::spec::Custom custom{{"grid", grid}};
        std::unique_ptr<eckit::geo::Spec> spec(eckit::geo::GridFactory::make_spec(custom));

        return new grid_type(*spec);
    }()) {}


FESOM::FESOM(const param::MIRParametrisation& param) :
    FESOM([&param]() {
        std::string uid;
        ASSERT(param.get("uid", uid));
        return uid;
    }()) {}


std::string FESOM::match(const std::string& name, const param::MIRParametrisation& param) {
    return key::grid::GridPattern::match(name, param);
}


std::string FESOM::name() const {
    auto n = grid_->name() + "_" + grid_->arrangement();

    if (const auto& spec = static_cast<const eckit::geo::Grid&>(*grid_).spec(); spec.has("uid")) {
        n += "_" + spec.get_string("uid");
    }

    return n;
}


FESOM::points_type& FESOM::to_latlons() const {
    if (points_.first.empty() || points_.second.empty()) {
        ASSERT(points_.first.empty() && points_.second.empty());

        points_ = grid_->to_latlons();
        ASSERT(points_.first.size() == points_.second.size());
        ASSERT(points_.first.size() == numberOfPoints());
    }

    return points_;
}


bool FESOM::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const FESOM*>(&other);
    return (o != nullptr) && *grid_ == *(o->grid_);
}


void FESOM::makeName(std::ostream& out) const {
    out << name();
}


void FESOM::fillGrib(grib_info& info) const {
    info.grid.grid_type        = GRIB_UTIL_GRID_SPEC_UNSTRUCTURED;
    info.packing.editionNumber = 2;

    info.extra_set("unstructuredGridType", grid_->name().c_str());
    info.extra_set("unstructuredGridSubtype", grid_->arrangement().c_str());
    info.extra_set("uuidOfHGrid", grid_->uid().c_str());
}


void FESOM::fillMeshGen(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "delaunay";
    }
}


void FESOM::fillJob(api::MIRJob& job) const {
    const auto& spec = static_cast<const eckit::geo::Grid&>(*grid_).spec();
    job.set("grid", spec.get_string(spec.has("uid") ? "uid" : "grid"));
}


void FESOM::json(eckit::JSON& j) const {
    const auto& spec = static_cast<const eckit::geo::Grid&>(*grid_).spec();
    spec.json(j);
}


Iterator* FESOM::iterator() const {
    const auto& [lats, lons] = to_latlons();
    return new iterator::UnstructuredIterator(lats, lons);
}


void FESOM::print(std::ostream& out) const {
    out << "FESOM[grid=" << name() << "]";
}


void FESOM::validate(const MIRValuesVector& values) const {
    ASSERT_VALUES_SIZE_EQ_ITERATOR_COUNT("FESOM", values.size(), numberOfPoints());
}


size_t FESOM::numberOfPoints() const {
    return grid_->size();
}


atlas::Grid FESOM::atlasGrid() const {
#if mir_HAVE_ATLAS
    return {atlas::grid::SpecRegistry::get(grid_->uid())};
#else
    NOTIMP;
#endif
}


static const RepresentationBuilder<FESOM> __grid("fesom");


}  // namespace mir::repres::geo
