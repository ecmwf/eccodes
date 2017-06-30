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
#include "eckit/exception/Exceptions.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace repres {
namespace latlon {


RotatedLL::RotatedLL(const param::MIRParametrisation &parametrisation):
    RegularLL(parametrisation),
    rotation_(parametrisation) {
}


RotatedLL::RotatedLL(const util::BoundingBox &bbox,
    const util::Increments &increments,
    const util::Shift &shift,
    const util::Rotation &rotation):
    RegularLL(bbox, increments, shift),
    rotation_(rotation) {
}


RotatedLL::~RotatedLL() {
}


void RotatedLL::print(std::ostream &out) const {
    out << "RotatedLL[";
    RegularLL::print(out);
    out << ",rotation=" << rotation_
        << "]";
}


void RotatedLL::makeName(std::ostream& out) const {
    RegularLL::makeName(out);
    rotation_.makeName(out);
}

bool RotatedLL::sameAs(const Representation& other) const {

    const RotatedLL* o = dynamic_cast<const RotatedLL*>(&other);
    return o && (rotation_ == o->rotation_) && RegularLL::sameAs(other);

}


// Called by RegularLL::crop()
const RotatedLL *RotatedLL::cropped(const util::BoundingBox &bbox) const {
    eckit::Log::debug<LibMir>() << "Create cropped copy as RotatedLL bbox=" << bbox << std::endl;
    return new RotatedLL(bbox, increments_, shift_, rotation_);
}


Iterator *RotatedLL::iterator() const {

    class RotatedLLIterator : protected LatLonIterator, public Iterator {
        void print(std::ostream& out) const {
            out << "RotatedLLIterator[";
            Iterator::print(out);
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
            Iterator(rotation) {
        }
    };

    return new RotatedLLIterator(ni_, nj_, bbox_.north(), bbox_.west(), increments_.west_east(), increments_.south_north(), rotation_);
}


void RotatedLL::fill(grib_info &info) const  {
    RegularLL::fill(info);
    rotation_.fill(info);
}


void RotatedLL::fill(api::MIRJob &job) const  {
    RegularLL::fill(job);
    rotation_.fill(job);
}


atlas::Grid RotatedLL::atlasGrid() const {
    return rotation_.rotate(RegularLL::atlasGrid());;
}


namespace {
static RepresentationBuilder<RotatedLL> rotatedLL("rotated_ll");  // Name is what is returned by grib_api
}


}  // namespace latlon
}  // namespace repres
}  // namespace mir

