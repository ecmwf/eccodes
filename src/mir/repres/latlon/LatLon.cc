/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/repres/latlon/LatLon.h"

#include <memory>
#include <ostream>
#include <sstream>

#include "eckit/log/JSON.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"

#include "mir/data/MIRField.h"
#include "mir/iterator/detail/RegularIterator.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Domain.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/Log.h"
#include "mir/util/MeshGeneratorParameters.h"
#include "mir/util/Types.h"


namespace mir::repres::latlon {


LatLon::LatLon(const param::MIRParametrisation& parametrisation) :
    Gridded(parametrisation), increments_(parametrisation), reference_(bbox_.south(), bbox_.west()), ni_(0), nj_(0) {
    correctBoundingBox(bbox_, ni_, nj_, increments_, reference_);
    ASSERT(ni_ != 0);
    ASSERT(nj_ != 0);

    // confirm Ni/Nj from parametrisation (input)
    size_t ni = 0;
    size_t nj = 0;
    ASSERT(parametrisation.get("Ni", ni));
    ASSERT(parametrisation.get("Nj", nj));

    Log::debug() << "LatLon:"
                    "\n\t"
                    "(Ni, Nj) = ("
                 << ni_ << ", " << nj_
                 << ") calculated"
                    "\n\t"
                    "(Ni, Nj) = ("
                 << ni << ", " << nj << ") from parametrisation" << std::endl;

    ASSERT(ni == ni_);
    ASSERT(nj == nj_);
}


LatLon::LatLon(const util::Increments& increments, const util::BoundingBox& bbox, const PointLatLon& reference) :
    Gridded(bbox), increments_(increments), reference_(reference), ni_(0), nj_(0) {
    correctBoundingBox(bbox_, ni_, nj_, increments_, reference_);
    ASSERT(ni_ != 0);
    ASSERT(nj_ != 0);
}


LatLon::~LatLon() = default;


void LatLon::reorder(long scanningMode, MIRValuesVector& values) const {
    grib_reorder(values, scanningMode, ni_, nj_);
}


void LatLon::print(std::ostream& out) const {
    out << "LatLon["
        << "bbox=" << bbox_ << ",increments=" << increments_ << ",ni=" << ni_ << ",nj=" << nj_ << "]";
}


void LatLon::json(eckit::JSON& s) const {
    s << "bbox" << bbox_;
    s << "increments" << increments_;
    s << "ni" << ni_;
    s << "nj" << nj_;
}


void LatLon::fillGrib(grib_info& info) const {
    // See copy_spec_from_ksec.c in libemos for info
    // Warning: scanning mode not considered

    info.grid.Ni = long(ni_);
    info.grid.Nj = long(nj_);

    increments_.fillGrib(info);
    bbox_.fillGrib(info);
}


void LatLon::fillJob(api::MIRJob& job) const {
    increments_.fillJob(job);
    bbox_.fillJob(job);
}


void LatLon::makeName(std::ostream& out) const {
    out << "LL";
    increments_.makeName(out);
    bbox_.makeName(out);
}


bool LatLon::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const LatLon*>(&other);
    return (o != nullptr) && (bbox_ == o->bbox_) && (increments_ == o->increments_);
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
    constexpr double TWO = 2.;

    auto snHalf = increments_.south_north().latitude().value() / TWO;
    ASSERT(!eckit::types::is_approximately_equal(snHalf, 0.));

    auto weHalf = increments_.west_east().longitude().value() / TWO;
    ASSERT(!eckit::types::is_approximately_equal(weHalf, 0.));

    d = TWO * util::Earth::distance({0., 0.}, {weHalf, snHalf});
    return true;
}


Representation* LatLon::globalise(data::MIRField& field) const {
    ASSERT(field.representation() == this);

    if (isGlobal()) {
        return nullptr;
    }

    // For now, we only grow towards North and South poles
    ASSERT(domain().isPeriodicWestEast());

    util::BoundingBox newbbox(bbox_);
    globaliseBoundingBox(newbbox, increments_, {bbox_.south(), bbox_.west()});

    std::unique_ptr<LatLon> newll(const_cast<LatLon*>(croppedRepresentation(newbbox)));

    ASSERT(newll->nj_ > nj_);
    ASSERT(newll->ni_ == ni_);

    size_t n    = ni_ * nj_;
    size_t newn = newll->ni_ * newll->nj_;

    auto lat       = newbbox.north();
    auto lon       = newbbox.west();
    size_t nBefore = 0;
    while (!bbox_.contains(lat, lon)) {
        lat -= increments_.south_north().latitude();
        nBefore += ni_;
        ASSERT(n + nBefore <= newn);
    }

    for (size_t i = 0; i < field.dimensions(); i++) {
        MIRValuesVector newvalues(newn, field.missingValue());
        const MIRValuesVector& values = field.direct(i);
        ASSERT(values.size() == n);

        for (size_t j = 0; j < n; ++j) {
            newvalues[j + nBefore] = values[j];
        }

        field.update(newvalues, i);
    }

    field.hasMissing(true);

    return newll.release();
}


const LatLon* LatLon::croppedRepresentation(const util::BoundingBox& /*unused*/) const {
    std::ostringstream os;
    os << "LatLon::croppedRepresentation() not implemented for " << *this;
    throw exception::SeriousBug(os.str());
}


bool LatLon::extendBoundingBoxOnIntersect() const {
    return false;
}


void LatLon::fillMeshGen(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "structured";
    }
    if (boundingBox().south() > Latitude::EQUATOR) {
        params.set("force_include_south_pole", true);
    }
    if (boundingBox().north() < Latitude::EQUATOR) {
        params.set("force_include_north_pole", true);
    }
}


size_t LatLon::frame(MIRValuesVector& values, size_t size, double missingValue, bool estimate) const {

    // Could be done better, just a demo
    if (!estimate) {
        validate(values);
    }

    size_t count = 0;

    size_t k = 0;
    for (size_t j = 0; j < nj_; j++) {
        for (size_t i = 0; i < ni_; i++) {
            if (!((i < size) || (j < size) || (i >= ni_ - size) || (j >= nj_ - size))) {  // Check me, may be buggy
                if (!estimate) {
                    values[k] = missingValue;
                }
                count++;
            }
            k++;
        }
    }

    // Log::info() << "LatLon::frame(" << size << ") " << count << " " << k << std::endl;


    if (!estimate) {
        ASSERT(k == values.size());
    }
    return count;
}


void LatLon::validate(const MIRValuesVector& values) const {
    const size_t count = numberOfPoints();

    Log::debug() << "LatLon::validate checked " << Log::Pretty(values.size(), {"value"}) << ", iterator counts "
                 << Log::Pretty(count) << " (" << domain() << ")." << std::endl;

    ASSERT_VALUES_SIZE_EQ_ITERATOR_COUNT("LatLon", values.size(), count);
}


LatLon::LatLonIterator::LatLonIterator(size_t ni, size_t nj, Latitude north, Longitude west,
                                       const util::Increments& increments) :
    ni_(ni),
    nj_(nj),
    north_(north.fraction()),
    west_(west.fraction()),
    we_(increments.west_east().longitude().fraction()),
    ns_(increments.south_north().latitude().fraction()),
    i_(0),
    j_(0),
    count_(0),
    first_(true) {
    lat_      = north_;
    lon_      = west_;
    latValue_ = lat_;
    lonValue_ = lon_;
}


LatLon::LatLonIterator::~LatLonIterator() {
    auto count = count_ + (i_ > 0 || j_ > 0 ? 1 : 0);
    ASSERT(count == ni_ * nj_);
}


void LatLon::LatLonIterator::print(std::ostream& out) const {
    out << "LatLonIterator["
        << "ni=" << ni_ << ",nj=" << nj_ << ",north=" << north_ << ",west=" << west_ << ",we=" << we_ << ",ns=" << ns_
        << ",i=" << i_ << ",j=" << j_ << ",count=" << count_ << "]";
}


bool LatLon::LatLonIterator::next(Latitude& lat, Longitude& lon) {
    if (j_ < nj_) {
        if (i_ < ni_) {
            lat = latValue_;
            lon = lonValue_;

            lon_ += we_;

            if (first_) {
                first_ = false;
            }
            else {
                count_++;
            }

            if (++i_ == ni_) {
                j_++;
                i_ = 0;
                lat_ -= ns_;
                lon_      = west_;
                latValue_ = lat_;
            }

            lonValue_ = lon_;

            return true;
        }
    }
    return false;
}


void LatLon::globaliseBoundingBox(util::BoundingBox& bbox, const util::Increments& inc, const PointLatLon& reference) {
    using eckit::Fraction;
    using iterator::detail::RegularIterator;

    Fraction sn = inc.south_north().latitude().fraction();
    Fraction we = inc.west_east().longitude().fraction();
    ASSERT(sn > 0);
    ASSERT(we > 0);

    Fraction shift_sn = (reference.lat().fraction() / sn).decimalPart() * sn;
    Fraction shift_we = (reference.lon().fraction() / we).decimalPart() * we;


    // Latitude limits

    Latitude n = shift_sn + RegularIterator::adjust(Latitude::NORTH_POLE.fraction() - shift_sn, sn, false);
    Latitude s = shift_sn + RegularIterator::adjust(Latitude::SOUTH_POLE.fraction() - shift_sn, sn, true);


    // Longitude limits
    // - West for non-periodic grids is not corrected!
    // - East for periodic grids is W + 360 - increment

    Longitude w = bbox.west();
    if (inc.isPeriodic()) {
        w = shift_we + RegularIterator::adjust(Longitude::GREENWICH.fraction() - shift_we, we, true);
    }

    Longitude e = shift_we + RegularIterator::adjust(w.fraction() + Longitude::GLOBE.fraction() - shift_we, we, false);
    if (e - w == Longitude::GLOBE) {
        e -= we;
    }


    // set bounding box
    bbox = {n, w, s, e};
}


void LatLon::correctBoundingBox(util::BoundingBox& bbox, size_t& ni, size_t& nj, const util::Increments& inc,
                                const PointLatLon& reference) {
    using iterator::detail::RegularIterator;

    // Latitude/longitude ranges
    RegularIterator lat{bbox.south().fraction(), bbox.north().fraction(), inc.south_north().latitude().fraction(),
                        reference.lat().fraction()};
    auto n = lat.b();
    auto s = lat.a();

    nj = lat.n();
    ASSERT(nj > 0);

    RegularIterator lon{bbox.west().fraction(), bbox.east().fraction(), inc.west_east().longitude().fraction(),
                        reference.lon().fraction(), Longitude::GLOBE.fraction()};
    auto w = lon.a();
    auto e = lon.b();

    ni = lon.n();
    ASSERT(ni > 0);

    // checks
    ASSERT(w + (ni - 1) * lon.inc() == e || ni * lon.inc() == Longitude::GLOBE.fraction());
    ASSERT(s + (nj - 1) * lat.inc() == n);

    bbox = {n, w, s, e};
}


}  // namespace mir::repres::latlon
