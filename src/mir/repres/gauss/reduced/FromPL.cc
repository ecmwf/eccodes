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


#include "mir/repres/gauss/reduced/FromPL.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/MD5.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


FromPL::FromPL(const param::MIRParametrisation& parametrisation) :
    Reduced(parametrisation) {
}


FromPL::FromPL(size_t N, const std::vector<long>& pl, const util::BoundingBox& bbox) :
    Reduced(N, pl, bbox) {
}


FromPL::~FromPL() {
}


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
    const FromPL* o = dynamic_cast<const FromPL*>(&other);
    return o && (pls() == o->pls()) && Reduced::sameAs(other);
}


void FromPL::fill(grib_info& info) const  {
    Reduced::fill(info);
}


void FromPL::fill(api::MIRJob& job) const  {
    Reduced::fill(job);
}


atlas::Grid FromPL::atlasGrid() const {
    return atlas::grid::ReducedGaussianGrid(pls(), domain());
}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir

