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


#include "mir/repres/other/ClenshawCurtis.h"

#include <ostream>
#include <sstream>

#include "eckit/exception/Exceptions.h"

#include "mir/config/LibMir.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Grib.h"
#include "mir/util/Pretty.h"


namespace mir {
namespace repres {
namespace other {


ClenshawCurtis::ClenshawCurtis(const std::string& name) : name_(name), grid_(name) {
    util::RectangularDomain rect(grid_.domain());
    if (!rect) {
        std::ostringstream msg;
        msg << "ClenshawCurtis: grid '" << name << "' not supported (domain " << grid_.domain().spec() << ")";
        throw eckit::UserError(msg.str());
    }

    domain_ = util::Domain(rect.containsNorthPole() ? Latitude::NORTH_POLE : rect.ymax(), rect.xmin(),
                           rect.containsSouthPole() ? Latitude::SOUTH_POLE : rect.ymin(),
                           rect.zonal_band() ? rect.xmin() + Longitude::GLOBE.value() : rect.xmax());

    eckit::Log::debug<LibMir>() << "ClenshawCurtis: grid '" << name << "', domain=" << domain_ << std::endl;
}


ClenshawCurtis::ClenshawCurtis(const param::MIRParametrisation&) {
    NOTIMP;
}


ClenshawCurtis::~ClenshawCurtis() = default;


bool ClenshawCurtis::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const ClenshawCurtis*>(&other);
    return (o != nullptr) && name_ == o->name_;
}


void ClenshawCurtis::validate(const data::MIRValuesVector& values) const {
    size_t count = numberOfPoints();
    eckit::Log::debug<LibMir>() << "ClenshawCurtis::validate " << Pretty(values.size(), {"value"}) << ", count: " << Pretty(count)
                                << "." << std::endl;
    ASSERT(values.size() == count);
}


size_t ClenshawCurtis::numberOfPoints() const {
    return grid_.size();
}


void ClenshawCurtis::fill(grib_info& info) const {
    info.grid.grid_type        = GRIB_UTIL_GRID_SPEC_UNSTRUCTURED;
    info.packing.editionNumber = 2;

    // TODO fill metadata
}


void ClenshawCurtis::makeName(std::ostream& out) const {
    out << name_;
}


void ClenshawCurtis::print(std::ostream& out) const {
    out << "ClenshawCurtis[atlasGrid=" << grid_.spec() << ",domain=" << domain_ << "]";
}


Iterator* ClenshawCurtis::iterator() const {

    class ClenshawCurtisIterator : public Iterator {
        ::atlas::Grid grid_;  // Note: needs the object because IterateLonLat uses a Grid reference
        ::atlas::Grid::IterateLonLat lonlat_;

        decltype(lonlat_)::iterator it_;
        decltype(lonlat_)::iterator::value_type point_;

        size_t count_;
        const size_t total_;

        void print(std::ostream& out) const {
            out << "ClenshawCurtisIterator[";
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
        ClenshawCurtisIterator(::atlas::Grid grid) :
            grid_(grid),
            lonlat_(grid.lonlat()),
            it_(lonlat_.begin()),
            count_(0),
            total_(grid.size()) {}

        ClenshawCurtisIterator(const ClenshawCurtisIterator&) = delete;
        ClenshawCurtisIterator& operator=(const ClenshawCurtisIterator&) = delete;
    };

    return new ClenshawCurtisIterator(grid_);
}


}  // namespace other
}  // namespace repres
}  // namespace mir
