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


#include "mir/repres/latlon/ReducedLL.h"

#include <iostream>
#include "atlas/grid/lonlat/ReducedLonLat.h"
#include "mir/action/misc/AreaCropper.h"
#include "mir/api/MIRJob.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Compare.h"


namespace mir {
namespace repres {
namespace latlon {


ReducedLL::ReducedLL(const param::MIRParametrisation &parametrisation):
    bbox_(parametrisation) {
    ASSERT(parametrisation.get("pl", pl_));
    ASSERT(parametrisation.get("Nj", Nj_));
    ASSERT(Nj_);
    ASSERT(pl_.size()==Nj_);
}


ReducedLL::~ReducedLL() {
}


void ReducedLL::print(std::ostream &out) const {
    out << "ReducedLL[bbox=" << bbox_ << "]";
}


void ReducedLL::fill(grib_info &info) const  {
    NOTIMP;
}


void ReducedLL::fill(api::MIRJob &job) const  {
    bbox_.fill(job);
    job.set("pl", pl_);
    job.set("Nj", Nj_);
    NOTIMP;
}


void ReducedLL::cropToDomain(const param::MIRParametrisation &parametrisation, context::Context & ctx) const {
    if (!atlasDomain().isGlobal()) {
        action::AreaCropper cropper(parametrisation, bbox_);
        cropper.execute(ctx);
    }
}


atlas::grid::Grid *ReducedLL::atlasGrid() const {
    return new atlas::grid::lonlat::ReducedLonLat(pl_.size(), &pl_[0], atlasDomain());
}


atlas::grid::Domain ReducedLL::atlasDomain() const {
  return atlasDomain(bbox_);
}


atlas::grid::Domain ReducedLL::atlasDomain(const util::BoundingBox& bbox) const {
    typedef eckit::FloatCompare<double> cmp;

    ASSERT(pl_.size());

    long maxpl = pl_[0];
    for (size_t i=1; i<pl_.size(); ++i) {
        maxpl = std::max(maxpl, pl_[i]);
    }

    const double ew = bbox.east() - bbox.west();
    const double inc_west_east = maxpl? ew/double(maxpl) : 0.;

    const bool isPeriodicEastWest = cmp::isApproximatelyEqual(ew + inc_west_east, 360.);
    const bool includesPoleNorth  = cmp::isApproximatelyEqual(bbox.north(),  90.);
    const bool includesPoleSouth  = cmp::isApproximatelyEqual(bbox.south(), -90.);

    return atlas::grid::Domain(
                includesPoleNorth?   90 : bbox.north(),
                isPeriodicEastWest?   0 : bbox.west(),
                includesPoleSouth?  -90 : bbox.south(),
                isPeriodicEastWest? 360 : bbox.east() );
}


void ReducedLL::validate(const std::vector<double> &values) const {
    size_t count = 0;
    for (size_t i = 0; i < pl_.size(); i++) {
        count += pl_[i];
    }
    ASSERT(values.size() == count);
}


class ReducedLLIterator: public Iterator {

    const std::vector<long>& pl_;
    const atlas::grid::Domain domain_;

    size_t ni_;
    const size_t nj_;

    size_t i_;
    size_t j_;
    size_t p_;

    size_t count_;

    virtual void print(std::ostream &out) const {
        out << "ReducedLLIterator["
            <<  "domain=" << domain_
            << ",ni="     << ni_
            << ",nj="     << nj_
            << ",i="      << i_
            << ",j="      << j_
            << ",p="      << p_
            << ",count="  << count_
            << "]";
    }

    virtual bool next(double &lat, double &lon) {

        const double ew = (domain_.east() - domain_.west());
        const double inc_north_south = (domain_.north() - domain_.south()) / (nj_ - 1);

        while (j_ < nj_ && i_ < ni_) {

            lat = domain_.north() - j_ * inc_north_south;
            lon = domain_.west() + (i_ * ew) / ni_;

            i_++;
            if (i_ == ni_) {

                j_++;
                i_ = 0;

                if (j_ < nj_) {
                    ASSERT(p_ < pl_.size());
                    ni_ = pl_[p_++];
                }

            }

            if (domain_.contains(lon,lat)) {
                count_++;
                return true;
            }
        }
        return false;
    }

public:

    ReducedLLIterator(size_t nj, const std::vector<long>& pl, const atlas::grid::Domain& dom) :
        pl_(pl),
        domain_(dom),
        nj_(nj),
        i_(0),
        j_(0),
        p_(0),
        count_(0) {

        ni_ = pl_[p_++];

        // eckit::Log::debug<LibMir>() << *this << std::endl;
    }

};


Iterator *ReducedLL::unrotatedIterator() const {
    return new ReducedLLIterator(Nj_, pl_, atlasDomain());
}


Iterator* ReducedLL::rotatedIterator() const {
    return unrotatedIterator();
}


namespace {
static RepresentationBuilder<ReducedLL> reducedLL("reduced_ll"); // Name is what is returned by grib_api
}


}  // namespace latlon
}  // namespace repres
}  // namespace mir

