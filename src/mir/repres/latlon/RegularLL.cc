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


#include "mir/repres/latlon/RegularLL.h"

#include <iostream>
#include "atlas/grid.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace latlon {


RegularLL::RegularLL(const param::MIRParametrisation& parametrisation) :
    LatLon(parametrisation) {
}


RegularLL::RegularLL(const util::BoundingBox& bbox,
                     const util::Increments& increments) :
    LatLon(bbox, increments) {
}


RegularLL::~RegularLL() {
}


Iterator* RegularLL::iterator() const {

    class RegularLLIterator : protected LatLonIterator, public Iterator {
        void print(std::ostream& out) const {
            out << "RegularLLIterator[";
            Iterator::print(out);
            out << ",";
            LatLonIterator::print(out);
            out << "]";
        }
        bool next(Latitude& lat, Longitude& lon) {
            return LatLonIterator::next(lat, lon);
        }
    public:
        RegularLLIterator(size_t ni, size_t nj, Latitude north, Longitude west, double we, double ns) :
            LatLonIterator(ni, nj, north, west, we, ns) {
        }
    };

    return new RegularLLIterator(ni_, nj_, bbox_.north(), bbox_.west(), increments_.west_east(), increments_.south_north());
}


void RegularLL::print(std::ostream &out) const {
    out << "RegularLL[";
    LatLon::print(out);
    out << "]";
}


atlas::Grid RegularLL::atlasGrid() const {

    // NOTE: for non-shifted/shifted grid, yspace uses bounding box
    // (this works together with the Atlas RectangularDomain cropping)
    const util::Domain dom = domain();

    using atlas::grid::StructuredGrid;
    using atlas::grid::LinearSpacing;
    StructuredGrid::XSpace xspace( LinearSpacing( dom.west().value(),  dom.east().value(),  long(ni_), !dom.isPeriodicEastWest() ));
    StructuredGrid::YSpace yspace( LinearSpacing( bbox_.north().value(), bbox_.south().value(), long(nj_) ));

    return StructuredGrid(xspace, yspace, StructuredGrid::Projection(), domain());
}


void RegularLL::fill(grib_info& info) const  {
    LatLon::fill(info);

    // See copy_spec_from_ksec.c in libemos for info
    // Warning: scanning mode not considered
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_LL;
}


void RegularLL::fill(api::MIRJob& job) const  {
    LatLon::fill(job);
}


void RegularLL::makeName(std::ostream& out) const {
    LatLon::makeName(out);
}


bool RegularLL::sameAs(const Representation& other) const {
    const RegularLL* o = dynamic_cast<const RegularLL*>(&other);
    return o && LatLon::sameAs(other);
}


const RegularLL* RegularLL::cropped(const util::BoundingBox& bbox) const {
    // Called by AreaCropper::execute
    return new RegularLL(bbox, increments_);
}


namespace {
static RepresentationBuilder<RegularLL> regularLL("regular_ll"); // Name is what is returned by grib_api
}


}  // namespace latlon
}  // namespace repres
}  // namespace mir

