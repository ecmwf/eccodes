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


#include "mir/repres/proxy/ProxyGrid.h"

#include "mir/repres/Iterator.h"
#include "mir/util/Atlas.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"


namespace mir::repres::proxy {


ProxyGrid::ProxyGrid() : Gridded(util::BoundingBox()) {}


void ProxyGrid::validate(const MIRValuesVector& values) const {
    size_t count = numberOfPoints();

    Log::debug() << "Atlas::validate checked " << Log::Pretty(values.size(), {"value"}) << ", iterator counts "
                 << Log::Pretty(count) << "." << std::endl;

    ASSERT_VALUES_SIZE_EQ_ITERATOR_COUNT("Atlas", values.size(), count);
}


size_t ProxyGrid::numberOfPoints() const {
    return static_cast<size_t>(atlasGridRef().size());
}


::atlas::Grid ProxyGrid::atlasGrid() const {
    return atlasGridRef();
}


Iterator* ProxyGrid::iterator() const {

    class AtlasIterator : public Iterator {
        const ::atlas::Grid& grid_;  // Note: needs the object because IterateLonLat uses a Grid reference
        ::atlas::Grid::IterateLonLat lonlat_;

        decltype(lonlat_)::iterator it_;
        decltype(lonlat_)::iterator::value_type p_;

        const size_t total_;
        size_t index_;
        bool first_;

        void print(std::ostream& out) const override {
            out << "AtlasIterator[";
            Iterator::print(out);
            out << ",count=" << index_ << ",total=" << total_ << "]";
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
                    index_++;
                }

                return true;
            }

            ASSERT(first_ || index_ == (total_ - 1));
            return false;
        }

        size_t index() const override { return index_; }

    public:
        AtlasIterator(const ::atlas::Grid& grid) :
            grid_(grid),
            lonlat_(grid.lonlat()),
            it_(lonlat_.begin()),
            total_(size_t(grid.size())),
            index_(0),
            first_(true) {}

        ~AtlasIterator() override = default;

        AtlasIterator(const AtlasIterator&)            = delete;
        AtlasIterator(AtlasIterator&&)                 = delete;
        AtlasIterator& operator=(const AtlasIterator&) = delete;
        AtlasIterator& operator=(AtlasIterator&&)      = delete;
    };

    return new AtlasIterator(atlasGridRef());
}

}  // namespace mir::repres::proxy
