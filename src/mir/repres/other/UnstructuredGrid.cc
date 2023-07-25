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


#include "mir/repres/other/UnstructuredGrid.h"

#include <fstream>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <utility>

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/serialisation/FileStream.h"
#include "eckit/serialisation/IfstreamStream.h"
#include "eckit/utils/MD5.h"

#include "mir/api/MIRJob.h"
#include "mir/api/mir_config.h"
#include "mir/iterator/UnstructuredIterator.h"
#include "mir/key/grid/Grid.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/CheckDuplicatePoints.h"
#include "mir/util/Domain.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/Log.h"
#include "mir/util/MeshGeneratorParameters.h"
#include "mir/util/Types.h"

#if mir_HAVE_ATLAS
#include "mir/key/grid/ORCAPattern.h"
#include "mir/repres/proxy/ORCA.h"
#endif


namespace mir::repres {


template <>
Representation* RepresentationBuilder<other::UnstructuredGrid>::make(const param::MIRParametrisation& param) {
#if mir_HAVE_ATLAS
    // specially-named unstructured grids
    std::string grid;
    if (param.get("grid", grid)) {
        if (!key::grid::ORCAPattern::match(grid, param).empty()) {
            return new proxygrid::ORCA(param);
        }
    }
#endif

    return new other::UnstructuredGrid(param);
}


namespace other {


UnstructuredGrid::UnstructuredGrid(const param::MIRParametrisation& parametrisation) {
    parametrisation.get("latitudes", latitudes_);
    parametrisation.get("longitudes", longitudes_);

    if (latitudes_.empty() || longitudes_.empty()) {
        throw exception::UserError("UnstructuredGrid: requires 'latitudes' and 'longitudes'");
    }
    ASSERT(latitudes_.size() == longitudes_.size());

    util::check_duplicate_points("UnstructuredGrid from MIRParametrisation", latitudes_, longitudes_, parametrisation);
}


UnstructuredGrid::UnstructuredGrid(const eckit::PathName& path) {
    std::ifstream in(path.asString().c_str());
    if (!in) {
        throw exception::CantOpenFile(path);
    }

    if (::isprint(in.peek()) == 0) {

        Log::info() << "UnstructuredGrid::load  " << path << std::endl;

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
            // Log::info() << latitudes_[i] << " " << longitudes_[i] << std::endl;
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

    util::check_duplicate_points("UnstructuredGrid from " + path.asString(), latitudes_, longitudes_);
}


void UnstructuredGrid::save(const eckit::PathName& path, const std::vector<double>& latitudes,
                            const std::vector<double>& longitudes, bool binary) {
    Log::info() << "UnstructuredGrid::save " << path << std::endl;

    util::check_duplicate_points("UnstructuredGrid::save to " + path.asString(), latitudes, longitudes);

    ASSERT(latitudes.size() == longitudes.size());
    if (binary) {
        eckit::FileStream s(path, "w");
        size_t version = 1;
        size_t count   = latitudes.size();
        s << version;
        s << count;
        for (size_t i = 0; i < count; ++i) {
            s << latitudes[i];
            s << longitudes[i];

            Log::info() << latitudes[i] << " " << longitudes[i] << std::endl;
        }
        s.close();
    }
    else {
        NOTIMP;
    }
}


UnstructuredGrid::UnstructuredGrid(const std::vector<double>& latitudes, const std::vector<double>& longitudes,
                                   const util::BoundingBox& bbox) :
    Gridded(bbox), latitudes_(latitudes), longitudes_(longitudes) {
    ASSERT(latitudes_.size() == longitudes_.size());
    util::check_duplicate_points("UnstructuredGrid from arguments", latitudes_, longitudes_);
}


UnstructuredGrid::~UnstructuredGrid() = default;


void UnstructuredGrid::print(std::ostream& out) const {
    out << "UnstructuredGrid[points=" << numberOfPoints() << "]";
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
    const auto* o = dynamic_cast<const UnstructuredGrid*>(&other);
    return (o != nullptr) && (latitudes_ == o->latitudes_) && (longitudes_ == o->longitudes_);
}


void UnstructuredGrid::fillGrib(grib_info& info) const {
    info.grid.grid_type        = CODES_UTIL_GRID_SPEC_UNSTRUCTURED;
    info.packing.editionNumber = 2;
}


void UnstructuredGrid::fillJob(api::MIRJob& job) const {
    job.set("latitudes", latitudes_);
    job.set("longitudes", longitudes_);
}


void UnstructuredGrid::fillMeshGen(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "delaunay";
    }
}


util::Domain UnstructuredGrid::domain() const {
    // FIXME Should be global?
    return util::Domain(bbox_.north(), bbox_.west(), bbox_.south(), bbox_.east());
}


atlas::Grid UnstructuredGrid::atlasGrid() const {
    ASSERT(numberOfPoints());

    std::vector<atlas::PointXY> pts;
    pts.reserve(numberOfPoints());

    for (size_t i = 0; i < numberOfPoints(); ++i) {
        pts.emplace_back(atlas::PointXY(longitudes_[i], latitudes_[i]));
    }

    return atlas::UnstructuredGrid(std::move(pts));
}


void UnstructuredGrid::validate(const MIRValuesVector& values) const {
    ASSERT_VALUES_SIZE_EQ_ITERATOR_COUNT("UnstructuredGrid", values.size(), numberOfPoints());
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

    for (const std::unique_ptr<Iterator> iter(iterator()); iter->next(); ++i) {
        if (bbox.contains(iter->pointUnrotated())) {
            auto ip = iter->index();
            lat.emplace_back(latitudes_.at(ip));
            lon.emplace_back(longitudes_.at(ip));
            ++j;
        }
    }

    if (j < i) {
        Log::debug() << "UnstructuredGrid::croppedRepresentation: cropped " << Log::Pretty(i) << " to "
                     << Log::Pretty(j, {"point"}) << std::endl;
        ASSERT(j);
        return new UnstructuredGrid(lat, lon, bbox);
    }

    Log::debug() << "UnstructuredGrid::croppedRepresentation: no cropping" << std::endl;
    return this;
}


Iterator* UnstructuredGrid::iterator() const {
    return new iterator::UnstructuredIterator(latitudes_, longitudes_);
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


bool UnstructuredGrid::extendBoundingBoxOnIntersect() const {
    return false;
}


static const RepresentationBuilder<UnstructuredGrid> triangular_grid("triangular_grid");
static const RepresentationBuilder<UnstructuredGrid> unstructured_grid("unstructured_grid");


}  // namespace other


}  // namespace mir::repres
