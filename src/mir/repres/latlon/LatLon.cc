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

#include "mir/repres/latlon/LatLon.h"
#include "mir/repres/Iterator.h"
#include "mir/param/MIRParametrisation.h"

#include <iostream>

#include "atlas/grids/LonLatGrid.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Timer.h"

#include "mir/util/Grib.h"
#include "mir/util/Compare.h"
#include "mir/action/misc/AreaCropper.h"

namespace mir {
namespace repres {
namespace latlon {

LatLon::LatLon(const param::MIRParametrisation &parametrisation):
    bbox_(parametrisation), increments_(parametrisation) {
    ASSERT(parametrisation.get("Ni", ni_));
    ASSERT(parametrisation.get("Nj", nj_));

}


LatLon::LatLon(const util::BoundingBox &bbox,
               const util::Increments &increments):
    bbox_(bbox),
    increments_(increments) {
    setNiNj();
}


LatLon::~LatLon() {
}

bool LatLon::globalDomain() const {

    // Special case for shifted grids
    double ns = bbox_.north() - bbox_.south() ;
    double ew = bbox_.east() - bbox_.west() ;

    bool all_lons = eckit::FloatCompare<double>::isApproximatelyEqual(ew + increments_.west_east() , 360);
    bool all_lats = eckit::FloatCompare<double>::isApproximatelyEqual(ns, 180) || eckit::FloatCompare<double>::isApproximatelyEqual(ns + increments_.south_north(), 180);

    return all_lats && all_lons;
    //     if (all_lats && all_lons)  {
    //         eckit::Log::info() << "WARNING: global shifted grid (before): " << bbox_ << " ===== " << increments_ << std::endl;
    //         bbox_ = util::BoundingBox::Global(bbox_.north(), bbox_.west(), bbox_.south(), bbox_.east());
    //         eckit::Log::info() << "WARNING: global shifted grid (after): " << bbox_ << " ===== " << increments_ << std::endl;
    //     }

    // }
}


void LatLon::cropToDomain(const param::MIRParametrisation &parametrisation, data::MIRField &field) const {
    if (!globalDomain()) {
        action::AreaCropper cropper(parametrisation, bbox_);
        cropper.execute(field);
    }
}
// size_t LatLon::ni() const {
//     return ni_;
// }

// size_t LatLon::nj() const {
//     return nj_;
// }


void LatLon::setNiNj() {

    computeNiNj(ni_, nj_, bbox_, increments_);
}

void LatLon::computeNiNj(size_t &ni,
                         size_t &nj,
                         const util::BoundingBox &bbox,
                         const util::Increments &increments) {

    ni = computeN(bbox.west(), bbox.east(), increments.west_east(), "Ni", "west", "east");
    nj = computeN(bbox.south(), bbox.north(), increments.south_north(), "Nj", "south", "north");
}

void LatLon::reorder(long scanningMode, std::vector<double> &values) const {
    // Code from ecRegrid, UNTESTED!!!

    eckit::Log::info() << "WARNING: UNTESTED!!! ";
    eckit::Log::info() << "LatLon::reorder scanning mode 0x" << std::hex << scanningMode << std::dec << std::endl;

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

    eckit::StrStream os;
    os << "LatLon::reorder: unsupported scanning mode 0x" << std::hex << scanningMode << eckit::StrStream::ends;
    throw eckit::SeriousBug(os);
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

class LatLonIterator: public Iterator {
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
        out << "LatLonIterator[]";
    }

    virtual bool next(double &lat, double &lon) {
        if (j_ < nj_) {
            if (i_ < ni_) {
                lat = north_ - j_ * ns_; // This is slower, but looks more precise
                lon = west_ + i_ * we_; // This is slower, but looks more precise
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

Iterator *LatLon::iterator() const {
    return new LatLonIterator(ni_, nj_, bbox_.north(), bbox_.west(), increments_.west_east(), increments_.south_north());
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
    eckit::Log::info() << "LatLon::validate " << values.size() << " ni*nj " << ni_ * nj_ << std::endl;
    ASSERT(values.size() == ni_ * nj_);
}

void LatLon::shape(size_t &ni, size_t &nj) const {
    ni = ni_;
    nj = nj_;
}


}  // namespace latlon
}  // namespace repres
}  // namespace mir

