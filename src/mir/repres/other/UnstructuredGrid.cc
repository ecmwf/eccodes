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


#include "mir/repres/other/UnstructuredGrid.h"

#include <fstream>
#include <iostream>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Plural.h"
#include "eckit/serialisation/FileStream.h"
#include "eckit/serialisation/IfstreamStream.h"
#include "eckit/utils/MD5.h"

#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Domain.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir {
namespace repres {
namespace other {


UnstructuredGrid::UnstructuredGrid(const param::MIRParametrisation& parametrisation) {
    ASSERT(parametrisation.get("latitudes", latitudes_));
    ASSERT(parametrisation.get("longitudes", longitudes_));
    ASSERT(latitudes_.size() == longitudes_.size());
    ASSERT(!longitudes_.empty());

    bool checkDuplicatePoints = true;
    parametrisation.get("check-duplicate-points", checkDuplicatePoints);

    if (checkDuplicatePoints) {
        check("UnstructuredGrid from MIRParametrisation",
              latitudes_,
              longitudes_);
    }
}


UnstructuredGrid::UnstructuredGrid(const eckit::PathName& path) {
    std::ifstream in(path.asString().c_str());
    if (!in) {
        throw eckit::CantOpenFile(path);
    }

    if (!::isprint(in.peek())) {

        eckit::Log::info() << "UnstructuredGrid::load  " << path << std::endl;


        eckit::IfstreamStream s(in);
        size_t version;
        s >> version;
        ASSERT(version == 1);

        size_t count;
        s >> count;

        latitudes_.resize(count);
        longitudes_.resize(count);

        for (size_t i = 0; i < count; ++i) {
            s >> latitudes_[i];
            s >> longitudes_[i];
            // eckit::Log::info() << latitudes_[i] << " " << longitudes_[i] << std::endl;

        }
    }
    else {
        double lat;
        double lon;
        while (in >> lat >> lon) {
            latitudes_.push_back(lat);
            longitudes_.push_back(lon);
        }
    }

    check("UnstructuredGrid from " + path.asString(),
          latitudes_,
          longitudes_);

}


void UnstructuredGrid::save(const eckit::PathName& path,
                            const std::vector<double>& latitudes,
                            const std::vector<double>& longitudes,
                            bool binary) {

    check("UnstructuredGrid save to " + path.asString(), latitudes, longitudes);

    eckit::Log::info() << "UnstructuredGrid::save " << path << std::endl;

    ASSERT(latitudes.size() == longitudes.size());
    if (binary) {
        eckit::FileStream s(path, "w");
        size_t version = 1;
        size_t count = latitudes.size();
        s << version;
        s << count;
        for (size_t i = 0; i < count; ++i) {
            s << latitudes[i];
            s << longitudes[i];

            eckit::Log::info() << latitudes[i] << " " << longitudes[i] << std::endl;
        }
        s.close();
    }
    else {
        NOTIMP;
    }
}



UnstructuredGrid::UnstructuredGrid(const std::vector<double>& latitudes,
                                   const std::vector<double>& longitudes,
                                   const util::BoundingBox& bbox) :
    Gridded(bbox),
    latitudes_(latitudes),
    longitudes_(longitudes) {
    ASSERT(latitudes_.size() == longitudes_.size());
}


UnstructuredGrid::~UnstructuredGrid() = default;


void UnstructuredGrid::print(std::ostream& out) const {
    out << "UnstructuredGrid[points=" << numberOfPoints()
        << "]";
}


void UnstructuredGrid::makeName(std::ostream& out) const {

    out << "unstructured-" << numberOfPoints() << "-";

    eckit::MD5 md5;
    for (const auto& j : latitudes_) {
        md5 << j;
    }
    for (const auto& j : longitudes_) {
        md5 << j;
    }
    out << std::string(md5);
}


bool UnstructuredGrid::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const UnstructuredGrid*>(&other);
    return o && (latitudes_ == o->latitudes_) && (longitudes_ == o->longitudes_);
}


void UnstructuredGrid::fill(grib_info&) const {
    NOTIMP;
}


void UnstructuredGrid::fill(api::MIRJob&) const {
    NOTIMP;
}


void UnstructuredGrid::fill(util::MeshGeneratorParameters& params) const {
    params.meshGenerator_ = "delaunay";
}


util::Domain UnstructuredGrid::domain() const {
    return util::Domain(bbox_.north(), bbox_.west(),
                        bbox_.south(), bbox_.east());
}


atlas::Grid UnstructuredGrid::atlasGrid() const {
    ASSERT(numberOfPoints());

    std::vector<atlas::PointXY> pts;
    pts.reserve(numberOfPoints());

    for (size_t i = 0; i < numberOfPoints(); ++i) {
        pts.emplace_back(atlas::PointXY(longitudes_[i], latitudes_[i]));
    }

    return atlas::grid::UnstructuredGrid(std::move(pts));
}


void UnstructuredGrid::validate(const MIRValuesVector& values) const {
    ASSERT(values.size() == numberOfPoints());
}


size_t UnstructuredGrid::numberOfPoints() const {
    ASSERT(latitudes_.size() == longitudes_.size());
    return latitudes_.size();
}


const Gridded* UnstructuredGrid::croppedRepresentation(const util::BoundingBox& bbox) const {

    std::vector<double> lat;
    std::vector<double> lon;

    size_t i = 0;
    size_t j = 0;

    eckit::ScopedPtr<repres::Iterator> iter(iterator());
    while (iter->next()) {
        if (bbox.contains(iter->pointUnrotated())) {
            lat.emplace_back(latitudes_[i]);
            lon.emplace_back(longitudes_[i]);
            ++j;
        }
        ++i;
    }

    if (j < i) {
        eckit::Log::debug<LibMir>() << "UnstructuredGrid::croppedRepresentation: cropped "
                                    << eckit::BigNum(i) << " to " << eckit::Plural(j, "point")
                                    << std::endl;
        ASSERT(j);
        return new UnstructuredGrid(lat, lon, bbox);
    }

    eckit::Log::debug<LibMir>() << "UnstructuredGrid::croppedRepresentation: no cropping" << std::endl;
    return this;
}


class UnstructuredGridIterator : public Iterator {

    size_t i_;
    const size_t size_;
    const std::vector<double>& latitudes_;
    const std::vector<double>& longitudes_;

    void print(std::ostream& out) const {
        out << "UnstructuredGridIterator[]";
    }

    bool next(Latitude& lat, Longitude& lon) {
        if (i_ < size_) {
            lat = latitudes_[i_];
            lon = longitudes_[i_];
            i_++;
            return true;
        }
        return false;
    }

public:

    UnstructuredGridIterator(const std::vector<double>& latitudes, const std::vector<double>& longitudes) :
        i_(0),
        size_(latitudes.size()),
        latitudes_(latitudes),
        longitudes_(longitudes) {
        ASSERT(latitudes_.size() == longitudes_.size());
    }

};


Iterator* UnstructuredGrid::iterator() const {
    return new UnstructuredGridIterator(latitudes_, longitudes_);
}


bool UnstructuredGrid::isPeriodicWestEast() const {
    return bbox_.east() - bbox_.west() == Longitude::GLOBE;
}


bool UnstructuredGrid::includesNorthPole() const {
    return bbox_.north() == Latitude::NORTH_POLE;
}


bool UnstructuredGrid::includesSouthPole() const {
    return bbox_.south() == Latitude::SOUTH_POLE;
}


void UnstructuredGrid::check( const std::string& title,
                              const std::vector<double>& latitudes,
                              const std::vector<double>& longitudes) {

    ASSERT(latitudes.size() == longitudes.size());
    ASSERT(!longitudes.empty());

    std::set<std::pair<double, double> > seen;
    size_t count = latitudes.size();

    for (size_t i = 0; i < count; ++i) {
        std::pair<double, double> p(latitudes[i], longitudes[i]);
        if (!seen.insert(p).second) {
            std::ostringstream oss;
            oss << title << ": duplicate point lat=" << latitudes[i] << ", lon=" << longitudes[i];
            throw eckit::UserError(oss.str());
        }
    }

}


namespace {
static RepresentationBuilder<UnstructuredGrid> triangular_grid("triangular_grid"); // Name is what is returned by grib_api
static RepresentationBuilder<UnstructuredGrid> unstructured_grid("unstructured_grid"); // Name is what is returned by grib_api
}


}  // namespace other
}  // namespace repres
}  // namespace mir

