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

#include <ostream>

#include "eckit/log/Log.h"
#include "eckit/utils/MD5.h"

#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Grib.h"
#include "mir/util/MeshGeneratorParameters.h"
#include "mir/util/Pretty.h"


namespace mir {
namespace repres {
namespace regular {

RegularGrid::RegularGrid(const param::MIRParametrisation& param, const RegularGrid::Projection& projection) {

    ASSERT(projection);

    shapeOfTheEarthProvided_ = param.get("shapeOfTheEarth", shapeOfTheEarth_ = 6);
    param.get("radius", radius_ = ::atlas::util::Earth::radius());
    param.get("earthMajorAxis", earthMajorAxis_ = radius_);
    param.get("earthMinorAxis", earthMinorAxis_ = radius_);

    size_t nx = 0;
    size_t ny = 0;
    ASSERT(param.get("numberOfPointsAlongXAxis", nx) && nx > 0);
    ASSERT(param.get("numberOfPointsAlongYAxis", ny) && ny > 0);

    std::vector<double> grid;
    ASSERT(param.get("grid", grid) && grid.size() == 2);
    double dx = grid[0];
    double dy = grid[1];
    ASSERT(dx > 0);
    ASSERT(dy > 0);

    // y[0] > y[1], for the "canonical" scanningMode (iScansPositively, jScansNegatively)
    Point2 firstLL;
    ASSERT(param.get("latitudeOfFirstGridPointInDegrees", firstLL[LLCOORDS::LAT]));
    ASSERT(param.get("longitudeOfFirstGridPointInDegrees", firstLL[LLCOORDS::LON]));

    Point2 first = projection.xy(firstLL);
    Point2 last  = first + Point2{(nx - 1) * dx, (1 - int(ny)) * dy};

    x_ = {first[XYZCOORDS::XX], last[XYZCOORDS::XX], long(nx)};
    y_ = {first[XYZCOORDS::YY], last[XYZCOORDS::YY], long(ny)};
    ASSERT(x_.front() < x_.back());
    ASSERT(y_.front() > y_.back());

    grid_ = {x_, y_, projection};

    ::atlas::RectangularDomain range({x_.front(), x_.back()}, {y_.front(), y_.back()}, "meters");
    ::atlas::RectangularLonLatDomain bbox = projection.lonlatBoundingBox(range);
    ASSERT(bbox);

    bbox_ = {bbox.north(), bbox.west(), bbox.south(), bbox.east()};
}

RegularGrid::~RegularGrid() = default;

void RegularGrid::print(std::ostream& out) const {
    out << "RegularGrid["
           "x="
        << x_.spec() << ",y=" << y_.spec() << ",projection=" << grid_.projection().spec() << ",bbox=" << bbox_ << "]";
}

bool RegularGrid::extendBoundingBoxOnIntersect() const {
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

void RegularGrid::fill(grib_info& info) const {

    // GRIB2 encoding of user-provided radius or semi-major/minor axis
    if (info.packing.editionNumber == 2) {
        auto spec = grid_.projection().spec();

        if (shapeOfTheEarthProvided_) {
            GribExtraSetting::set(info, "shapeOfTheEarth", shapeOfTheEarth_);
            switch (shapeOfTheEarth_) {
                case 1:
                    GribExtraSetting::set(info, "radius", spec.getDouble("radius", radius_));
                    break;
                case 3:
                    GribExtraSetting::set(info, "earthMajorAxis",
                                          spec.getDouble("semi_major_axis", earthMajorAxis_) / 1000.);
                    GribExtraSetting::set(info, "earthMinorAxis",
                                          spec.getDouble("semi_minor_axis", earthMinorAxis_) / 1000.);
                    break;
                case 7:
                    GribExtraSetting::set(info, "earthMajorAxis", spec.getDouble("semi_major_axis", earthMajorAxis_));
                    GribExtraSetting::set(info, "earthMinorAxis", spec.getDouble("semi_minor_axis", earthMinorAxis_));
                    break;
                default:
                    break;
            }
        }
        else if (spec.has("radius")) {
            GribExtraSetting::set(info, "shapeOfTheEarth", 1L);
            GribExtraSetting::set(info, "radius", spec.getDouble("radius"));
        }
        else if (spec.has("semi_major_axis") && spec.has("semi_minor_axis")) {
            GribExtraSetting::set(info, "shapeOfTheEarth", 7L);
            GribExtraSetting::set(info, "earthMajorAxis", spec.getDouble("semi_major_axis"));
            GribExtraSetting::set(info, "earthMinorAxis", spec.getDouble("semi_minor_axis"));
        }
    }
}

bool RegularGrid::includesNorthPole() const {
    return bbox_.north() == Latitude::NORTH_POLE;
}

bool RegularGrid::includesSouthPole() const {
    return bbox_.south() == Latitude::SOUTH_POLE;
}

void RegularGrid::reorder(long scanningMode, mir::data::MIRValuesVector& values) const {
    GribReorder::reorder(values, scanningMode, x_.size(), y_.size());
}

void RegularGrid::validate(const MIRValuesVector& values) const {
    const size_t count = numberOfPoints();
    eckit::Log::debug<LibMir>() << "RegularGrid::validate checked " << Pretty(values.size(), {"value"})
                                << ", numberOfPoints: " << Pretty(count) << "." << std::endl;
    ASSERT(values.size() == count);
}

Iterator* RegularGrid::iterator() const {

    class AtlasRegularIterator : public Iterator {
        Projection projection_;
        const LinearSpacing& x_;
        const LinearSpacing& y_;
        PointLonLat pLonLat_;

        size_t ni_;
        size_t nj_;
        size_t i_;
        size_t j_;
        size_t count_;

        void print(std::ostream& out) const {
            out << "AtlasRegularIterator[";
            Iterator::print(out);
            out << ",i=" << i_ << ",j=" << j_ << ",count=" << count_ << "]";
        }

        bool next(Latitude& _lat, Longitude& _lon) {
            if (j_ < nj_ && i_ < ni_) {
                pLonLat_ = projection_.lonlat({x_[i_], y_[j_]});
                _lat     = lat(pLonLat_.lat());
                _lon     = lon(pLonLat_.lon());

                if (++i_ == ni_) {
                    i_ = 0;
                    j_++;
                }

                count_++;
                return true;
            }
            return false;
        }

    public:
        AtlasRegularIterator(Projection projection, const LinearSpacing& x, const LinearSpacing& y) :
            projection_(std::move(projection)),
            x_(x),
            y_(y),
            ni_(x.size()),
            nj_(y.size()),
            i_(0),
            j_(0),
            count_(0) {
            // ensure iScansPositively, jScansNegatively
            ASSERT(x_.front() < x_.back());
            ASSERT(y_.front() > y_.back());
        }
        AtlasRegularIterator(const AtlasRegularIterator&) = delete;
        AtlasRegularIterator& operator=(const AtlasRegularIterator&) = delete;
    };

    return new AtlasRegularIterator(grid_.projection(), x_, y_);
}

void RegularGrid::makeName(std::ostream& out) const {
    eckit::MD5 h;
    h << grid_.projection().spec();
    h << x_.spec();
    h << y_.spec();
    if (shapeOfTheEarthProvided_) {
        h << shapeOfTheEarth_;
        h << radius_;
        h << earthMajorAxis_;
        h << earthMinorAxis_;
    }
    auto type = grid_.projection().spec().getString("type");
    out << "RegularGrid-" << (type.empty() ? "" : type + "-") << h.digest();
}

bool RegularGrid::sameAs(const Representation& other) const {
    auto name = [](const RegularGrid& repres) {
        std::stringstream str;
        repres.makeName(str);
        return str.str();
    };

    auto o = dynamic_cast<const RegularGrid*>(&other);
    return (o != nullptr) && name(*this) == name(*o);
}

void RegularGrid::fill(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "structured";
    }
}

}  // namespace regular
}  // namespace repres
}  // namespace mir
