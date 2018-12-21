/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @date Apr 2015

#include "mir/repres/unsupported/IrregularLatlon.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/MD5.h"
#include "mir/api/Atlas.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Domain.h"
#include "mir/util/MeshGeneratorParameters.h"
#include <iostream>

namespace mir {
namespace repres {

static void range(const std::vector<double>& v, double& mn, double& mx, double& dmax) {

    ASSERT(v.size() >= 2);

    dmax = 0;
    mx = v[0];
    mn = v[0];

    for (size_t i = 1; i < v.size(); ++i) {
        double d = std::abs(v[i] - v[i - 1]);
        dmax = std::max(d, dmax);
        mx = std::max(v[i], mx);
        mn = std::min(v[i], mn);
    }
}

IrregularLatlon::IrregularLatlon(const param::MIRParametrisation& parametrisation) {

    ASSERT(parametrisation.get("latitudes", latitudes_));
    range(latitudes_, south_, north_, south_north_);

    ASSERT(parametrisation.get("longitudes", longitudes_));
    range(longitudes_, west_, east_, west_east_);
}

IrregularLatlon::IrregularLatlon() {}

IrregularLatlon::~IrregularLatlon() = default;

size_t IrregularLatlon::numberOfPoints() const {
    return latitudes_.size() * longitudes_.size();
}

bool IrregularLatlon::getLongestElementDiagonal(double& d) const {

    // Look for a majorant of all element diagonals, using the difference of
    // latitudes closest/furthest from equator and largest longitude difference

    ASSERT(latitudes_.size() >= 2);
    ASSERT(longitudes_.size() >= 2);

    // largest longitude difference
    double lonMin;
    double lonMax;
    double we;
    range(longitudes_, lonMin, lonMax, we);

    d = 0.;
    for (size_t j = 1; j < latitudes_.size(); ++j) {
        const bool away(std::abs(latitudes_[j - 1]) > std::abs(latitudes_[j]));
        const double &latAwayFromEquator(latitudes_[away ? j - 1 : j]),
            latCloserToEquator(latitudes_[away ? j : j - 1]);

        d = std::max(d, atlas::util::Earth::distance(atlas::PointLonLat(0., latCloserToEquator),
                                                     atlas::PointLonLat(we, latAwayFromEquator)));
    }

    ASSERT(d > 0.);
    return true;
}

void IrregularLatlon::print(std::ostream& out) const {
    out << "IrregularLatlon[latitudes=" << latitudes_.size() << ",longitudes=" << longitudes_.size() << "]";
}

void IrregularLatlon::makeName(std::ostream& out) const {
    out << "irregular-latlon-" << latitudes_.size() << "-" << longitudes_.size() << "-";
    eckit::MD5 md5;
    for (double d : latitudes_) {
        md5 << d;
    }
    for (double d : longitudes_) {
        md5 << d;
    }
    out << std::string(md5);
}

bool IrregularLatlon::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const IrregularLatlon*>(&other);
    return o && (latitudes_ == o->latitudes_) && (longitudes_ == o->longitudes_);
}

void IrregularLatlon::fill(grib_info&) const {
    NOTIMP;
}

void IrregularLatlon::fill(util::MeshGeneratorParameters& params) const {
// FIXME confirm the following works!
#if 0
    params.meshGenerator_ = "structured";
    if (boundingBox().south() > Latitude::EQUATOR) {
        params.set("force_include_south_pole", true);
    }
    if (boundingBox().north() < Latitude::EQUATOR) {
        params.set("force_include_north_pole", true);
    }
#endif

    params.meshGenerator_ = "delaunay";
}

util::Domain IrregularLatlon::domain() const {
    return util::Domain(includesNorthPole() ? Latitude::NORTH_POLE.value() : north_, west_,
                        includesSouthPole() ? Latitude::SOUTH_POLE.value() : south_,
                        isPeriodicWestEast() ? west_ + Longitude::GLOBE.value() : east_);
}

class IrregularLatlonIterator : public Iterator {

    size_t count_;
    size_t i_;
    size_t ni_;
    size_t j_;
    size_t nj_;

    const std::vector<double>& latitudes_;
    const std::vector<double>& longitudes_;

    virtual void print(std::ostream& out) const {
        out << "IrregularLatlonIterator[";
        Iterator::print(out);
        out << "]";
    }

    virtual bool next(Latitude& lat, Longitude& lon) {
        if (j_ < nj_) {
            if (i_ < ni_) {
                lat = latitudes_[j_];
                lon = longitudes_[i_];
                i_++;
                if (i_ == ni_) {
                    j_++;
                    i_ = 0;
                }
                count_++;
                return true;
            }
        }
        return false;
    }

public:
    // TODO: Consider keeping a reference on the latitudes and bbox, to avoid copying

    IrregularLatlonIterator(const std::vector<double>& latitudes, const std::vector<double>& longitudes)
        : count_(0)
        , i_(0)
        , ni_(longitudes.size())
        , j_(0)
        , nj_(latitudes.size())
        , latitudes_(latitudes)
        , longitudes_(longitudes) {}

    ~IrregularLatlonIterator() { ASSERT(count_ == ni_ * nj_); }
};

Iterator* IrregularLatlon::iterator() const {
    return new IrregularLatlonIterator(latitudes_, longitudes_);
}

bool IrregularLatlon::isPeriodicWestEast() const {
    return (east_ - west_) + west_east_ >= Longitude::GLOBE.value();
}

bool IrregularLatlon::includesNorthPole() const {
    return north_ + south_north_ >= Latitude::NORTH_POLE.value();
}

bool IrregularLatlon::includesSouthPole() const {
    return south_ - south_north_ <= Latitude::SOUTH_POLE.value();
}

atlas::Grid IrregularLatlon::atlasGrid() const {

    std::vector<atlas::PointXY>* pts = new std::vector<atlas::PointXY>();
    pts->reserve(latitudes_.size() * longitudes_.size());

    for (double lat : latitudes_) {
        for (double lon : longitudes_) {
            pts->push_back(atlas::PointXY(lon, lat));
        }
    }

    return atlas::UnstructuredGrid(pts);
}

namespace {
static RepresentationBuilder<IrregularLatlon>
    irregularLatlon("irregular_latlon"); // Name is what is returned by grib_api
}

} // namespace repres
} // namespace mir
