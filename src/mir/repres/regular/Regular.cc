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

#include "mir/repres/regular/Regular.h"


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

Regular::Regular(const param::MIRParametrisation &parametrisation) {
    ASSERT(parametrisation.get("N", N_));

    // Only global input supported...

    size_t Ni;
    ASSERT(parametrisation.get("Ni", Ni));
    ASSERT(N_ * 4 == Ni);

    size_t Nj;
    ASSERT(parametrisation.get("Nj", Nj));
    ASSERT(N_ * 2 == Nj);
}

Regular::Regular(size_t N):
    N_(N) {

}


Regular::Regular(size_t N, const util::BoundingBox &bbox):
    N_(N),
    bbox_(bbox) {

}

Regular::~Regular() {
}


void Regular::fill(grib_info &info) const  {

    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_GG;
    info.grid.Nj = N_ * 2;

    double we = 90.0 / N_; // FIXME: Just a guess
    double ni = (bbox_.east() - bbox_.west()) / we;

    ASSERT(long(ni) == ni);
    info.grid.Ni = ni;
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
    size_t nj = N_ * 2;
    size_t ni = (bbox_.east() - bbox_.west()) / (90.0 / N_);
    ASSERT(values.size() == ni * nj);
}


class RegularIterator: public Iterator {
    std::vector<double> latitudes_;
    // double north_;
    double west_;
    // double ns_;

    size_t ni_;
    size_t nj_;

    size_t i_;
    size_t j_;

    size_t count_;
    size_t total_;



    virtual void print(std::ostream &out) const {
        out << "RegularIterator[]";
    }

    virtual bool next(double &lat, double &lon) {
        if (j_ < nj_) {
            if (i_ < ni_) {
                lat = latitudes_[j_];
                lon = west_ + (i_ * 360.0) / ni_;
                i_++;
                if (i_ == ni_) {

                    j_++;
                    i_ = 0;
                }
                count_++;
                return true;
            }
        }
        return false;
    }

  public:
    RegularIterator(const std::vector <double> &latitudes, size_t N):
        latitudes_(latitudes),
        west_(0),
        i_(0),
        j_(0),
        ni_(N * 4),
        nj_(N * 2),
        count_(0),
        total_(0) {

        ASSERT(latitudes_.size() == nj_);

    }

    ~RegularIterator() {
        ASSERT(count_ == ni_ * nj_);
    }

};

Iterator *Regular::iterator() const {
    return new RegularIterator(latitudes(), N_);
}



// Same code as Gaussian.cc, TODO: factorise
const std::vector <double> &Regular::latitudes() const {
    if (latitudes_.size() == 0) {
        if (bbox_.global()) {
            latitudes_.resize(N_ * 2);
            atlas::grids::gaussian_latitudes_npole_spole(N_, &latitudes_[0]);
        } else {
            std::vector<double> latitudes(N_ * 2);
            atlas::grids::gaussian_latitudes_npole_spole(N_, &latitudes[0]);

            double north = bbox_.north();
            double south = bbox_.south();

            for (size_t i = 0; i < latitudes.size(); i++) {
                if ((latitudes[i] >= south) && (latitudes[i] <= north)) {
                    latitudes_.push_back(latitudes[i]);
                }
            }
        }
    }
    return latitudes_;
}



}  // namespace regular
}  // namespace repres
}  // namespace mir

