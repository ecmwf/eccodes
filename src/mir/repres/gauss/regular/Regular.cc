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


#include "mir/repres/gauss/regular/Regular.h"

#include <cmath>
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Plural.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"
#include "atlas/domain/detail/RectangularDomain.h"
#include "atlas/grid.h"
#include "mir/api/MIRJob.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace regular {


Regular::Regular(const param::MIRParametrisation &parametrisation):
    Gaussian(parametrisation) {
    setNiNj();
}


Regular::Regular(size_t N):
    Gaussian(N) {
    setNiNj();
}


Regular::Regular(size_t N, const util::BoundingBox &bbox):
    Gaussian(N, bbox) {
    setNiNj();
}


Regular::~Regular() {
}


void Regular::fill(grib_info &info) const  {

    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_GG;

    info.grid.N = N_;
    info.grid.iDirectionIncrementInDegrees = 90.0 / N_;
    info.grid.Ni = Ni_;
    info.grid.Nj = Nj_;

    bbox_.fill(info);

    /*
        Comment in libemos is:

        "grib_api to set global area in full precision for gaussian grid"

        TODO: check and document

    */

    // for GRIB, a global field is also aligned with Greenwich
    bool global = domain().isGlobal();
    bool westAtGreenwich = eckit::types::is_approximately_equal<double>(0, bbox_.west());

    long j = info.packing.extra_settings_count++;
    info.packing.extra_settings[j].type = GRIB_TYPE_LONG;
    info.packing.extra_settings[j].name = "global";
    info.packing.extra_settings[j].long_value = global && westAtGreenwich ? 1 : 0;
}


void Regular::fill(api::MIRJob &job) const  {
    std::stringstream os;
    os << "F" << N_;
    job.set("gridname", os.str());
}


atlas::Grid Regular::atlasGrid() const {
    util::Domain dom = domain();
    atlas::RectangularDomain rectangle({dom.west(), dom.east()}, {dom.south(), dom.north()});

    return atlas::grid::RegularGaussianGrid("F" + std::to_string(N_), rectangle);
}


util::Domain Regular::domain() const {

    const std::vector<double> &lats = latitudes();
    ASSERT(lats.size() >= 2);


    // West-East domain limits
    // FIXME get precision from GRIB (angularPrecision)
    // GRIB=1 is in millidegree, GRIB-2 in in micro-degree. Use the precision given by GRIB in this check
    // The dissemination will put in the GRIB header what is specified by the user
    // so, for example if the user specify 359.999999 as the eastern longitude, this
    // value will end up in the header
    const double epsilon_grib1 = 1.0 / 1000.0;
    eckit::types::CompareApproximatelyEqual<double> cmp_eps(epsilon_grib1);

    const eckit::Fraction inc(90, long(N_));

    double west = bbox_.west();
    double east = bbox_.east();
    if (eckit::types::is_approximately_greater_or_equal<double>(bbox_.east() - bbox_.west() + inc, 360., epsilon_grib1)) {
        east = west + 360.;
    } else {
        long n = long(std::floor(west / double(inc)));
        west = cmp_eps(west, inc * n)?     inc * n
             : cmp_eps(west, inc * (n+1))? inc * (n+1)
             : throw eckit::SeriousBug("Regular::domain: cannot match bounding box West " + std::to_string(west) + " given increment " + std::to_string(double(inc)));

        n = long(std::floor(east / double(inc)));
        east = cmp_eps(east, inc * n)?     inc * n
             : cmp_eps(east, inc * (n+1))? inc * (n+1)
             : throw eckit::SeriousBug("Regular::domain: cannot match bounding box East " + std::to_string(east) + " given increment " + std::to_string(double(inc)));
    }


    // North-South domain limits
    // assumes latitudes are sorted North-to-South
    double north = bbox_.north();
    double south = bbox_.south();
    for (const double& lat: lats) {
        if (cmp_eps(north, lat)) { north = lat; }
        if (cmp_eps(south, lat)) { south = lat; }
    }

    if (eckit::types::is_approximately_equal(north, lats.front())) { north =  90; }
    if (eckit::types::is_approximately_equal(south, lats.back()))  { south = -90; }

    return util::Domain(north, west, south, east);
}


void Regular::validate(const std::vector<double>& values) const {
    const util::Domain dom = domain();
    long long count = 0;

    if (dom.isGlobal()) {
        count = (N_ * 2) * (N_ * 4);
    } else {
        eckit::ScopedPtr<Iterator> it(unrotatedIterator());
        double lat;
        double lon;
        while (it->next(lat, lon)) {
            if (dom.contains(lat, lon)) {
                ++count;
            }
        }
    }

    eckit::Log::debug<LibMir>() << "Regular::validate checked " << eckit::Plural(values.size(), "value") << ", within domain: " << eckit::BigNum(count) << "." << std::endl;
    ASSERT(values.size() == size_t(count));
}


