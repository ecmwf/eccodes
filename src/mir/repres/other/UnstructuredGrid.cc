/*
 * (C) Copyright 1996-2016 ECMWF.
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

#include <iostream>
#include <fstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"

#include "atlas/grid/global/Unstructured.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/log/MIR.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace other {


UnstructuredGrid::UnstructuredGrid(const param::MIRParametrisation &parametrisation) {
    ASSERT(parametrisation.get("latitudes", latitudes_));
    ASSERT(parametrisation.get("longitudes", longitudes_));
    ASSERT(latitudes_.size() == longitudes_.size());
}

UnstructuredGrid::UnstructuredGrid(const eckit::PathName &path) {
    std::cout << "Open " << path << std::endl;
    std::ifstream in(path.asString().c_str());
    if (!in) {
        throw eckit::CantOpenFile(path);
    }
    double lat;
    double lon;
    while (in >> lat >> lon) {
        latitudes_.push_back(lat);
        longitudes_.push_back(lon);
    }
    std::cout << *this << std::endl;
}


UnstructuredGrid::~UnstructuredGrid() {
}


void UnstructuredGrid::print(std::ostream &out) const {
    out << "UnstructuredGrid[points=" << latitudes_.size()
        << "]";
}


void UnstructuredGrid::fill(grib_info &info) const  {
    NOTIMP;
}


void UnstructuredGrid::fill(api::MIRJob &job) const  {
    NOTIMP;
}


bool UnstructuredGrid::globalDomain() const {
    eckit::Log::warning() << "UnstructuredGrid::globalDomain: assuming grid is global" << std::endl;
    return true;
}


atlas::grid::Grid *UnstructuredGrid::atlasGrid() const {
    std::vector<atlas::grid::Grid::Point> *pts = new std::vector<atlas::grid::Grid::Point>();
    ASSERT(latitudes_.size() == longitudes_.size());
    pts->reserve(latitudes_.size());

    for (size_t i = 0; i < latitudes_.size(); i++) {
        pts->push_back(atlas::grid::Grid::Point(longitudes_[i], latitudes_[i]));
        if (i < 10) {
            eckit::Log::trace<MIR>() << "UnstructuredGrid::atlasGrid lon=" << longitudes_[i] << ", lat=" << latitudes_[i] << std::endl;
        }
    }

    return new atlas::grid::global::Unstructured(pts);

    // so constructor takes a vector<Point> (where point is LLPoint2)
}


void UnstructuredGrid::validate(const std::vector<double> &values) const {
    ASSERT(values.size() == latitudes_.size());
    ASSERT(values.size() == longitudes_.size());
}

const std::vector<double>& UnstructuredGrid::latitudes() const {
    return latitudes_;
}

const std::vector<double>& UnstructuredGrid::longitudes() const {
    return longitudes_;
}


namespace {
static RepresentationBuilder<UnstructuredGrid> triangular_grid("triangular_grid"); // Name is what is returned by grib_api
static RepresentationBuilder<UnstructuredGrid> unstructured_grid("unstructured_grid"); // Name is what is returned by grib_api
}


}  // namespace other
}  // namespace repres
}  // namespace mir

