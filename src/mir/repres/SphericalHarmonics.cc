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


#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/utils/Translator.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"

#include "mir/repres/SphericalHarmonics.h"


namespace mir {
namespace repres {


SphericalHarmonics::SphericalHarmonics(const param::MIRParametrisation &parametrisation) {

    eckit::Translator<std::string, size_t> s2d;
    std::string value;

    ASSERT(parametrisation.get("truncation", value));
    truncation_ = s2d(value);

}


SphericalHarmonics::SphericalHarmonics(size_t truncation):
    truncation_(truncation) {
}


SphericalHarmonics::~SphericalHarmonics() {
}


void SphericalHarmonics::print(std::ostream &out) const {
    out << "SphericalHarmonics["
        << "truncation=" << truncation_
        << "]";
}


void SphericalHarmonics::fill(grib_info &info) const  {
    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_SH;
    info.grid.truncation = truncation_;
    // info.packing.computeLaplacianOperator = 1;
    // info.packing.truncateLaplacian = 1;
    // info.packing.laplacianOperator = 0;
    info.packing.packing_type = GRIB_UTIL_PACKING_TYPE_SPECTRAL_COMPLEX; // Check if this is needed, why does grib_api not copy input?
}

size_t SphericalHarmonics::truncation() const {
    return truncation_;
}

Representation *SphericalHarmonics::truncate(size_t truncation,
        const std::vector<double> &in, std::vector<double> &out) const {

    // WARNING: Untested code!

    if (truncation == truncation_) {
        return 0;

    }

    size_t insize = (truncation_ + 1) * (truncation_ + 2);
    ASSERT(in.size() == insize);

    SphericalHarmonics *sh = new SphericalHarmonics(truncation);

    size_t outsize = (truncation + 1) * (truncation + 2);
    out = std::vector<double>(outsize);

    int delta = truncation_ - truncation;
    size_t i = 0;
    size_t j = 0;

    if (delta > 0) {
        size_t t1 = truncation + 1;
        for (size_t m = 0; m < t1; m++) {
            for (size_t n = m ; n < t1; n++) {
                out[i++] = in[j++];
                out[i++] = in[j++];
            }
            j += delta;
            j += delta;
        }
        ASSERT(i == outsize);
    } else {
        // Pad with zeros
        size_t t1 = truncation + 1;
        size_t t_ = truncation_ ;

        for (size_t m = 0; m < t1; m++) {
            for (size_t n = m ; n < t1; n++) {
                if (m > t_ || n > t_) {
                    out[i++] = 0;
                    out[i++] = 0;
                } else {
                    out[i++] = in[j++];
                    out[i++] = in[j++];
                }
            }
        }
        // std::cout << j << " " << insize << std::endl;
        ASSERT(j == insize);
    }

    // std::cout << i << " " << outsize << std::endl;
    // std::cout << j << " " << insize << std::endl;

    // ASSERT(i == outsize);
    // ASSERT(j == insize);

    return sh;
}


namespace {
static RepresentationBuilder<SphericalHarmonics> sphericalHarmonics("sh"); // Name is what is returned by grib_api
}


}  // namespace repres
}  // namespace mir

