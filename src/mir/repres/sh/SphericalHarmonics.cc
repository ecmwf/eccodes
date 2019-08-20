/*
 * (C) Copyright 1996- ECMWF.
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


#include "mir/repres/sh/SphericalHarmonics.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "mir/api/MIRJob.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"
#include "mir/api/MIREstimation.h"


namespace mir {
namespace repres {
namespace sh {


SphericalHarmonics::SphericalHarmonics(const param::MIRParametrisation &parametrisation) {
    ASSERT(parametrisation.get("truncation", truncation_));
}


SphericalHarmonics::SphericalHarmonics(size_t truncation) :
    truncation_(truncation) {
}


SphericalHarmonics::~SphericalHarmonics() = default;


void SphericalHarmonics::print(std::ostream &out) const {
    out << "SphericalHarmonics["
        << "truncation=" << truncation_
        << "]";
}


void SphericalHarmonics::makeName(std::ostream& out) const {
    out << "T" << truncation_;
}


bool SphericalHarmonics::sameAs(const Representation& other) const { NOTIMP; }


bool SphericalHarmonics::isPeriodicWestEast() const {
    return true;
}


bool SphericalHarmonics::includesNorthPole() const {
    return true;
}


bool SphericalHarmonics::includesSouthPole() const {
    return true;
}


void SphericalHarmonics::fill(grib_info &info) const  {
    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_SH;
    info.grid.truncation = static_cast<long>(truncation_);

    // Decision: MIR-131
    info.packing.packing_type = GRIB_UTIL_PACKING_TYPE_SPECTRAL_COMPLEX; // Check if this is needed, why does grib_api not copy input?
    info.packing.computeLaplacianOperator = 1;
    info.packing.truncateLaplacian = 1;
    // info.packing.laplacianOperator = 0;
}


void SphericalHarmonics::fill(api::MIRJob& job) const {
    job.set("resol", truncation_);
}


size_t SphericalHarmonics::truncation() const {
    return truncation_;
}


void SphericalHarmonics::comparison(std::string& comparator) const {
    comparator = "spectral";
}


void SphericalHarmonics::truncate(
    size_t truncation_from,
    size_t truncation_to,
    const MIRValuesVector& in,
    MIRValuesVector& out ) {

    ASSERT(truncation_to != truncation_from);

    size_t insize = number_of_complex_coefficients(truncation_from) * 2;
    ASSERT(in.size() == insize);

    size_t outsize = number_of_complex_coefficients(truncation_to) * 2;
    out.resize(outsize);

    int delta = truncation_from - truncation_to;
    size_t i = 0;
    size_t j = 0;

    if (delta > 0) {
        size_t t1 = truncation_to + 1;
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
        size_t t1 = truncation_to + 1;
        size_t t_ = truncation_from ;

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
        ASSERT(j == insize);
    }


}


void SphericalHarmonics::interlace_spectra(
        data::MIRValuesVector& interlaced,
        const data::MIRValuesVector& spectra,
        size_t truncation,
        size_t numberOfComplexCoefficients,
        size_t index,
        size_t indexTotal) {
    ASSERT(0 <= index && index < indexTotal);
    ASSERT(numberOfComplexCoefficients * 2 * indexTotal == interlaced.size());

    if (spectra.size() != numberOfComplexCoefficients * 2) {
        const std::string msg = "MIRSpectralTransform: expected field values size " +
                std::to_string(numberOfComplexCoefficients * 2) +
                " (T=" + std::to_string(truncation) + "), " +
                " got " + std::to_string(spectra.size());
        eckit::Log::error() << msg << std::endl;
        throw eckit::UserError(msg);
    }

    for (size_t j = 0; j < numberOfComplexCoefficients * 2; ++j) {
        interlaced[ j * indexTotal + index ] = spectra[j];
    }
}


const Representation *SphericalHarmonics::truncate(
        size_t truncation,
        const MIRValuesVector& in,
        MIRValuesVector& out) const {


    if (truncation == truncation_) {
        return 0;
    }

    truncate(truncation_, truncation, in, out);

    return new SphericalHarmonics(truncation);
}


void SphericalHarmonics::validate(const MIRValuesVector& values) const {
    ASSERT(values.size() == number_of_complex_coefficients(truncation_) * 2);
}


void SphericalHarmonics::setComplexPacking(grib_info& info) const {
    info.packing.packing_type = GRIB_UTIL_PACKING_TYPE_SPECTRAL_COMPLEX;
}


void SphericalHarmonics::setSimplePacking(grib_info& info) const {
    info.packing.packing_type = GRIB_UTIL_PACKING_TYPE_SPECTRAL_SIMPLE;
}

void SphericalHarmonics::estimate(api::MIREstimation& estimation) const {
    estimation.packing("spectral_complex"); // Will be overriden
}


namespace {
static RepresentationBuilder<SphericalHarmonics> sphericalHarmonics("sh"); // Name is what is returned by grib_api
}


}  // namespace sh
}  // namespace repres
}  // namespace mir

