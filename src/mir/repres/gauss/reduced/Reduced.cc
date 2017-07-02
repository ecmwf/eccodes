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


#include "mir/repres/gauss/reduced/Reduced.h"

#include <cmath>
#include <limits>
#include <sstream>
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Plural.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"
#include "mir/api/MIRJob.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"

#include "atlas/library/config.h"

#ifdef ATLAS_HAVE_TRANS
#include "transi/trans.h"
#endif


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


Reduced::Reduced(size_t N):
    Gaussian(N) {
}


Reduced::Reduced(size_t N, const util::BoundingBox& bbox):
    Gaussian(N, bbox) {
}


Reduced::Reduced(const param::MIRParametrisation& parametrisation):
    Gaussian(parametrisation) {
}


Reduced::~Reduced() {
}


bool Reduced::sameAs(const Representation& other) const {
    const Reduced* o = dynamic_cast<const Reduced*>(&other);
    return o && Gaussian::sameAs(other);
}


bool Reduced::isPeriodicWestEast() const {
    const std::vector<long>& pl = pls();
    ASSERT(pl.size());
    const long maxpl = *std::max_element(pl.begin(), pl.end());

    const Longitude we = bbox_.east() - bbox_.west();
    const Longitude inc = eckit::Fraction(360, maxpl);

    return (we + inc).sameWithGrib1Accuracy(Longitude::GLOBE.value());
}


Iterator* Reduced::unrotatedIterator() const {

    class ReducedUnrotatedGGIterator : protected ReducedIterator, public Iterator {
        void print(std::ostream& out) const {
            out << "RegularGGIterator[";
            Iterator::print(out);
            out << ",";
            ReducedIterator::print(out);
            out << "]";
        }
        bool next(Latitude& lat, Longitude& lon) {
            return ReducedIterator::next(lat, lon);
        }
    public:
        ReducedUnrotatedGGIterator(const std::vector<double>& latitudes, const std::vector<long>& pl, const util::Domain& dom) :
            ReducedIterator(latitudes, pl, dom) {
        }
    };

    return new ReducedUnrotatedGGIterator(latitudes(), pls(), domain());
}


Iterator *Reduced::rotatedIterator(const util::Rotation& rotation) const {

    class ReducedRotatedGGIterator : protected ReducedIterator, public Iterator {
        void print(std::ostream& out) const {
            out << "RegularGGIterator[";
            Iterator::print(out);
            out << ",";
            ReducedIterator::print(out);
            out << "]";
        }
        bool next(Latitude& lat, Longitude& lon) {
            return ReducedIterator::next(lat, lon);
        }
    public:
        ReducedRotatedGGIterator(const std::vector<double>& latitudes, const std::vector<long>& pl, const util::Domain& dom, const util::Rotation& rotation) :
            ReducedIterator(latitudes, pl, dom),
            Iterator(rotation) {
        }
    };

    return new ReducedRotatedGGIterator(latitudes(), pls(), domain(), rotation);
}


void Reduced::fill(grib_info& info) const  {

    // See copy_spec_from_ksec.c in libemos for info

    const std::vector<long> &pl = pls();

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_GG;
    info.grid.Nj = pl.size();
    info.grid.N = N_;

    info.grid.pl = &pl[0];
    info.grid.pl_size = pl.size();


    for (size_t i = 0; i < size_t(info.grid.pl_size); i++) {
        ASSERT(info.grid.pl[i] > 0);
    }

    bbox_.fill(info);

    /*
        Comment in libemos is:

        "grib_api to set global area in full precision for gaussian grid"

        TODO: check and document

    */

    // for GRIB, a global field is also aligned with Greenwich
    bool westAtGreenwich = bbox_.west() == Longitude::GREENWICH;;

    long j = info.packing.extra_settings_count++;
    info.packing.extra_settings[j].type = GRIB_TYPE_LONG;
    info.packing.extra_settings[j].name = "global";
    info.packing.extra_settings[j].long_value = isGlobal() && westAtGreenwich ? 1 : 0;
}


void Reduced::fill(api::MIRJob& job) const  {
    ASSERT(isGlobal());
    job.set("pl", pls());
}


size_t Reduced::frame(std::vector<double>& values, size_t size, double missingValue) const {

    validate(values);

    size_t count = 0;

    // TODO: Check if that logic cannot also be used for other grid, and therefor move it to a higher class

    std::map<size_t, size_t> shape;

    Latitude prev_lat = std::numeric_limits<double>::max();
    Longitude prev_lon = -std::numeric_limits<double>::max();

    size_t rows = 0;
    size_t dummy = 0; // Used to keep static analyser quiet
    size_t *col = &dummy;

    // Collect the 'shape' of the gaussian field
    // This could be done with the latitudes() and pls(), maybe more efficeintly
    // but this code could also be used for all grids
    // and even be cached (md5 of iterators)

    // Iterator is 'unrotated'
    eckit::ScopedPtr<Iterator> it(iterator());
    while (it->next()) {
        const Iterator::point_ll_t& p = it->pointUnrotated();

        if (p.lat != prev_lat ) {
            ASSERT(p.lat < prev_lat); // Assumes scanning mode
            prev_lat = p.lat;
            prev_lon = -std::numeric_limits<double>::max();

            col = &shape[rows++];
            (*col) = 0;
        }

        ASSERT(p.lon > prev_lon); // Assumes scanning mode
        prev_lon = p.lon;
        (*col) ++;
    }

    size_t k = 0;
    for (size_t j = 0; j < rows; j++) {
        size_t cols = shape[j];
        for (size_t i = 0; i < cols; i++) {
            if ( !((i < size) || (j < size) || (i >= cols - size) || (j >= rows - size))) {
                values[k] = missingValue;
                count++;
            }
            k++;
        }
    }

    ASSERT(k == values.size());
    return count;
}


