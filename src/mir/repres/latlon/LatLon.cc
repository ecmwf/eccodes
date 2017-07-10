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

#include <algorithm>
#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"
#include "atlas/library/config.h"
#ifdef ATLAS_HAVE_TRANS
#include "transi/trans.h"
#endif
#include "mir/action/misc/AreaCropper.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"
#include "mir/data/MIRField.h"


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
    increments_(increments){
    setNiNj();
}


LatLon::~LatLon() {
}


void LatLon::setNiNj() {
    ni_ = increments_.computeNi(bbox_);
    nj_ = increments_.computeNj(bbox_);
}


void LatLon::reorder(long scanningMode, std::vector<double>& values) const {
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


void LatLon::print(std::ostream& out) const {
    out << "LatLon["
        <<  "bbox=" << bbox_
        << ",increments=" << increments_
        << ",ni=" << ni_
        << ",nj=" << nj_
        << "]";
}


void LatLon::fill(grib_info& info) const {
    // See copy_spec_from_ksec.c in libemos for info
    // Warning: scanning mode not considered

    info.grid.Ni = ni_;
    info.grid.Nj = nj_;

    increments_.fill(info);
    bbox_.fill(info);
}


void LatLon::fill(api::MIRJob& job) const {
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

    // if longitude range spans the globe
    const Longitude range = bbox_.east() - bbox_.west() + increments_.west_east();

    return range.sameWithGrib1Accuracy(Longitude::GLOBE);
}


bool LatLon::includesNorthPole() const {

    // if latitude range spans the globe, or within one increment from bounding box North
    const Latitude range = bbox_.north() - bbox_.south();
    const Latitude reach = std::min(bbox_.north() + increments_.south_north(), Latitude::NORTH_POLE);

    return  range.sameWithGrib1Accuracy(Latitude::GLOBE) ||
            reach.sameWithGrib1Accuracy(Latitude::NORTH_POLE);
}


bool LatLon::includesSouthPole() const {

    // if latitude range spans the globe, or within one increment from bounding box South
    const Latitude range = bbox_.north() - bbox_.south();
    const Latitude reach = std::max(bbox_.south() - increments_.south_north(), Latitude::SOUTH_POLE);

    return  range.sameWithGrib1Accuracy(Latitude::GLOBE) ||
            reach.sameWithGrib1Accuracy(Latitude::SOUTH_POLE);
}


size_t LatLon::numberOfPoints() const {
    ASSERT(ni_);
    ASSERT(nj_);
    return ni_ * nj_;
}


Representation* LatLon::globalise(data::MIRField& field) const {
    ASSERT(field.representation() == this);

    if (isGlobal()) {
        return 0;
    }

    ASSERT(!increments_.isShifted(bbox_));

    // For now, we only use that function for the LAW model, so we only grow by the end (south pole)
    ASSERT(bbox_.north() == Latitude::NORTH_POLE);
    ASSERT(bbox_.west() == Longitude::GREENWICH);
    ASSERT(bbox_.east() + increments_.west_east() == Longitude::GLOBE);

    util::BoundingBox newbbox(bbox_.north(), bbox_.west(), Latitude::SOUTH_POLE, bbox_.east());

    eckit::ScopedPtr<LatLon> newll(const_cast<LatLon*>(cropped(newbbox)));

    ASSERT(newll->nj_ > nj_);
    ASSERT(newll->ni_ == ni_);

    size_t n = ni_ * nj_;
    size_t newn = newll->ni_ * newll->nj_;
    double missingValue = field.missingValue();

    for (size_t i = 0; i < field.dimensions(); i++ ) {
        std::vector<double> newvalues(newn, missingValue);
        const std::vector<double> &values = field.direct(i);
        ASSERT(values.size() == n);

        for (size_t j = 0 ; j < n; ++j) {
            newvalues[j] = values[j];
        }

        field.update(newvalues, i);
    }

    field.hasMissing(true);

    return newll.release();
}


const LatLon* LatLon::cropped(const util::BoundingBox&) const {
    std::ostringstream os;
    os << "LatLon::cropped() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


size_t LatLon::frame(std::vector<double>& values, size_t size, double missingValue) const {

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


void LatLon::validate(const std::vector<double>& values) const {
    eckit::Log::debug<LibMir>() << "LatLon::validate " << values.size() << " ni*nj " << ni_ * nj_ << std::endl;
    ASSERT(values.size() == ni_ * nj_);
}


void LatLon::shape(size_t& ni, size_t& nj) const {
    ni = ni_;
    nj = nj_;
}


void LatLon::initTrans(Trans_t& trans) const {
    ASSERT(!increments_.isShifted(bbox_));
#ifdef ATLAS_HAVE_TRANS
    ASSERT(trans_set_resol_lonlat(&trans, ni_, nj_) == 0);
#else
    NOTIMP;
#endif
}


LatLon::LatLonIterator::LatLonIterator(size_t ni, size_t nj, Latitude north, Longitude west, double we, double ns) :
    ni_(ni),
    nj_(nj),
    north_(north.fraction()),
    west_(west.fraction()),
    we_(we),
    ns_(ns),
    i_(0),
    j_(0),
    count_(0) {
    lat_ = north_;
    lon_ = west_;
}


LatLon::LatLonIterator::~LatLonIterator() {
    ASSERT(count_ == ni_ * nj_);
}


void LatLon::LatLonIterator::print(std::ostream& out) const {
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


bool LatLon::LatLonIterator::next(Latitude& lat, Longitude& lon) {
    if (j_ < nj_) {
        if (i_ < ni_) {
            lat = lat_;
            lon = lon_;
            lon_ += we_;
            i_++;
            if (i_ == ni_) {
                j_++;
                i_ = 0;
                lat_ -= ns_;
                lon_ = west_;
            }
            count_++;
            return true;
        }
    }
    return false;
}


}  // namespace latlon
}  // namespace repres
}  // namespace mir

