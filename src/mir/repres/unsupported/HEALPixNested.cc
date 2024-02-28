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


#include "mir/repres/unsupported/HEALPixNested.h"

// #include <algorithm>
// #include <bitset>
// #include <cmath>
// #include <ostream>
// #include <tuple>
//
// #include "eckit/types/FloatCompare.h"
//
// #include "mir/repres/proxy/HEALPix.h"
// #include "mir/repres/Iterator.h"
// #include "mir/util/Atlas.h"
// #include "mir/util/Grib.h"
// #include "mir/util/MeshGeneratorParameters.h"
//
//
// #include "atlas/interpolation/method/knn/GridBox.h"
//
// #include "mir/util/Exceptions.h"


namespace mir::repres::unsupported {


bool HEALPixNested::sameAs(const Representation&) const {
    NOTIMP;
}


void HEALPixNested::makeName(std::ostream&) const {
    NOTIMP;
}


void HEALPixNested::fillGrib(grib_info&) const {
    NOTIMP;
}


void HEALPixNested::fillMeshGen(util::MeshGeneratorParameters&) const {
    NOTIMP;
}


void HEALPixNested::fillJob(api::MIRJob&) const {
    NOTIMP;
}


void HEALPixNested::print(std::ostream&) const {
    NOTIMP;
}


std::vector<util::GridBox> HEALPixNested::gridBoxes() const {
    NOTIMP;
}


::atlas::Grid HEALPixNested::atlasGrid() const {
    NOTIMP;
}


void HEALPixNested::validate(const MIRValuesVector& values) const {
    NOTIMP;
}


size_t HEALPixNested::numberOfPoints() const {
    NOTIMP;
}


Iterator* HEALPixNested::iterator() const {
    NOTIMP;
}


}  // namespace mir::repres::unsupported
