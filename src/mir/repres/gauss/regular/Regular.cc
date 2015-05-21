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

#include "mir/repres/gauss/regular/Regular.h"


#include <iostream>

#include "atlas/grids/GaussianGrid.h"

#include "eckit/exception/Exceptions.h"
#include "atlas/grids/GaussianLatitudes.h"
#include "mir/repres/Iterator.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"

namespace mir {
namespace repres {
namespace regular {

Regular::Regular(const param::MIRParametrisation &parametrisation):
    Gaussian(parametrisation) {

    // Only global input supported...

    size_t Ni;
    ASSERT(parametrisation.get("Ni", Ni));
    ASSERT(N_ * 4 == Ni);

    size_t Nj;
    ASSERT(parametrisation.get("Nj", Nj));
    ASSERT(N_ * 2 == Nj);
}

Regular::Regular(size_t N):
    Gaussian(N) {

}


Regular::Regular(size_t N, const util::BoundingBox &bbox):
    Gaussian(N, bbox) {

}

Regular::~Regular() {
}


void Regular::fill(grib_info &info) const  {

    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_GG;
    info.grid.Nj = N_ * 2;

    double we = 90.0 / N_; // FIXME: Just a guess

    info.grid.Ni = computeN(bbox_.west(), bbox_.east(), we, "ni", "west", "east");
    info.grid.N = N_;
    info.grid.iDirectionIncrementInDegrees = we;

    bbox_.fill(info);

    /*
        Comment in libemos is:

        "grib_api to set global area in full precision for gaussian grid"

        TODO: check and document

    */

    size_t j = info.packing.extra_settings_count++;
    info.packing.extra_settings[j].type = GRIB_TYPE_LONG;
    info.packing.extra_settings[j].name = "global";
    info.packing.extra_settings[j].long_value = bbox_.global() ? 1 : 0;

}

atlas::Grid *Regular::atlasGrid() const {
    return new atlas::grids::GaussianGrid(N_);
}


void Regular::validate(const std::vector<double> &values) const {
    if (bbox_.global()) {
        ASSERT(values.size() == (N_ * 2) * (N_ * 4));
    } else {
        std::auto_ptr<Iterator> it(iterator());
        double lat;
        double lon;

        size_t count = 0;
        while (it->next(lat, lon)) {
            if (bbox_.contains(lat, lon)) {
                count++;
            }
        }

        eckit::Log::info() << "Reduced::validate " << values.size() << " " << count << std::endl;

        ASSERT(values.size() == count);
    }
}


class RegularIterator: public Iterator {

    std::vector<double> latitudes_;
    util::BoundingBox bbox_;

    size_t ni_;
    size_t nj_;

    size_t i_;
    size_t j_;

    size_t count_;

    virtual void print(std::ostream &out) const {
        out << "RegularIterator[]";
    }

    virtual bool next(double &lat, double &lon) {
        while (j_ < nj_ && i_ < ni_) {
            lat = latitudes_[j_];
            lon = (i_ * 360.0) / ni_;
            i_++;
            if (i_ == ni_) {

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

  public:

    // TODO: Consider keeping a reference on the latitudes and bbox, to avoid copying

    RegularIterator(const std::vector <double> &latitudes, size_t N, const util::BoundingBox &bbox):
        latitudes_(latitudes),
        i_(0),
        j_(0),
        ni_(N * 4),
        nj_(N * 2),
        count_(0),
        bbox_(bbox) {
        ASSERT(latitudes_.size() == nj_);

    }

    ~RegularIterator() {
        // ASSERT(count_ == ni_ * nj_);
    }

};

Iterator *Regular::iterator() const {
    return new RegularIterator(latitudes(), N_, bbox_);
}


}  // namespace regular
}  // namespace repres
}  // namespace mir

