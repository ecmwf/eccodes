/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/repres/gauss/reduced/FromPL.h"

#include "eckit/utils/MD5.h"

#include "mir/api/mir_config.h"
#include "mir/util/Domain.h"


namespace mir::repres::gauss::reduced {


FromPL::FromPL(const param::MIRParametrisation& parametrisation) : Reduced(parametrisation) {}


FromPL::FromPL(size_t N, const std::vector<long>& pl, const util::BoundingBox& bbox, double angularPrecision) :
    Reduced(N, pl, bbox, angularPrecision) {}


void FromPL::makeName(std::ostream& out) const {
    out << "R" << N_ << "-";

    eckit::MD5 md5;
    for (const auto& j : pls()) {
        md5 << j;
    }

    out << std::string(md5);
    bbox_.makeName(out);
}


bool FromPL::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const FromPL*>(&other);
    return (o != nullptr) && (pls() == o->pls()) && Reduced::sameAs(other);
}


void FromPL::fillGrib(grib_info& info) const {
    Reduced::fillGrib(info);
}


void FromPL::fillJob(api::MIRJob& job) const {
    Reduced::fillJob(job);
}


atlas::Grid FromPL::atlasGrid() const {
#if mir_HAVE_ATLAS
    return atlas::ReducedGaussianGrid(pls(), domain());
#else
    NOTIMP;
#endif
}


}  // namespace mir::repres::gauss::reduced