void Regular::setNiNj() {
    const util::Domain dom = domain();

    Ni_ = N_ * 4;
    if (!dom.isPeriodicEastWest()) {
        const double lat_middle = (dom.north() + dom.south()) / 2.;
        const eckit::Fraction inc(90, N_);

        Ni_ = 0;
        for (size_t i = 0; i < N_ * 4; ++i) {
            const eckit::Fraction lon = dom.west() + i * inc;
            if (dom.contains(lat_middle, lon)) {
                ++Ni_;
            }
        }
    }
    ASSERT(2 <= Ni_ && Ni_ <= N_ * 4);

    Nj_ = N_ * 2;
    if (!dom.includesPoleNorth() || !dom.includesPoleSouth()) {
        const double lon_middle = (dom.west() + dom.east()) / 2.;

        Nj_ = 0;
        for (const double& lat: latitudes()) {
            if (dom.contains(lat, lon_middle)) {
                ++Nj_;
            }
        }
    }
    ASSERT(2 <= Nj_ && Nj_ <= N_ * 2);
}


size_t Regular::frame(std::vector<double>& values, size_t size, double missingValue) const {

    // TODO: Check if that logic cannot also be used for other grid, and therefore move it to a higher class
    validate(values);

    size_t count = 0;

    size_t k = 0;
    for (size_t j = 0; j < Nj_; j++) {
        for (size_t i = 0; i < Ni_; i++) {
            if (!((i < size) || (j < size) || (i >= Ni_ - size) || (j >= Nj_ - size))) { // Check me, may be buggy
                values[k] = missingValue;
                count++;
            }
            k++;
        }
    }

    ASSERT(k == values.size());
    return count;
}


class RegularIterator : public Iterator {

    std::vector<double> latitudes_;
    const double west_;

    const size_t N_;
    const size_t Ni_;
    const size_t Nj_;

    eckit::Fraction lon_;
    const eckit::Fraction inc_;

    size_t i_;
    size_t j_;
    size_t k_;

    size_t count_;

    virtual void print(std::ostream &out) const {
        out << "RegularIterator["
            <<  "west="  << west_
            << ",N="     << N_
            << ",Ni="    << Ni_
            << ",Nj="    << Nj_
            << ",i="     << i_
            << ",j="     << j_
            << ",k="     << k_
            << ",count=" << count_
            << "]";
    }

    virtual bool next(double &lat, double &lon) {
        while (j_ < Nj_ && i_ < Ni_) {

            ASSERT(j_ + k_ < latitudes_.size());
            lat = latitudes_[j_ + k_];
            lon = lon_;

            i_++;
            lon_ += inc_;

            if (i_ == Ni_) {
                j_++;
                i_ = 0;
                lon_ = west_;
            }
            count_++;
            return true;
        }
        return false;
    }

    ~RegularIterator() {
        ASSERT(count_ == Ni_ * Nj_);
    }

public:

    // TODO: Consider keeping a reference on the latitudes, to avoid copying

    RegularIterator(const std::vector<double>& latitudes, size_t N, size_t Ni, size_t Nj, const util::Domain& dom) :
        latitudes_(latitudes),
        west_(dom.west()),
        N_(N),
        Ni_(Ni),
        Nj_(Nj),
        lon_(west_),
        inc_(90, N_),
        i_(0),
        j_(0),
        k_(0),
        count_(0) {

        // latitudes_ covers the whole globe, but (Ni_,Nj_) cover only the domain
        ASSERT(latitudes_.size() == N * 2);
        ASSERT(2 <= Ni_ && Ni_ <= N * 4);
        ASSERT(2 <= Nj_ && Nj_ <= N * 2);

        // Position to first latitude
        while (k_ < latitudes_.size() && dom.north() < latitudes_[k_]) {
            k_++;
        }

    }

};


Iterator *Regular::unrotatedIterator() const {
    return new RegularIterator(latitudes(), N_, Ni_, Nj_, domain());
}


Iterator* Regular::rotatedIterator() const {
    return unrotatedIterator();
}


void Regular::shape(size_t &ni, size_t &nj) const {
    ni = Ni_;
    nj = Nj_;
}


}  // namespace regular
}  // namespace repres
}  // namespace mir