void Reduced::validate(const std::vector<double>& values) const {

//    std::cout << "Reduced " << domain() << std::endl;

    long long count = 0;
    if (isGlobal()) {
        const std::vector<long>& pl = pls();
        for (size_t i = 0; i < pl.size(); i++) {
            count += pl[i];
        }
    } else {
        eckit::ScopedPtr<Iterator> it(iterator());
        while (it->next()) {
            ++count;
        }
    }

    eckit::Log::debug<LibMir>() << "Reduced::validate checked "
                                << eckit::Plural(values.size(), "value")
                                << ", within domain: "
                                << eckit::BigNum(count)
                                << "."
                                << std::endl;

    ASSERT(values.size() == size_t(count));
}


const Reduced *Reduced::cropped(const util::BoundingBox& bbox) const  {
    const std::vector<long>& pl = pls();
    std::vector<long> newpl;
    newpl.reserve(pl.size());

    const std::vector<double>& lats = latitudes();
    Latitude north = bbox.north();
    Latitude south = bbox.south();

    ASSERT(lats.size() == pl.size());

    for (size_t i = 0; i < lats.size(); i++) {
        Latitude ll(lats[i]);
        if ((ll >= south) && (ll <= north)) {
            newpl.push_back(pl[i]);
        }
    }

    return cropped(bbox, newpl);
}


const Reduced *Reduced::cropped(const util::BoundingBox&, const std::vector<long>&) const {
    std::ostringstream os;
    os << "Reduced::cropped() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Reduced::initTrans(Trans_t& trans) const {
#ifdef ATLAS_HAVE_TRANS


    const std::vector<long>& pl = pls();
    ASSERT(pl.size());

    std::vector<int> pli(pl.size());
    ASSERT(pl.size() == pli.size());

    for (size_t i = 0; i < pl.size(); ++i) {
        pli[i] = pl[i];
    }

    ASSERT(trans_set_resol(&trans, pli.size(), &pli[0]) == 0);

#else
    NOTIMP;
#endif
}


Reduced::ReducedIterator::ReducedIterator(const std::vector<double>& latitudes, const std::vector<long>& pl, const util::Domain& dom) :
    latitudes_(latitudes),
    pl_(pl),
    domain_(dom),
    nj_(pl_.size()),
    i_(0),
    j_(0),
    k_(0),
    p_(0),
    count_(0) {

    // latitudes_/pl_ cover the whole globe
    ASSERT(pl_.size() <= latitudes_.size());
    ASSERT(pl_.size() >= 2);

    // position to first latitude and first/last longitude

    while (k_ < latitudes_.size() && domain_.north() < latitudes_[k_]) {
        k_++;
    }
    ASSERT(k_ < latitudes_.size());

    ni_ = size_t(pl_[p_++]);
    inc_ = eckit::Fraction(360, ni_);
    lon_ = eckit::Fraction(0.0);

}


Reduced::ReducedIterator::~ReducedIterator() {
}


void Reduced::ReducedIterator::print(std::ostream& out) const {
    out << "ReducedIterator["
        <<  "domain=" << domain_
        << ",ni="     << ni_
        << ",nj="     << nj_
        << ",i="      << i_
        << ",j="      << j_
        << ",k="      << k_
        << ",p="      << p_
        << ",count="  << count_
        << "]";
}


bool Reduced::ReducedIterator::next(Latitude& lat, Longitude& lon) {
    while (j_ < nj_ && i_ < ni_) {

        ASSERT(j_ + k_ < latitudes_.size());

        lat = latitudes_[j_ + k_];
        lon = lon_;

        i_++;
        lon_ += inc_;

        if (i_ == ni_) {
            j_++;
            if (j_ < nj_) {
                ASSERT(p_ < pl_.size());
                ni_ = size_t(pl_[p_++]);
                lon_ = eckit::Fraction(0.0);
                inc_ = eckit::Fraction(360, ni_);
                i_ = 0;


            }
        }

        if (domain_.contains(lat, lon)) {
            count_++;
            return true;
        }
    }
    return false;
}


size_t Reduced::numberOfPoints() const {
    ASSERT(isGlobal());

    size_t total = 0;
    const std::vector<long>& pl = pls();
    for (auto j = pl.begin(); j != pl.end(); ++j) {
        total += *j;
    }
    return total;
}



}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir

