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

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"
#include "mir/action/misc/AreaCropper.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"

#include "atlas/library/config.h"

#ifdef ATLAS_HAVE_TRANS
#include "transi/trans.h"
#endif

namespace mir {
namespace repres {
namespace latlon {


LatLon::LatLon(const param::MIRParametrisation& parametrisation) :
    Gridded(parametrisation),
    increments_(parametrisation) {
    ASSERT(parametrisation.get("Ni", ni_));
    ASSERT(parametrisation.get("Nj", nj_));
}


LatLon::LatLon(const util::BoundingBox& bbox, const util::Increments& increments) :
    Gridded(bbox),
    increments_(increments) {
    setNiNj();
}


LatLon::~LatLon() {
}


void LatLon::setNiNj() {
    ni_ = bbox_.computeNi(increments_);
    nj_ = bbox_.computeNj(increments_);
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

void LatLon::makeName(std::ostream& out) const {
    out << "LL";
    increments_.makeName(out);
    bbox_.makeName(out);
}


bool LatLon::sameAs(const Representation& other) const {
    const LatLon* o = dynamic_cast<const LatLon*>(&other);
    return o && (bbox_ == o->bbox_) && (increments_ == o->increments_);
}


bool LatLon::isPeriodicWestEast() const {

    Longitude we(increments_.west_east());
    Longitude east = bbox_.east();
    Longitude west = bbox_.west();

    // correct if grid is periodic, or is shifted West-East

    return (east - west + we).sameWithGrib1Accuracy(360)
           || (
               west.sameWithGrib1Accuracy(we / 2.)
               &&
               east.sameWithGrib1Accuracy(360. - we / 2.)
           );
}


bool LatLon::includesNorthPole() const {


    Latitude north = bbox_.north();
    Latitude south = bbox_.south();
    Latitude sn(increments_.south_north());

    // includes, if grid range is pole-to-pole, or is shifted South-North
    return (north - south).sameWithGrib1Accuracy(180)
           || north.sameWithGrib1Accuracy(Latitude::NORTH_POLE)
           || north.sameWithGrib1Accuracy(90. - sn / 2.);
}


bool LatLon::includesSouthPole() const {


    Latitude north = bbox_.north();
    Latitude south = bbox_.south();
    Latitude sn(increments_.south_north());
    // includes, if grid range is pole-to-pole, or is shifted South-North

    return (north - south).sameWithGrib1Accuracy(180)
           || north.sameWithGrib1Accuracy(Latitude::SOUTH_POLE)
           || north.sameWithGrib1Accuracy(-90. + sn / 2.);

}


class LatLonIterator : public Iterator {

    size_t ni_;
    size_t nj_;

    eckit::Fraction north_;
    eckit::Fraction west_;
    eckit::Fraction lat_;
    eckit::Fraction lon_;

    eckit::Fraction we_;
    eckit::Fraction ns_;

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

    virtual bool next(Latitude &lat, Longitude &lon) {
        if (j_ < nj_) {
            if (i_ < ni_) {
                lat = lat_;
                lon = lon_;
                lon_ += we_;
                i_++;
                if (i_ == ni_) {
                    j_++;
                    lat_ -= ns_;
                    i_ = 0;
                    lon_ = west_;
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
        lat_(north),
        lon_(west_),
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
    return new LatLonIterator(ni_,
                              nj_,
                              bbox_.north().value(),
                              bbox_.west().value(),
                              increments_.west_east(),
                              increments_.south_north());
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


void LatLon::initTrans(Trans_t& trans) const {
#ifdef ATLAS_HAVE_TRANS
    ASSERT(trans_set_resol_lonlat(&trans, ni_, nj_) == 0);
#else
    NOTIMP;
#endif
}



}  // namespace latlon
}  // namespace repres
}  // namespace mir

