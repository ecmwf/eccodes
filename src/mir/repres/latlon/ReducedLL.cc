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
    if (!globalDomain()) {
        action::AreaCropper cropper(parametrisation, bbox_);
        cropper.execute(ctx);
    }
}


bool ReducedLL::globalDomain() const {

    // FIXME: cache
    if (bbox_.north() == 90 && bbox_.south() == -90) {
        if (Nj_ == pl_.size()) {
            ASSERT(pl_.size());
            long maxpl = pl_[0];
            for (size_t i = 1; i < pl_.size(); i++) {
                maxpl = std::max(maxpl, pl_[i]);
            }

            double ew = 360.0 / maxpl;

            if (eckit::FloatCompare<double>::isApproximatelyEqual(bbox_.east() - bbox_.west() + ew, 360.)) {
                return true;
            }
        }

    }
    return false;
}


atlas::grid::Grid *ReducedLL::atlasGrid() const {
    // FIXME: we are missing the distribution of latitudes
    return new atlas::grid::lonlat::ReducedLonLat(pl_.size(), &pl_[0], atlasDomain());
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
    util::BoundingBox bbox_;

    size_t ni_;
    size_t nj_;

    size_t i_;
    size_t j_;
    size_t p_;

    size_t count_;
    double north_;
    double west_;
    double east_;
    double ns_;


    virtual void print(std::ostream &out) const {
        out << "ReducedLLIterator[]";
    }

    virtual bool next(double &lat, double &lon) {
        while (j_ < nj_ && i_ < ni_) {
            lat = north_ - j_ * ns_;
            lon = west_ + (i_ * (east_ - west_)) / ni_;
            i_++;
            if (i_ == ni_) {

                j_++;
                i_ = 0;

                if (j_ < nj_) {
                    ASSERT(p_ < pl_.size());
                    ni_ = pl_[p_++];
                }

            }

            if (bbox_.contains(lat, lon)) {
                count_++;
                return true;
            }
        }
        return false;
    }

public:

    ReducedLLIterator(size_t nj, const std::vector<long> &pl, const util::BoundingBox &bbox):
        pl_(pl),
        bbox_(bbox),
        nj_(nj),
        i_(0),
        j_(0),
        p_(0),
        count_(0) {


        north_ = bbox_.north();
        west_ = bbox_.west();
        east_ = bbox_.east();
        ns_ = (bbox_.north() - bbox_.south()) / (nj_ - 1);
        ni_ = pl_[p_++];

        // eckit::Log::trace<MIR>() << *this << std::endl;
    }

};


Iterator *ReducedLL::unrotatedIterator() const {
    // Use a global bounding box if global domain, to avoid rounding issues
    // due to GRIB (in)accuracies
    return new ReducedLLIterator(Nj_, pl_, globalDomain() ? util::BoundingBox() : bbox_);
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

