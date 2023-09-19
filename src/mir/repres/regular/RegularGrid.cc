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


#include "mir/repres/regular/RegularGrid.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <ostream>
#include <sstream>

#include "eckit/config/Resource.h"
#include "eckit/utils/MD5.h"
#include "eckit/utils/StringTools.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Domain.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/Log.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir::repres::regular {


RegularGrid::RegularGrid(const param::MIRParametrisation& param, const RegularGrid::Projection& projection) :
    shape_(param), xPlus_(true), yPlus_(false), firstPointBottomLeft_(false) {
    ASSERT(projection);

    auto get_long_first_key = [](const param::MIRParametrisation& param, const std::vector<std::string>& keys) -> long {
        long value = 0;
        for (const auto& key : keys) {
            if (param.get(key, value)) {
                return value;
            }
        }
        throw exception::SeriousBug("RegularGrid: couldn't find any key: " + eckit::StringTools::join(", ", keys));
    };

    long nx = get_long_first_key(param, {"numberOfPointsAlongXAxis", "Ni"});
    long ny = get_long_first_key(param, {"numberOfPointsAlongYAxis", "Nj"});
    ASSERT(nx > 0);
    ASSERT(ny > 0);

    std::vector<double> grid;
    ASSERT(param.get("grid", grid));
    ASSERT_KEYWORD_GRID_SIZE(grid.size());

    Point2 firstLL;
    ASSERT(param.get("latitudeOfFirstGridPointInDegrees", firstLL[LLCOORDS::LAT]));
    ASSERT(param.get("longitudeOfFirstGridPointInDegrees", firstLL[LLCOORDS::LON]));
    auto first = projection.xy(firstLL);

    param.get("iScansPositively", xPlus_);  // iScansPositively != 0
    param.get("jScansPositively", yPlus_);  // jScansPositively == 0
    param.get("first_point_bottom_left", firstPointBottomLeft_);

    x_    = linspace(first.x(), grid[0], nx, firstPointBottomLeft_ || xPlus_);
    y_    = linspace(first.y(), grid[1], ny, firstPointBottomLeft_ || yPlus_);
    grid_ = {x_, y_, projection};

    atlas::RectangularDomain range({x_.min(), x_.max()}, {y_.min(), y_.max()}, "meters");
    auto bbox = projection.lonlatBoundingBox(range);
    ASSERT(bbox);

    bbox_ = {bbox.north(), bbox.west(), bbox.south(), bbox.east()};
}


RegularGrid::RegularGrid(const Projection& projection, const util::BoundingBox& bbox, const LinearSpacing& x,
                         const LinearSpacing& y, const util::Shape& shape) :
    Gridded(bbox),
    x_(x),
    y_(y),
    shape_(shape),
    xPlus_(x.front() <= x.back()),
    yPlus_(y.front() < y.back()),
    firstPointBottomLeft_(false) {
    grid_ = {x_, y_, projection};

    if (!shape_.provided) {
        shape_ = {grid_.projection().spec()};
    }
}


RegularGrid::~RegularGrid() = default;


RegularGrid::Projection::Spec RegularGrid::make_proj_spec(const param::MIRParametrisation& param) {
    static bool useProjIfAvailable = eckit::Resource<bool>("$MIR_USE_PROJ_IF_AVAILABLE", true);

    std::string proj;
    param.get("proj", proj);

    if (proj.empty() || !useProjIfAvailable || !::atlas::projection::ProjectionFactory::has("proj")) {
        return {};
    }

    Projection::Spec spec("type", "proj");
    spec.set("proj", proj);

    std::string projSource;
    if (param.get("projSource", projSource) && !projSource.empty()) {
        spec.set("proj_source", projSource);
    }

    std::string projGeocentric;
    if (param.get("projGeocentric", projGeocentric) && !projGeocentric.empty()) {
        spec.set("proj_geocentric", projGeocentric);
    }

    return spec;
}


RegularGrid::LinearSpacing RegularGrid::linspace(double start, double step, long num, bool plus) {
    ASSERT(step >= 0.);
    return {start, start + step * double(plus ? num - 1 : 1 - num), num};
}


std::pair<RegularGrid::ij_t, RegularGrid::ij_t> RegularGrid::minmax_ij(const util::BoundingBox& bbox) const {
    auto Ni = x_.size();
    auto Nj = y_.size();

    ij_t max{0, 0};
    ij_t min{Ni, Nj};

    bool first = true;
    for (std::unique_ptr<Iterator> it(iterator()); it->next();) {
        if (bbox.contains(*(*it))) {
            auto i = it->index() % Ni;
            auto j = it->index() / Ni;
            if (first) {
                first = false;
                max = min = {i, j};
                continue;
            }

            max.i = std::max(max.i, i);
            max.j = std::max(max.j, j);
            min.i = std::min(min.i, i);
            min.j = std::min(min.j, j);
        }
    }

    ASSERT_NONEMPTY_AREA("RegularGrid::minmax_ij", min.i <= max.i && min.j <= max.j);
    return {min, max};
}


