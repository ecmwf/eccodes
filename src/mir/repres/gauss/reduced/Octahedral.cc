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


#include "mir/repres/gauss/reduced/Octahedral.h"


#include "mir/api/MIRJob.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


Octahedral::Octahedral(size_t N) :
    Reduced(N) {
}


Octahedral::~Octahedral() {
}


void Octahedral::fill(grib_info& info) const  {
    Reduced::fill(info);
}


void Octahedral::fill(api::MIRJob& job) const  {
    Reduced::fill(job);
    std::stringstream os;
    os << "O" << N_;
    job.set("gridname", os.str());
}


void Octahedral::makeName(std::ostream& out) const {
    out << "O" << N_;
    bbox_.makeName(out);
}


bool Octahedral::sameAs(const Representation& other) const {
    const Octahedral* o = dynamic_cast<const Octahedral*>(&other);
    return o && Reduced::sameAs(other);
}


atlas::Grid Octahedral::atlasGrid() const {
    return atlas::grid::ReducedGaussianGrid("O" + std::to_string(N_), domain());
}


const std::vector<long>& Octahedral::pls() const {

    if (pl_.size() == 0) {

        atlas::util::Config config;
        config.set("name", "O" + std::to_string(N_));
        atlas::grid::ReducedGaussianGrid grid(config);
        ASSERT(grid);

        const std::vector<long>& pl = grid.nx();
        ASSERT(pl.size() == N_ * 2);
        ASSERT(*std::min_element(pl.begin(), pl.end()) >= 2);

        pl_ = pl;
    }
    return pl_;

}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir

