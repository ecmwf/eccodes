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

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Plural.h"
#include "eckit/serialisation/FileStream.h"
#include "eckit/serialisation/IfstreamStream.h"
#include "eckit/utils/MD5.h"

#include "mir/action/misc/AreaCropper.h"
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
            eckit::Log::info() << latitudes_[i] << " " << longitudes_[i] << std::endl;

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
                                   const std::vector<double>& longitudes) :
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
    eckit::Log::warning() << "UnstructuredGrid::domain(): assuming global" << std::endl;
    return util::Domain();
}


atlas::Grid UnstructuredGrid::atlasGrid() const {
    ASSERT(numberOfPoints());

    std::vector<atlas::PointXY>* pts = new std::vector<atlas::PointXY>();
    pts->reserve(numberOfPoints());

    for (size_t i = 0; i < numberOfPoints(); i++) {
        pts->push_back(atlas::PointXY(longitudes_[i], latitudes_[i]));
        if (i < 10) {
            eckit::Log::debug<LibMir>() << "UnstructuredGrid::atlasGrid lon="
                                        << longitudes_[i]
                                        << ", lat="
                                        << latitudes_[i]
                                        << std::endl;
        }
    }

    return atlas::grid::UnstructuredGrid(pts);
}


void UnstructuredGrid::validate(const MIRValuesVector& values) const {
    ASSERT(values.size() == numberOfPoints());
}


size_t UnstructuredGrid::numberOfPoints() const {
    ASSERT(latitudes_.size() == longitudes_.size());
    return latitudes_.size();
}


const Gridded* UnstructuredGrid::croppedRepresentation(const util::BoundingBox& bbox) const {

    std::vector<size_t> mapping;
    util::BoundingBox small(bbox);
    action::AreaCropper::crop(*this, small, mapping);
    ASSERT(bbox.contains(small));

    eckit::Log::debug<LibMir>() << "UnstructuredGrid::croppedRepresentation: cropped "
                                << eckit::BigNum(numberOfPoints()) << " to " << eckit::Plural(mapping.size(), "point")
//                                << "\n\t" "   " << bbox
//                                << "\n\t" " > " << small
                                << std::endl;

    if (mapping.size() < numberOfPoints()) {
        ASSERT(!mapping.empty());

        std::vector<double> lat;
        std::vector<double> lon;

        for (const auto& j : mapping) {
            lat.push_back(latitudes_[j]);
            lon.push_back(longitudes_[j]);
        }

        return new UnstructuredGrid(lat, lon);
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
    // TODO:
    return true;
}


bool UnstructuredGrid::includesNorthPole() const {
    // TODO:
    return true;
}


bool UnstructuredGrid::includesSouthPole() const {
    // TODO:
    return true;
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

