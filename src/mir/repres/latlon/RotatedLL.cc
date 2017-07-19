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


#include "mir/repres/latlon/RotatedLL.h"

#include <iostream>

#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace latlon {


RotatedLL::RotatedLL(const param::MIRParametrisation& parametrisation) :
    LatLon(parametrisation),
    rotation_(parametrisation) {
}


RotatedLL::RotatedLL(
        const util::BoundingBox& bbox,
        const util::Increments& increments,
        const util::Rotation& rotation ) :
    LatLon(bbox, increments),
    rotation_(rotation) {
}


RotatedLL::~RotatedLL() {
}


Iterator *RotatedLL::iterator() const {

    class RotatedLLIterator : protected LatLonIterator, public RotatedIterator {
        void print(std::ostream& out) const {
            out << "RotatedLLIterator[";
            RotatedIterator::print(out);
            out << ",";
            LatLonIterator::print(out);
            out << "]";
        }
        bool next(Latitude& lat, Longitude& lon) {
            return LatLonIterator::next(lat, lon);
        }
    public:
        RotatedLLIterator(size_t ni, size_t nj, Latitude north, Longitude west, double we, double ns, const util::Rotation& rotation) :
            LatLonIterator(ni, nj, north, west, we, ns),
            RotatedIterator(rotation) {
        }
    };

    return new RotatedLLIterator(ni_, nj_, bbox_.north(), bbox_.west(), increments_.west_east(), increments_.south_north(), rotation_);
}


void RotatedLL::print(std::ostream& out) const {
    out << "RotatedLL[";
    LatLon::print(out);
    out << ",rotation=" << rotation_
        << "]";
}


atlas::Grid RotatedLL::atlasGrid() const {

    // NOTE: for non-shifted/shifted grid, yspace uses bounding box
    // (this works together with the Atlas RectangularDomain cropping)
    const util::Domain dom = domain();

    using atlas::grid::StructuredGrid;
    using atlas::grid::LinearSpacing;
    StructuredGrid::XSpace xspace( LinearSpacing( dom.west().value(),  dom.east().value(),  long(ni_), !dom.isPeriodicEastWest() ));
    StructuredGrid::YSpace yspace( LinearSpacing( bbox_.north().value(), bbox_.south().value(), long(nj_) ));

    StructuredGrid unrotatedGrid(xspace, yspace, StructuredGrid::Projection(), domain());
    return rotation_.rotate(unrotatedGrid);
}


void RotatedLL::fill(grib_info& info) const  {
    LatLon::fill(info);

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_ROTATED_LL;
    rotation_.fill(info);
}


void RotatedLL::fill(api::MIRJob& job) const  {
    LatLon::fill(job);
    rotation_.fill(job);
}


void RotatedLL::makeName(std::ostream& out) const {
    LatLon::makeName(out);
    rotation_.makeName(out);
}

bool RotatedLL::sameAs(const Representation& other) const {

    const RotatedLL* o = dynamic_cast<const RotatedLL*>(&other);
    return o && (rotation_ == o->rotation_) && LatLon::sameAs(other);
}


const RotatedLL* RotatedLL::cropped(const util::BoundingBox& bbox) const {
    // Called by AreaCropper::execute and GlobaliseFilter::execute
    return new RotatedLL(bbox, increments_, rotation_);
}


namespace {
static RepresentationBuilder<RotatedLL> rotatedLL("rotated_ll");  // Name is what is returned by grib_api
}


}  // namespace latlon
}  // namespace repres
}  // namespace mir

