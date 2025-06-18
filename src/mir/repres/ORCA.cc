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


#include "mir/repres/ORCA.h"

#include <cctype>
#include <ostream>
#include <vector>

#include "eckit/geo/grid/ORCA.h"

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


class NamedORCA : public key::grid::NamedGrid {
public:
    // -- Constructors

    explicit NamedORCA(const std::string& key) : NamedGrid(key) {}

protected:
    // -- Overridden methods

    void print(std::ostream& out) const override { out << "NamedORCA[key=" << key_ << "]"; }
    size_t gaussianNumber() const override { return default_gaussian_number(); }
    const repres::Representation* representation() const override { return new repres::ORCA(key_); }
    const repres::Representation* representation(const util::Rotation&) const override { NOTIMP; }
};


const std::string PATTERN("^([eE])?[oO][rR][cC][aA]([0-9]+)(_[tTuUvVwWfF])?$");


class ORCAPattern : public key::grid::GridPattern {
public:
    // -- Constructors

    explicit ORCAPattern(const std::string& pattern) : GridPattern(pattern) {}

private:
    // -- Overridden methods

    void print(std::ostream& out) const override { out << "ORCAPattern[pattern=" << pattern_ << "]"; }

    const key::grid::Grid* make(const std::string& name) const override { return new NamedORCA(name); }

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

        return e + "ORCA" + n + "_" + a;
    }
};


const ORCAPattern __ORCA(PATTERN);


}  // namespace


ORCA::ORCA(const std::string& grid) :
    grid_([&grid]() {
        eckit::geo::spec::Custom custom{{"grid", grid}};
        std::unique_ptr<eckit::geo::Spec> spec(eckit::geo::GridFactory::make_spec(custom));

        return new eckit::geo::grid::ORCA(*spec);
    }()) {}


ORCA::ORCA(const param::MIRParametrisation& param) :
    ORCA([&param]() {
        std::string uid;
        ASSERT(param.get("uid", uid));
        return uid;
    }()) {}


std::string ORCA::match(const std::string& name, const param::MIRParametrisation& param) {
    return key::grid::GridPattern::match(name, param);
}


std::string ORCA::name() const {
    auto n = grid_->name() + "_" + grid_->arrangement();

    const auto& spec = grid_->spec();
    if (spec.has("uid")) {
        n += "_" + spec.get_string("uid");
    }

    return n;
}


ORCA::points_type& ORCA::to_latlons() const {
    if (points_.first.empty() || points_.second.empty()) {
        ASSERT(points_.first.empty() && points_.second.empty());

        points_ = grid_->to_latlons();
        ASSERT(points_.first.size() == points_.second.size());
        ASSERT(points_.first.size() == numberOfPoints());
    }

    return points_;
}


bool ORCA::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const ORCA*>(&other);
    return (o != nullptr) && *grid_ == *(o->grid_);
}


void ORCA::makeName(std::ostream& out) const {
    out << name();
}


void ORCA::fillGrib(grib_info& info) const {
    info.grid.grid_type        = GRIB_UTIL_GRID_SPEC_UNSTRUCTURED;
    info.packing.editionNumber = 2;

    info.extra_set("unstructuredGridType", grid_->name().c_str());
    info.extra_set("unstructuredGridSubtype", grid_->arrangement().c_str());
    info.extra_set("uuidOfHGrid", grid_->uid().c_str());
}


void ORCA::fillMeshGen(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "orca";
    }
}


void ORCA::fillJob(api::MIRJob& job) const {
    const auto& spec = grid_->spec();
    job.set("grid", spec.get_string(spec.has("uid") ? "uid" : "grid"));
}


void ORCA::json(eckit::JSON& j) const {
    grid_->spec().json(j);
}


Iterator* ORCA::iterator() const {
    const auto& [lats, lons] = to_latlons();
    return new iterator::UnstructuredIterator(lats, lons);
}


void ORCA::print(std::ostream& out) const {
    out << "ORCA[grid=" << name() << "]";
}


void ORCA::validate(const MIRValuesVector& values) const {
    ASSERT_VALUES_SIZE_EQ_ITERATOR_COUNT("ORCA", values.size(), numberOfPoints());
}


size_t ORCA::numberOfPoints() const {
    return grid_->size();
}


atlas::Grid ORCA::atlasGrid() const {
#if mir_HAVE_ATLAS
    return {atlas::grid::SpecRegistry::get(grid_->uid())};
#else
    NOTIMP;
#endif
}


static const RepresentationBuilder<ORCA> __grid("orca");


}  // namespace mir::repres
