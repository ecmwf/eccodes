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
#include "eckit/log/Log.h"
#include "eckit/utils/Translator.h"

#include "atlas/Grid.h"
#include "atlas/GridSpec.h"
#include "atlas/grids/LonLatGrid.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"

#include "mir/repres/RegularLL.h"


namespace mir {
namespace repres {


RegularLL::RegularLL(const param::MIRParametrisation &parametrisation):
    bbox_(parametrisation) {

    eckit::Translator<std::string, double> s2d;
    std::string value;

    ASSERT(parametrisation.get("north_south_increment", value));
    north_south_increment_ = s2d(value);

    ASSERT(parametrisation.get("west_east_increment", value));
    west_east_increment_ = s2d(value);

    setNiNj();
}


RegularLL::RegularLL(const util::BoundingBox &bbox,
                     double north_south_increment,
                     double west_east_increment):
    bbox_(bbox),
    north_south_increment_(north_south_increment),
    west_east_increment_(west_east_increment) {
    setNiNj();
}


RegularLL::~RegularLL() {
}


void RegularLL::setNiNj() {
    double ni = (bbox_.east() - bbox_.west()) / west_east_increment_;
    ASSERT(ni > 0);
    ASSERT(long(ni) == ni);
    ni_ = ni + 1;

    double nj = (bbox_.north() - bbox_.south()) / north_south_increment_;
    ASSERT(nj > 0);
    ASSERT(long(nj) == nj);
    nj_ = nj + 1;
}


void RegularLL::print(std::ostream &out) const {
    out << "RegularLL["

        << "bbox=" << bbox_
        << ",north_south_increment=" << north_south_increment_
        << ",west_east_increment=" << west_east_increment_

        << ",ni=" << ni_
        << ",nj=" << nj_

        << "]";
}


void RegularLL::fill(grib_info &info) const  {
    // See copy_spec_from_ksec.c in libemos for info
    // Warning: scanning mode not considered

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_LL;

    info.grid.Ni = ni_;
    info.grid.Nj = nj_;

    info.grid.iDirectionIncrementInDegrees = west_east_increment_;
    info.grid.jDirectionIncrementInDegrees = north_south_increment_;

    bbox_.fill(info);

}


Representation *RegularLL::crop(const util::BoundingBox &bbox, const std::vector<double> &in, std::vector<double> &out) const {


    out.clear();
    out.reserve(in.size()); // Over-estimation

    ASSERT((ni() * nj()) == in.size());

    double n = 0;
    double s = 0;
    double e = 0;
    double w = 0;

    size_t p = 0;
    double lat = bbox_.north();
    for (size_t i = 0; i < ni_; i++, lat -= north_south_increment_) {
        double lon = bbox_.west();
        for (size_t j = 0; j < nj_; j++, lon += west_east_increment_) {
            if ( bbox.contains( lat, lon)) {

                if(out.size() == 0) {
                    n = s = lat;
                    w = e = lon;
                } else {
                    n = std::max(n, lat);
                    s = std::min(s, lat);
                    e = std::max(e, lon);
                    w = std::min(w, lon);
                }

                out.push_back(in[p]);
            }
            p++;
        }
    }

    RegularLL *cropped = new RegularLL(util::BoundingBox(n, w, s, e), north_south_increment_, north_south_increment_);

    ASSERT(out.size() > 0);
    ASSERT(cropped->ni() * cropped->nj() == out.size());
    ASSERT(p == in.size());

    return cropped;
}


atlas::Grid *RegularLL::atlasGrid() const {

    return new atlas::grids::LonLatGrid(west_east_increment_,
                                        north_south_increment_,
                                        atlas::grids::LonLatGrid::INCLUDES_POLES);
}


size_t RegularLL::frame(std::vector<double> &values, size_t size, double missingValue) const {

    // Could be done better, just a demo
    ASSERT((ni() * nj()) == values.size());
    size_t count = 0;

    size_t k = 0;
    for (size_t j = 0; j < nj_; j++) {
        for (size_t i = 0; i < ni_; i++) {
            if ( !((i < size) || (j < size) || (i >= ni_ - size) || (j >= nj_ - size))) { // Check me, may be buggy
                values[k] = missingValue;
                count++;
            }
            k++;
        }
    }

    ASSERT(k == values.size());
    return count;

}


namespace {
static RepresentationBuilder<RegularLL> regularLL("regular_ll"); // Name is what is returned by grib_api
}


}  // namespace repres
}  // namespace mir

