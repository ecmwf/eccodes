/*
 * (C) Copyright 1996- ECMWF.
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
#include "eckit/log/Plural.h"
#include "eckit/types/Fraction.h"
#include "mir/api/Atlas.h"
#include "mir/config/LibMir.h"
#include "mir/data/MIRField.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir {
namespace repres {
namespace latlon {


namespace {


static eckit::Fraction adjust(const eckit::Fraction target, const eckit::Fraction& inc, bool up) {
    ASSERT(inc > 0);

    auto r = target / inc;
    auto n = r.integralPart();

    if (!r.integer() && (r > 0) == up) {
        n += (up ? 1 : -1);
    }

    return (n * inc);
}


struct FractionRange {

    FractionRange(eckit::Fraction&& a, eckit::Fraction&& b, eckit::Fraction&& inc, eckit::Fraction&& ref) :
        inc_(inc), ref_(ref) {
        ASSERT(a <= b);
        ASSERT(inc >= 0);

        if (inc_ == 0) {
            b_ = a_ = a;
            return;
        }

        auto shift = (ref_ / inc_).decimalPart() * inc;
        a_ = shift + adjust(a - shift, inc_, true);

        if (b == a) {
            b_ = a_;
            return;
        }

        auto c = shift + adjust(b - shift, inc_, false);
        c = a_ + ((b - a_) / inc_).integralPart() * inc_;

        b_ = c < a_ ? a_ : c;
        ASSERT(a_ <= b_);
    }

    size_t n() const {
        return inc_ == 0 ? 1 : size_t(((b_ - a_) / inc_).integralPart() + 1);
    }

    size_t n(const eckit::Fraction& period) const {
        size_t ni = n();
        if ((ni - 1) * inc() >= period) {
            ni -= 1;
            ASSERT(ni * inc() <= period);
        }
        return ni;
    }

    const eckit::Fraction& a() const { return a_; }
    const eckit::Fraction& b() const { return b_; }
    const eckit::Fraction& inc() const { return inc_; }

private:
    eckit::Fraction a_, b_, inc_, ref_;
};


}  // (anonymous namespace)


LatLon::LatLon(const param::MIRParametrisation& parametrisation) :
    Gridded(parametrisation),
    increments_(parametrisation),
    ni_(0),
    nj_(0) {

    PointLatLon reference(bbox_.south(), bbox_.west());
    correctBoundingBox(bbox_, ni_, nj_, increments_, reference);
    ASSERT(ni_);
    ASSERT(nj_);

    // confirm Ni/Nj from parametrisation (input)
    size_t ni = 0;
    size_t nj = 0;
    ASSERT(parametrisation.get("Ni", ni));
    ASSERT(parametrisation.get("Nj", nj));

    ASSERT(ni == ni_);
    ASSERT(nj == nj_);
}


LatLon::LatLon(const util::Increments& increments, const util::BoundingBox& bbox, const PointLatLon& reference) :
    Gridded(bbox),
    increments_(increments),
    ni_(0),
    nj_(0) {

    correctBoundingBox(bbox_, ni_, nj_, increments_, reference);
    ASSERT(ni_);
    ASSERT(nj_);
}


LatLon::~LatLon() = default;


void LatLon::reorder(long scanningMode, MIRValuesVector& values) const {
    // Code from ecRegrid, UNTESTED!!!

    eckit::Log::debug<LibMir>() << "WARNING: UNTESTED!!! ";
    eckit::Log::debug<LibMir>() << "LatLon::reorder scanning mode 0x" << std::hex << scanningMode << std::dec << std::endl;

    ASSERT(values.size() == ni_ * nj_);

    MIRValuesVector out(values.size());

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

    info.grid.Ni = long(ni_);
    info.grid.Nj = long(nj_);

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
    auto o = dynamic_cast<const LatLon*>(&other);
    return o && (bbox_ == o->bbox_) && (increments_ == o->increments_);
}


bool LatLon::isPeriodicWestEast() const {

    // if range West-East is within one increment (or greater than) 360 degree
    const Longitude& inc = increments_.west_east().longitude();
    return bbox_.east() - bbox_.west() + inc >= Longitude::GLOBE;
}


bool LatLon::includesNorthPole() const {

    // if North latitude is within one increment from North Pole
    const Latitude& inc = increments_.south_north().latitude();
    return bbox_.north() + inc > Latitude::NORTH_POLE;
}


bool LatLon::includesSouthPole() const {

    // if South latitude is within one increment from South Pole
    const Latitude& inc = increments_.south_north().latitude();
    return bbox_.south() - inc < Latitude::SOUTH_POLE;
}


size_t LatLon::numberOfPoints() const {
    ASSERT(ni_);
    ASSERT(nj_);
    return ni_ * nj_;
}


bool LatLon::getLongestElementDiagonal(double& d) const {
    const Latitude& sn = increments_.south_north().latitude();
    const Longitude& we = increments_.west_east().longitude();

    d = atlas::util::Earth::distance(
                atlas::PointLonLat(0., 0.),
                atlas::PointLonLat(we.value(), sn.value()) );
    return true;
}


Representation* LatLon::globalise(data::MIRField& field) const {
    ASSERT(field.representation() == this);

    if (isGlobal()) {
        return nullptr;
    }

    ASSERT(!increments_.isShifted(bbox_));

    // For now, we only use that function for the LAW model, so we only grow by the end (south pole)
    ASSERT(bbox_.north() == Latitude::NORTH_POLE);
    ASSERT(bbox_.west() == Longitude::GREENWICH);
    ASSERT(bbox_.east() + increments_.west_east().longitude() == Longitude::GLOBE);

    util::BoundingBox newbbox(bbox_.north(), bbox_.west(), Latitude::SOUTH_POLE, bbox_.east());

    eckit::ScopedPtr<LatLon> newll(const_cast<LatLon*>(croppedRepresentation(newbbox)));

    ASSERT(newll->nj_ > nj_);
    ASSERT(newll->ni_ == ni_);

    size_t n = ni_ * nj_;
    size_t newn = newll->ni_ * newll->nj_;
    double missingValue = field.missingValue();

    for (size_t i = 0; i < field.dimensions(); i++ ) {
        MIRValuesVector newvalues(newn, missingValue);
        const MIRValuesVector& values = field.direct(i);
        ASSERT(values.size() == n);

        for (size_t j = 0 ; j < n; ++j) {
            newvalues[j] = values[j];
        }

        field.update(newvalues, i);
    }

    field.hasMissing(true);

    return newll.release();
}


const LatLon* LatLon::croppedRepresentation(const util::BoundingBox&) const {
    std::ostringstream os;
    os << "LatLon::croppedRepresentation() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void LatLon::fill(util::MeshGeneratorParameters& params) const {
    params.meshGenerator_ = "structured";

    if (boundingBox().south() > Latitude::EQUATOR) {
        params.set("force_include_south_pole", true);
    }
    if (boundingBox().north() < Latitude::EQUATOR) {
        params.set("force_include_north_pole", true);
    }
}


size_t LatLon::frame(MIRValuesVector& values, size_t size, double missingValue) const {

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


void LatLon::validate(const MIRValuesVector& values) const {
    const size_t count = numberOfPoints();

    eckit::Log::debug<LibMir>() << domain() << std::endl;

    eckit::Log::debug<LibMir>() << "LatLon::validate checked " << eckit::Plural(values.size(), "value") << ", within domain: " << eckit::BigNum(count) << "." << std::endl;
    ASSERT(values.size() == count);
}


LatLon::LatLonIterator::LatLonIterator(size_t ni, size_t nj, Latitude north, Longitude west, const util::Increments& increments) :
    ni_(ni),
    nj_(nj),
    north_(north.fraction()),
    west_(west.fraction()),
    we_(increments.west_east().longitude().fraction()),
    ns_(increments.south_north().latitude().fraction()),
    i_(0),
    j_(0),
    count_(0) {
    lat_ = north_;
    lon_ = west_;
    latValue_ = lat_;
    lonValue_ = lon_;
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
            lat = latValue_;
            lon = lonValue_;
            lon_ += we_;
            i_++;
            if (i_ == ni_) {
                j_++;
                i_ = 0;
                lat_ -= ns_;
                lon_ = west_;
                latValue_ = lat_;
            }
            lonValue_ = lon_;
            count_++;
            return true;
        }
    }
    return false;
}


void LatLon::globaliseBoundingBox(util::BoundingBox& bbox, const util::Increments& inc, const PointLatLon& reference) {
    using eckit::Fraction;

    Fraction sn = inc.south_north().latitude().fraction();
    Fraction we = inc.west_east().longitude().fraction();
    ASSERT(sn > 0);
    ASSERT(we > 0);

    Fraction shift_sn = (reference.lat().fraction() / sn).decimalPart() * sn;
    Fraction shift_we = (reference.lon().fraction() / we).decimalPart() * we;


    // Latitude limits

    Latitude n = shift_sn + adjust(Latitude::NORTH_POLE.fraction() - shift_sn, sn, false);
    Latitude s = shift_sn + adjust(Latitude::SOUTH_POLE.fraction() - shift_sn, sn, true);


    // Longitude limits
    // - West for non-periodic grids is not corrected!
    // - East for periodic grids is W + 360 - increment

    Longitude w = bbox.west();
    if (inc.isPeriodic()) {
        w = shift_we + adjust(Longitude::GREENWICH.fraction() - shift_we, we, true);
    }

    Longitude e = shift_we + adjust(w.fraction() + Longitude::GLOBE.fraction() - shift_we, we, false);
    if (e - w == Longitude::GLOBE) {
        e -= we;
    }


    // set bounding box
    bbox = {n, w, s, e};
}


void LatLon::correctBoundingBox(util::BoundingBox& bbox, size_t& ni, size_t& nj, const util::Increments& inc, const PointLatLon& reference) {

    // Latitude/longitude ranges
    FractionRange lat{ bbox.south().fraction(), bbox.north().fraction(), inc.south_north().latitude().fraction(), reference.lat().fraction() };
    auto n = lat.b();
    auto s = lat.a();

    nj = lat.n();
    ASSERT(nj > 0);

    FractionRange lon{ bbox.west().fraction(), bbox.east().fraction(), inc.west_east().longitude().fraction(), reference.lon().fraction() };
    auto w = lon.a();
    auto e = lon.b();

    ni = lon.n(Longitude::GLOBE.fraction());
    ASSERT(ni > 0);

    // checks
    ASSERT(w + (ni - 1) * lon.inc() == e || ni * lon.inc() == Longitude::GLOBE.fraction());
    ASSERT(s + (nj - 1) * lat.inc() == n);

    bbox = {n, w, s, e};
}


}  // namespace latlon
}  // namespace repres
}  // namespace mir

