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


#include "mir/repres/Gaussian.h"
#include "mir/util/Grib.h"
#include "eckit/log/Timer.h"
#include "mir/util/BoundingBox.h"
#include "mir/repres/Iterator.h"

#include "mir/param/MIRParametrisation.h"
#include "eckit/exception/Exceptions.h"
#include "mir/repres/Iterator.h"


#include "atlas/grids/GaussianLatitudes.h"
#include "mir/util/Grib.h"

#include <map>

namespace mir {
namespace repres {


Gaussian::Gaussian(size_t N):
    N_(N) {
}

Gaussian::Gaussian(size_t N, const util::BoundingBox &bbox):
    N_(N), bbox_(bbox) {
}

Gaussian::Gaussian(const param::MIRParametrisation &parametrisation):
    bbox_(parametrisation) {
    ASSERT(parametrisation.get("N", N_));

    bool global = false;
    parametrisation.get("global", global);  // Grib_api will work out if a gaussian is global

    if (global) {
        bbox_ = util::BoundingBox();
    } else {
        // Adding support for non-global gaussian grids requires atlas support
        eckit::StrStream os;
        os << "Non-global gaussian grid not yet supported (N=" << N_ << ", bbox=" << bbox_ << ")" << eckit::StrStream::ends;
        throw eckit::SeriousBug(os);
    }
}

Gaussian::~Gaussian() {

}



const std::vector <double> &Gaussian::latitudes() const {
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

}  // namespace repres
}  // namespace mir

