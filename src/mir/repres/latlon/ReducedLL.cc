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
/// @author Pedro Maciel
/// @date Apr 2015

#include "mir/repres/latlon/ReducedLL.h"

#include <iostream>

#include "atlas/grids/LocalGrid.h"
#include "atlas/grids/ReducedLonLatGrid.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Compare.h"
#include "mir/action/misc/AreaCropper.h"
#include "mir/repres/Iterator.h"

namespace mir {
namespace repres {
namespace latlon {


ReducedLL::ReducedLL(const param::MIRParametrisation &parametrisation):
    bbox_(parametrisation) {
    ASSERT(parametrisation.get("pl", pl_));
    ASSERT(parametrisation.get("Nj", Nj_));
}

ReducedLL::~ReducedLL() {
}


void ReducedLL::print(std::ostream &out) const {
    out << "ReducedLL[bbox=" << bbox_ << "]";
}


void ReducedLL::fill(grib_info &info) const  {
    NOTIMP;
}


void ReducedLL::cropToDomain(const param::MIRParametrisation &parametrisation, data::MIRField &field) const {
    if (!globalDomain()) {
        action::AreaCropper cropper(parametrisation, bbox_);
        cropper.execute(field);
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

atlas::Grid *ReducedLL::atlasGrid() const {

    if ( globalDomain() ) {
        // FIXME: we are missing the distrubution of latitudes
        return new atlas::grids::ReducedLonLatGrid(pl_.size(), &pl_[0], atlas::grids::ReducedLonLatGrid::INCLUDES_POLES);
    } else {
        atlas::Domain domain(bbox_.north(), bbox_.west(), bbox_.south(), bbox_.east() );
        // FIXME: we are missing the distrubution of latitudes
        return new atlas::grids::ReducedLonLatGrid(pl_.size(), &pl_[0], atlas::grids::ReducedLonLatGrid::INCLUDES_POLES, domain);
    }
}


void ReducedLL::validate(const std::vector<double> &values) const {
    size_t count = 0;
    for (size_t i = 0; i < pl_.size(); i++) {
        count += pl_[i];
    }
    ASSERT(values.size() == count);
}


class ReducedLLIterator: public Iterator {

    const std::vector<long> &pl_;
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
                    // eckit::Log::info() << "ni = " << ni_ << std::endl;
                }

            }

            // eckit::Log::info() << "++++++ " << lat << " " << lon << " - " << bbox_ << " -> " << bbox_.contains(lat, lon) << std::endl;

            if (bbox_.contains(lat, lon)) {
                count_++;
                return true;
            }
        }
        return false;
    }

  public:

    // TODO: Consider keeping a reference on the latitudes and bbox, to avoid copying

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

        // eckit::Log::info() << "ReducedLLIterator ni=" << ni_ << " nj=" << nj_
        // << " j=" << j_ << " " << bbox_ << " ns=" << ns_ << std::endl;


    }

    ~ReducedLLIterator() {
        std::cout << "~ReducedLLIterator " << count_ << std::endl;
        // ASSERT(count_ == ni_ * nj_);
    }

};

Iterator *ReducedLL::unrotatedIterator() const {
    // Use a global bounding box if global domain, to avoid rounding issues
    // due to GRIB (in)accuracies
    return new ReducedLLIterator(Nj_, pl_, globalDomain() ? util::BoundingBox() : bbox_);
}


namespace {
static RepresentationBuilder<ReducedLL> reducedLL("reduced_ll"); // Name is what is returned by grib_api
}

}  // namespace latlon
}  // namespace repres
}  // namespace mir

