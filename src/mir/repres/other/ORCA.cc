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


#include "mir/repres/other/ORCA.h"

#include <ostream>
#include <utility>
#include <vector>

#include "atlas/grid/SpecRegistry.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/Log.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir {
namespace repres {
namespace other {


// order is important for makeName()
static const std::vector<std::pair<std::string, std::string>> grib_keys{
    {"orca_name", "unstructuredGridType"}, {"orca_arrangement", "unstructuredGridSubtype"}, {"uid", "uuidOfHGrid"}};


ORCA::ORCA(const std::string& uid) :
    Gridded(util::BoundingBox() /*assumed global*/), spec_(atlas::grid::SpecRegistry::get(uid)) {}


ORCA::ORCA(const param::MIRParametrisation& param) :
    ORCA([&param]() {
        std::string uid;
        ASSERT(param.get("uid", uid));
        return uid;
    }()) {}


ORCA::~ORCA() = default;


bool ORCA::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const ORCA*>(&other);
    return (o != nullptr) && spec_.getString("uid") == o->spec_.getString("uid");
}


void ORCA::validate(const MIRValuesVector& values) const {
    size_t count = numberOfPoints();

    Log::debug() << "ORCA::validate checked " << Log::Pretty(values.size(), {"value"}) << ", iterator counts "
                 << Log::Pretty(count) << "." << std::endl;

    ASSERT_VALUES_SIZE_EQ_ITERATOR_COUNT("ORCA", values.size(), count);
}


size_t ORCA::numberOfPoints() const {
    return static_cast<size_t>(atlasGridRef().size());
}


void ORCA::fill(grib_info& info) const {
    info.grid.grid_type        = GRIB_UTIL_GRID_SPEC_UNSTRUCTURED;
    info.packing.editionNumber = 2;

    for (const auto& key : grib_keys) {
        auto value = spec_.getString(key.first);
        info.extra_set(key.second.c_str(), value.c_str());
    }
}


void ORCA::makeName(std::ostream& out) const {
    const auto* sep = "";
    for (const auto& key : grib_keys) {
        out << sep << spec_.getString(key.first);
        sep = "_";
    }
}


void ORCA::print(std::ostream& out) const {
    out << "ORCA[spec=" << spec_ << "]";
}


Iterator* ORCA::iterator() const {
    class ORCAIterator : public Iterator {
        ::atlas::Grid grid_;  // Note: needs the object because IterateLonLat uses a Grid reference
        ::atlas::Grid::IterateLonLat lonlat_;

        decltype(lonlat_)::iterator it_;
        decltype(lonlat_)::iterator::value_type p_;

        const size_t total_;
        size_t count_;
        bool first_;

        void print(std::ostream& out) const override {
            out << "ORCAIterator[";
            Iterator::print(out);
            out << ",count=" << count_ << ",total=" << total_ << "]";
        }

        bool next(Latitude& _lat, Longitude& _lon) override {
            if (it_.next(p_)) {
                point_[0] = p_.lat();
                point_[1] = p_.lon();
                _lat      = p_.lat();
                _lon      = p_.lon();

                if (first_) {
                    first_ = false;
                }
                else {
                    count_++;
                }

                return true;
            }

            ASSERT(count_ == total_);
            return false;
        }

        size_t index() const override { return count_; }

    public:
        ORCAIterator(::atlas::Grid grid) :
            grid_(grid),
            lonlat_(grid.lonlat()),
            it_(lonlat_.begin()),
            total_(size_t(grid.size())),
            count_(0),
            first_(true) {}

        ~ORCAIterator() override = default;

        ORCAIterator(const ORCAIterator&) = delete;
        ORCAIterator(ORCAIterator&&)      = delete;
        ORCAIterator& operator=(const ORCAIterator&) = delete;
        ORCAIterator& operator=(ORCAIterator&&) = delete;
    };
    return new ORCAIterator(atlasGridRef());
}


const atlas::Grid& ORCA::atlasGridRef() const {
    return grid_ ? grid_ : (grid_ = atlas::Grid(spec_));
}


atlas::Grid ORCA::atlasGrid() const {
    return atlasGridRef();
}


void ORCA::fill(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "orca";
    }
}


}  // namespace other
}  // namespace repres
}  // namespace mir