void RegularGrid::print(std::ostream& out) const {
    out << "RegularGrid[x=" << x_.spec() << ",y=" << y_.spec() << ",projection=" << grid_.projection().spec()
        << ",firstPointBottomLeft=" << firstPointBottomLeft_ << ",bbox=" << bbox_ << "]";
}


bool RegularGrid::extendBoundingBoxOnIntersect() const {
    return true;
}


bool RegularGrid::crop(util::BoundingBox& bbox, util::IndexMapping& mapping) const {
    auto mm = minmax_ij(bbox);
    auto Ni = x_.size();
    auto N  = (mm.second.i - mm.first.i + 1) * (mm.second.j - mm.first.j + 1);
    mapping.clear();
    mapping.reserve(N);

    for (std::unique_ptr<Iterator> it(iterator()); it->next();) {
        auto i = it->index() % Ni;
        auto j = it->index() / Ni;
        if (mm.first.i <= i && i <= mm.second.i && mm.first.j <= j && j <= mm.second.j) {
            mapping.push_back(it->index());
        }
    }
    ASSERT(mapping.size() == N);

    return true;
}


size_t RegularGrid::numberOfPoints() const {
    return x_.size() * y_.size();
}


::atlas::Grid RegularGrid::atlasGrid() const {
    return grid_;
}


bool RegularGrid::isPeriodicWestEast() const {
    return false;
}


void RegularGrid::fillGrib(grib_info& info) const {
    // shape of the reference object
    shape_.fillGrib(info, grid_.projection().spec());

    // scanningMode
    info.grid.iScansNegatively = x_.back() < x_.front() ? 1 : 0;
    info.grid.jScansPositively = y_.front() < y_.back() ? 1 : 0;
}


bool RegularGrid::includesNorthPole() const {
    return bbox_.north() == Latitude::NORTH_POLE;
}


bool RegularGrid::includesSouthPole() const {
    return bbox_.south() == Latitude::SOUTH_POLE;
}


void RegularGrid::reorder(long /*scanningMode*/, MIRValuesVector& /*unused*/) const {
    // do not reorder, iterator is doing the right thing
    // FIXME this function should not be overriding to do nothing
}


void RegularGrid::validate(const MIRValuesVector& values) const {
    const size_t count = numberOfPoints();

    Log::debug() << "RegularGrid::validate checked " << Log::Pretty(values.size(), {"value"}) << ", iterator counts "
                 << Log::Pretty(count) << " (" << domain() << ")." << std::endl;

    ASSERT_VALUES_SIZE_EQ_ITERATOR_COUNT("RegularGrid", values.size(), count);
}


Iterator* RegularGrid::iterator() const {
    class RegularGridIterator : public Iterator {
        Projection projection_;
        const LinearSpacing& x_;
        const LinearSpacing& y_;
        PointLonLat pLonLat_;

        size_t ni_;
        size_t nj_;
        size_t i_;
        size_t j_;
        size_t count_;

        void print(std::ostream& out) const override {
            out << "RegularGridIterator[";
            Iterator::print(out);
            out << ",i=" << i_ << ",j=" << j_ << ",count=" << count_ << "]";
        }

        bool next(Latitude& _lat, Longitude& _lon) override {
            if (j_ < nj_ && i_ < ni_) {
                pLonLat_ = projection_.lonlat({x_[i_], y_[j_]});
                _lat     = lat(pLonLat_.lat());
                _lon     = lon(pLonLat_.lon());

                if (i_ > 0 || j_ > 0) {
                    count_++;
                }

                if (++i_ == ni_) {
                    i_ = 0;
                    j_++;
                }

                return true;
            }
            return false;
        }

        size_t index() const override { return count_; }

    public:
        RegularGridIterator(Projection projection, const LinearSpacing& x, const LinearSpacing& y) :
            projection_(std::move(projection)), x_(x), y_(y), ni_(x.size()), nj_(y.size()), i_(0), j_(0), count_(0) {}
        ~RegularGridIterator() override = default;

        RegularGridIterator(const RegularGridIterator&)            = delete;
        RegularGridIterator(RegularGridIterator&&)                 = delete;
        RegularGridIterator& operator=(const RegularGridIterator&) = delete;
        RegularGridIterator& operator=(RegularGridIterator&&)      = delete;
    };

    return new RegularGridIterator(grid_.projection(), x_, y_);
}


void RegularGrid::makeName(std::ostream& out) const {
    eckit::MD5 h;
    h << grid_.projection().spec();
    h << x_.spec();
    h << y_.spec();
    h << firstPointBottomLeft_;
    if (shape_.provided) {
        h << shape_.code;
        h << shape_.a;
        h << shape_.b;
    }
    auto type = grid_.projection().spec().getString("type");
    out << "RegularGrid-" << (type.empty() ? "" : type + "-") << h.digest();
}


bool RegularGrid::sameAs(const Representation& other) const {
    auto name = [](const RegularGrid& repres) {
        std::ostringstream str;
        repres.makeName(str);
        return str.str();
    };

    const auto* o = dynamic_cast<const RegularGrid*>(&other);
    return (o != nullptr) && name(*this) == name(*o);
}


void RegularGrid::fillMeshGen(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "structured";
    }
}


}  // namespace mir::repres::regular
