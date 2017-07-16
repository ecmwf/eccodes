/*
 * (C) Copyright 1996-2015 ECMWF.
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


#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/unsupported/IrregularLatlon.h"
#include "eckit/utils/MD5.h"


namespace mir {
namespace repres {


IrregularLatlon::IrregularLatlon(const param::MIRParametrisation &parametrisation) {
    ASSERT(parametrisation.get("latitudes", latitudes_));
    ASSERT(parametrisation.get("longitudes", longitudes_));
}


IrregularLatlon::IrregularLatlon() {
}


IrregularLatlon::~IrregularLatlon() {
}

size_t IrregularLatlon::numberOfPoints() const {
    return latitudes_.size() * longitudes_.size();
}


void IrregularLatlon::print(std::ostream &out) const {
    out << "IrregularLatlon[latitudes=" << latitudes_.size()
        << ",longitudes=" << longitudes_.size()
        << "]";
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
    const IrregularLatlon* o = dynamic_cast<const IrregularLatlon*>(&other);
    return o && (latitudes_ == o->latitudes_) && (longitudes_ == o->longitudes_) ;
}


void IrregularLatlon::fill(grib_info &info) const  {
    NOTIMP;
}



atlas::Grid IrregularLatlon::atlasGrid() const {

    std::vector<atlas::PointXY> *pts = new std::vector<atlas::PointXY>();
    pts->reserve(latitudes_.size() * longitudes_.size());

    for(double lat: latitudes_) {
        for(double lon: longitudes_) {
             pts->push_back(atlas::PointXY(lon, lat));
        }
    }

    return atlas::grid::UnstructuredGrid(pts);
}

namespace {
static RepresentationBuilder<IrregularLatlon> irregularLatlon("irregular_latlon"); // Name is what is returned by grib_api
}


}  // namespace repres
}  // namespace mir

