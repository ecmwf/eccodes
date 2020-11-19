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

#include <algorithm>
#include <cctype>
#include <ostream>

#include "eckit/filesystem/PathName.h"

#include "mir/config/LibMir.h"
#include "mir/key/grid/ORCAPattern.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/Log.h"
#include "mir/util/MeshGeneratorParameters.h"
#include "mir/util/Regex.h"


namespace mir {
namespace repres {
namespace other {


namespace {


std::string get_name(const param::MIRParametrisation& param) {
    std::string name;
    ASSERT(param.get("grid", name));
    return name;
}


std::string change_case(const std::string& in, bool up) {
    ASSERT(!in.empty());
    std::string out(in);
    std::transform(in.begin(), in.end(), out.begin(),
                   [up](unsigned char c) { return up ? std::toupper(c) : std::tolower(c); });
    return out;
}


}  // namespace


ORCA::ORCA(const std::string& name) : Gridded(util::BoundingBox()) {
    // setup canonical type/subtype
    auto match = util::Regex(key::grid::ORCAPattern::pattern()).match(name);
    if (!match || match.size() != 3) {
        throw exception::UserError("ORCA: unrecognized name '" + name + "'");
    }

    type_ = change_case(match[1], true);
    if (type_.front() == 'E') {
        type_.front() = 'e';
    }

    subtype_     = match[2].str().front();
    subtypeLong_ = subtype_ + std::string(" grid");
    name_        = type_ + '_' + subtype_;
}


ORCA::ORCA(const param::MIRParametrisation& param) : ORCA(get_name(param)) {}


ORCA::~ORCA() = default;


bool ORCA::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const ORCA*>(&other);
    return (o != nullptr) && name_ == o->name_;
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

    GribExtraSetting::set(info, "unstructuredGridType", type_.c_str());
    GribExtraSetting::set(info, "unstructuredGridSubtype", subtypeLong_.c_str());

    unsigned char uuid[] = "PedroIsSoNaughty";  // exactly 16 bytes
    info.post.addBytes("uuidOfHGrid", uuid, 16);

    // TODO fill uuidOfHGdrid (or maybe not?)
    // GribExtraSetting::set(info, "uuidOfHGdrid", /*16076978a048410747dd7c9876677b28*/);
}


void ORCA::makeName(std::ostream& out) const {
    out << name_;
}


void ORCA::print(std::ostream& out) const {
    out << "ORCA[atlasGrid=" << atlasGridRef().spec() << "]";
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
    if (!grid_) {
        // setup atlas grid (once)
        grid_ = atlas::Grid(name_);
    }
    return grid_;
}


atlas::Grid ORCA::atlasGrid() const {
    return atlasGridRef();
}


void ORCA::fill(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "orca";
    }
    params.set("force_include_south_pole", true);
    params.set("triangulate", false);
}


}  // namespace other
}  // namespace repres
}  // namespace mir
