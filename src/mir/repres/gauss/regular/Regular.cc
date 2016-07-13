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

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Plural.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/types/FloatCompare.h"
#include "atlas/grid/gaussian/RegularGaussian.h"
#include "mir/api/MIRJob.h"
#include "mir/log/MIR.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
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

    size_t j = info.packing.extra_settings_count++;
    info.packing.extra_settings[j].type = GRIB_TYPE_LONG;
    info.packing.extra_settings[j].name = "global";
    info.packing.extra_settings[j].long_value = globalDomain()? 1 : 0;
}


void Regular::fill(api::MIRJob &job) const  {
    ASSERT(globalDomain());
    std::stringstream os;
    os << "F" << N_;
    job.set("gridname", os.str());
}


atlas::grid::Grid *Regular::atlasGrid() const {
    ASSERT(globalDomain()); // Atlas support needed for non global grids
    return new atlas::grid::gaussian::RegularGaussian(N_);
}


bool Regular::globalDomain() const {

    if (bbox_.west() == 0 && bbox_.east() == 360 && bbox_.north() == 90 && bbox_.south() == -90) {
        return true;
    }

// unused //  size_t Nj = N_ * 2;
    size_t Ni = N_ * 4;


    double last = 360.0 - 360.0 / Ni ;
    double ew = bbox_.east() - bbox_.west();

    // FIXME: GRIB=1 is in millidegree, GRIB-2 in in micro-degree
    // Use the precision given by GRIB in this check

    const double epsilon = 1.0 / 1000.0;

    bool full = fabs(ew - last) < epsilon;

    if (full) {
        const std::vector<double> &lats = latitudes();
        ASSERT(lats.size());
        bool npole = fabs(bbox_.north() - lats[0]) < epsilon;
        bool spole = fabs(bbox_.south() - lats[lats.size() - 1]) < epsilon;
        return npole && spole;
    }
    return false;
}


void Regular::validate(const std::vector<double> &values) const {
    if (globalDomain()) {
        ASSERT(values.size() == (N_ * 2) * (N_ * 4));
    }
    else {
        eckit::ScopedPtr<Iterator> it(unrotatedIterator());
        long long count = 0;
        double lat;
        double lon;
        while (it->next(lat, lon)) {
            if (bbox_.contains(lat, lon))
                count++;
        }
        eckit::Log::trace<MIR>() << "Regular::validate checked " << eckit::Plural(values.size(),"value") << ", got " << eckit::BigNum(count) << "." << std::endl;
        ASSERT(values.size() == size_t(count));
    }
}


void Regular::setNiNj() {
    //TODO Only global input supported...
    const double lon_middle = (bbox_.west()  + bbox_.east() )/2.;
    const double lat_middle = (bbox_.north() + bbox_.south())/2.;

    Ni_ = N_ * 4;
    if (!globalDomain()) {
        Ni_ = 0;
        for (size_t i=0; i<N_*4; ++i) {
            const double lon = bbox_.west() + (i * 90.0) / N_;
            if (bbox_.contains(lat_middle, lon))
                ++Ni_;
        }
    }
    ASSERT(2 <= Ni_ <= N_*4);

    Nj_ = N_ * 2;
    if (!globalDomain()) {
        Nj_ = 0;
        const std::vector<double>& lats = latitudes();
        for (std::vector<double>::const_iterator lat=lats.begin(); lat!=lats.end(); ++lat) {
            if (bbox_.contains(*lat, lon_middle))
                ++Nj_;
        }
    }
    ASSERT(2 <= Nj_ <= N_*2);
}


size_t Regular::frame(std::vector<double>& values, size_t size, double missingValue) const {

    // TODO: Check if that logic cannot also be used for other grid, and therefore move it to a higher class
    validate(values);

    size_t count = 0;

    size_t k = 0;
    for (size_t j=0; j<Nj_; j++) {
        for (size_t i=0; i<Ni_; i++) {
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
    util::BoundingBox bbox_;

    const size_t N_;
    const size_t Ni_;
    const size_t Nj_;

    size_t i_;
    size_t j_;

    size_t count_;

    virtual void print(std::ostream &out) const {
        out << "RegularIterator["
            <<  "bbox="  << bbox_
            << ",N="     << N_
            << ",Ni="    << Ni_
            << ",Nj="    << Nj_
            << ",i="     << i_
            << ",j="     << j_
            << ",count=" << count_
            << "]";
    }

    virtual bool next(double &lat, double &lon) {
        while (j_ < Nj_ && i_ < Ni_) {

            lat = latitudes_[j_];
            lon = (i_ * 90.0) / N_;

            i_++;
            if (i_ == Ni_) {
                j_++;
                i_ = 0;
            }
            if (bbox_.contains(lat, lon)) {
                count_++;
                return true;
            }
        }
        return false;
    }

    ~RegularIterator() {
        ASSERT(count_ == Ni_ * Nj_);
    }

public:

    // TODO: Consider keeping a reference on the latitudes and bbox, to avoid copying

    RegularIterator(const std::vector<double>& latitudes, size_t N, size_t Ni, size_t Nj, const util::BoundingBox &bbox) :
        latitudes_(latitudes),
        bbox_(bbox),
        N_(N),
        Ni_(Ni),
        Nj_(Nj),
        i_(0),
        j_(0),
        count_(0) {
        ASSERT(latitudes_.size() == Nj_);
    }

};


Iterator *Regular::unrotatedIterator() const {
    // Use a global bounding box if global domain, to avoid rounding issues
    // due to GRIB (in)accuracies
    return new RegularIterator(latitudes(), N_, Ni_, Nj_, globalDomain() ? util::BoundingBox() : bbox_);
}


Iterator* Regular::rotatedIterator() const {
    return unrotatedIterator();
}


void Regular::shape(size_t &ni, size_t &nj) const {
    ASSERT(globalDomain());
    ni = Ni_;
    nj = Nj_;
}


}  // namespace regular
}  // namespace repres
}  // namespace mir

