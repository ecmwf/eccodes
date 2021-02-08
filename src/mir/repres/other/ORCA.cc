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

#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/Log.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir {
namespace repres {
namespace other {


static const char* UUID    = "uuidOfHGrid";
static const char* TYPE    = "unstructuredGridType";
static const char* SUBTYPE = "unstructuredGridSubtype";


ORCA::ORCA(const std::string& id) :
    Gridded(util::BoundingBox()), spec_(atlas::util::SpecRegistry<atlas::Grid>::lookup(id)) {}


ORCA::ORCA(const param::MIRParametrisation& param) :
    ORCA([&param]() {
        std::string uuid;
        ASSERT(param.get(UUID, uuid) && !uuid.empty());
        return uuid;
    }()) {}


ORCA::~ORCA() = default;


bool ORCA::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const ORCA*>(&other);
    return (o != nullptr) && spec_.getString(UUID) == o->spec_.getString(UUID);
}


void ORCA::validate(const data::MIRValuesVector& values) const {
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

    for (auto& key : {TYPE, SUBTYPE, UUID}) {
        auto value = spec_.getString(key);
        info.extra_set(key, value.c_str());
    }
}


void ORCA::makeName(std::ostream& out) const {
    auto name = spec_.getString(TYPE) + "_" + spec_.getString(SUBTYPE) + "_" + spec_.getString(UUID);
    out << name;
}


void ORCA::print(std::ostream& out) const {
    out << "ORCA[spec=" << spec_ << "]";
}


Iterator* ORCA::iterator() const {
    class ORCAIterator : public Iterator {
        ::atlas::Grid grid_;  // Note: needs the object because IterateLonLat uses a Grid reference
        ::atlas::Grid::IterateLonLat lonlat_;

        decltype(lonlat_)::iterator it_;
        decltype(lonlat_)::iterator::value_type point_;

        size_t count_;
        const size_t total_;

        void print(std::ostream& out) const {
            out << "ORCAIterator[";
            Iterator::print(out);
            out << ",count=" << count_ << ",total=" << total_ << "]";
        }

        bool next(Latitude& _lat, Longitude& _lon) {
            if (it_.next(point_)) {
                _lat = point_.lat();
                _lon = point_.lon();
                ++count_;
                return true;
            }

            ASSERT(count_ == total_);
            return false;
        }

    public:
        ORCAIterator(::atlas::Grid grid) :
            grid_(grid), lonlat_(grid.lonlat()), it_(lonlat_.begin()), count_(0), total_(grid.size()) {}

        ORCAIterator(const ORCAIterator&) = delete;
        ORCAIterator& operator=(const ORCAIterator&) = delete;
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
    // FIXME: should come from spec
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "orca";
    }
    params.set("fixup", true);                     // This makes sure that there are no invalid elements
    params.set("force_include_south_pole", true);  // Add South Pole virtual points
}


}  // namespace other
}  // namespace repres
}  // namespace mir
