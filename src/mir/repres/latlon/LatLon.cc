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


#include "mir/repres/latlon/LatLon.h"

#include <cmath>
#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"
#include "atlas/grid/Domain.h"
#include "mir/action/misc/AreaCropper.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace latlon {


static size_t computeN(double first, double last, double inc, const char* n_name) {
    ASSERT(first <= last);
    ASSERT(inc > 0);

    size_t p = size_t((last - first) / inc);
    double d0 = fabs(last - (first + p * inc));
    double d1 = fabs(last - (first + (p + 1) * inc));
    ASSERT(d0 != d1);

    size_t n = p + (d0<d1? 0 : 1);
    // eckit::Log::debug<LibMir>() << p << " " << d0 << " " << d1 << " " << inc << " " << first << " " << last << std::endl;

    const double eps = double(std::numeric_limits<float>::epsilon());
    if (!eckit::types::is_approximately_equal(n*inc + first, last, eps)) {
        std::ostringstream os;
        os << "computeN: cannot compute accurately " << n_name << " from " << first << "/to/" << last << "/by/" << inc;
        eckit::Log::debug<LibMir>() << os.str() << std::endl;
        throw eckit::BadValue(os.str());
    }

    return n + 1;
}


LatLon::LatLon(const param::MIRParametrisation &parametrisation) :
    bbox_(parametrisation),
    increments_(parametrisation) {
    ASSERT(parametrisation.get("Ni", ni_));
    ASSERT(parametrisation.get("Nj", nj_));
}


LatLon::LatLon(const util::BoundingBox &bbox, const util::Increments &increments) :
    bbox_(bbox),
    increments_(increments) {
    setNiNj();
}


LatLon::~LatLon() {
}


void LatLon::cropToDomain(const param::MIRParametrisation &parametrisation, context::Context & ctx) const {
    if (!atlasDomain().isGlobal()) {
        action::AreaCropper cropper(parametrisation, bbox_);
        cropper.execute(ctx);
    }
}


void LatLon::setNiNj() {
    ni_ = computeN(bbox_.west(),  bbox_.east(),  increments_.west_east(),   "Ni");
    nj_ = computeN(bbox_.south(), bbox_.north(), increments_.south_north(), "Nj");
}


void LatLon::reorder(long scanningMode, std::vector<double> &values) const {
    // Code from ecRegrid, UNTESTED!!!

    eckit::Log::debug<LibMir>() << "WARNING: UNTESTED!!! ";
    eckit::Log::debug<LibMir>() << "LatLon::reorder scanning mode 0x" << std::hex << scanningMode << std::dec << std::endl;

    ASSERT(values.size() == ni_ * nj_);

    std::vector<double> out(values.size());

    if (scanningMode == jScansPositively) {
        size_t count = 0;
        for (int j = nj_ - 1 ; j >= 0; --j) {
            for (size_t i = 0 ; i <  ni_; ++i) {
                out[count++] = values[j * ni_ + i];
            }
        }
        ASSERT(count == out.size());
        std::swap(values, out);
        return;
    }

    if (scanningMode == iScansNegatively) {
        size_t count = 0;
        for (size_t j = 0  ; j < nj_; ++j) {
            for (int i = ni_ - 1 ; i >= 0; --i) {
                out[count++] = values[j * ni_ + i];
            }
        }
        ASSERT(count == out.size());
        std::swap(values, out);
        return;
    }

    if (scanningMode == (iScansNegatively | jScansPositively)) {
        size_t count = 0;
        for (int j = nj_ - 1  ; j >= 0; --j) {
            for (int i = ni_ - 1 ; i >= 0; --i) {
                out[count++] = values[j * ni_ + i];
            }
        }
        ASSERT(count == out.size());
        std::swap(values, out);
        return;
    }

    std::ostringstream os;
    os << "LatLon::reorder: unsupported scanning mode 0x" << std::hex << scanningMode;
    throw eckit::SeriousBug(os.str());
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


void LatLon::fill(api::MIRJob &job) const  {
    increments_.fill(job);
    bbox_.fill(job);
}


class LatLonIterator : public Iterator {

    size_t ni_;
    size_t nj_;

    double north_;
    double west_;

    double we_;
    double ns_;

    size_t i_;
    size_t j_;

    size_t count_;

    virtual void print(std::ostream &out) const {
        out << "LatLonIterator["
            <<  "ni="     << ni_
            << ",nj="     << nj_
            << ",north="  << north_
            << ",west="   << west_
            << ",we="     << we_
            << ",ns="     << ns_
            << ",i="      << i_
            << ",j="      << j_
            << ",count="  << count_
            << "]";
    }

    virtual bool next(double &lat, double &lon) {
        if (j_ < nj_) {
            if (i_ < ni_) {
                lat = north_ - j_ * ns_; // This is slower, but looks more precise
                lon = west_  + i_ * we_; // This is slower, but looks more precise
                i_++;
                if (i_ == ni_) {
                    j_++;
                    i_ = 0;
                }
                count_++;
                return true;
            }
        }
        return false;
    }

public:
    LatLonIterator(size_t ni,
                   size_t nj,
                   double north,
                   double west,
                   double we,
                   double ns):
        ni_(ni),
        nj_(nj),
        north_(north),
        west_(west),
        we_(we),
        ns_(ns),
        i_(0),
        j_(0),
        count_(0) {

    }

    ~LatLonIterator() {
        ASSERT(count_ == ni_ * nj_);
    }

};


Iterator *LatLon::unrotatedIterator() const {
    return new LatLonIterator(ni_, nj_, bbox_.north(), bbox_.west(), increments_.west_east(), increments_.south_north());
}


Iterator* LatLon::rotatedIterator() const {
    return unrotatedIterator();
}


size_t LatLon::frame(std::vector<double> &values, size_t size, double missingValue) const {

    // Could be done better, just a demo
    validate(values);

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


void LatLon::validate(const std::vector<double> &values) const {
    eckit::Log::debug<LibMir>() << "LatLon::validate " << values.size() << " ni*nj " << ni_ * nj_ << std::endl;
    ASSERT(values.size() == ni_ * nj_);
}


void LatLon::shape(size_t &ni, size_t &nj) const {
    ni = ni_;
    nj = nj_;
}


atlas::grid::Domain LatLon::atlasDomain() const {
    return atlasDomain(bbox_);
}


atlas::grid::Domain LatLon::atlasDomain(const util::BoundingBox& bbox) const {


    // Special case for shifted grids
    const double ns = bbox.north() - bbox.south() ;
    const double ew = bbox.east()  - bbox.west() ;

    const bool isPeriodicEastWest = eckit::types::is_approximately_equal(ew + increments_.west_east(), 360.);
    const bool includesPoles = eckit::types::is_approximately_equal(ns, 180.)
                            || eckit::types::is_approximately_equal(ns + increments_.south_north(), 180.);

    const double
            north = includesPoles?   90 : bbox.north(),
            south = includesPoles?  -90 : bbox.south(),
            west = bbox.west(),
            east = isPeriodicEastWest? bbox.west() + 360 : bbox.east();
    return atlas::grid::Domain(north, west, south, east);
}


}  // namespace latlon
}  // namespace repres
}  // namespace mir

