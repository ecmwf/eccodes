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
/// @LatLon Pedro Maciel
/// @date Apr 2015

#include "mir/repres/LatLon.h"

#include <iostream>

#include "atlas/grids/LonLatGrid.h"

#include "eckit/exception/Exceptions.h"

#include "mir/util/Grib.h"


namespace mir {
namespace repres {


LatLon::LatLon(const param::MIRParametrisation &parametrisation):
    bbox_(parametrisation), increments_(parametrisation) {
    setNiNj();
}


LatLon::LatLon(const util::BoundingBox &bbox,
               const util::Increments &increments):
    bbox_(bbox),
    increments_(increments) {
    setNiNj();
}


LatLon::~LatLon() {
}


static size_t compteN(double first, double last, double inc, const char *n_name, const char *first_name, const char *last_name) {
    size_t n;
    ASSERT(first <= last);
    ASSERT(inc > 0);
    size_t p = size_t((last - first) / inc);
    double d0 = fabs((last + p * inc) - first);
    double d1 = fabs((last + (p + 1) * inc) - first);

    eckit::Log::info() << p << " " << d0 << " " << d1 << " " << inc << " " << first << " " << last << std::endl;
    ASSERT(d0 != d1);

    if (d0 < d1) {
        n = p;
    } else {
        n = p + 1;
    }

    if ((p * inc + first) != last) {
        eckit::Log::info() << "LatLon: cannot compute accuratly "
                           << n_name << ", given "
                           << first_name << "=" << first << ", "
                           << last_name << "=" << last << " and increment=" << inc << std::endl;
        eckit::Log::info() << "Last value is computed as " << (p * inc + first)
                           << ", diff=" << (last - (p * inc + first))
                           << std::endl;
    }

    return n + 1;

}

void LatLon::setNiNj() {

    ni_ = compteN(bbox_.west(), bbox_.east(), increments_.west_east(), "Ni", "west", "east");
    nj_ = compteN(bbox_.south(), bbox_.north(), increments_.south_north(), "Nj", "south", "north");

}


void LatLon::print(std::ostream &out) const {
    out << "bbox=" << bbox_
        << ",increments=" << increments_
        << ",ni=" << ni_
        << ",nj=" << nj_
        ;
}


void LatLon::fill(grib_info &info) const  {
    // See copy_spec_from_ksec.c in libemos for info
    // Warning: scanning mode not considered

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_LL;

    info.grid.Ni = ni_;
    info.grid.Nj = nj_;

    increments_.fill(info);
    bbox_.fill(info);

}


Representation *LatLon::crop(const util::BoundingBox &bbox, const std::vector<double> &in, std::vector<double> &out) const {


    out.clear();
    out.reserve(in.size()); // Over-estimation

    ASSERT((ni() * nj()) == in.size());

    double n = 0;
    double s = 0;
    double e = 0;
    double w = 0;

    double ns = increments_.south_north();
    double we = increments_.west_east();

    size_t p = 0;
    double lat = bbox_.north();
    for (size_t j = 0; j < nj_; j++, lat -= ns) {
        double lon = bbox_.west();
        for (size_t i = 0; i < ni_; i++, lon += we) {
            if (bbox.contains(lat, lon)) {

                lon = bbox.normalise(lon);

                if (out.size() == 0) {
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

    std::cout << "CROP resulting bbox is: " << util::BoundingBox(n, w, s, e) << std::endl;
    LatLon *cropped =  this->cropped(util::BoundingBox(n, w, s, e));

    ASSERT(out.size() > 0);
    ASSERT(cropped->ni() * cropped->nj() == out.size());
    ASSERT(p == in.size());

    return cropped;
}



size_t LatLon::frame(std::vector<double> &values, size_t size, double missingValue) const {

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



}  // namespace repres
}  // namespace mir

