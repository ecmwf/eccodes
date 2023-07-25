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


#include "mir/repres/proxy/HEALPix.h"

#include <ostream>
#include <utility>
#include <vector>

#include "mir/repres/Iterator.h"
#include "mir/util/Atlas.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/Log.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir::repres::proxygrid {


HEALPix::HEALPix(size_t N, const std::string& orderingConvention) : N_(N), orderingConvention_(orderingConvention) {
    ASSERT(N_ > 0);
    ASSERT(orderingConvention_ == "ring");
}


HEALPix::HEALPix(const param::MIRParametrisation& param) : N_(0), orderingConvention_("none") {
    param.get("N", N_);
    ASSERT(N_ > 0);
    ASSERT(param.get("orderingConvention", orderingConvention_));
    ASSERT(orderingConvention_ == "ring");
}


HEALPix::~HEALPix() = default;


const ::atlas::Grid& HEALPix::atlasGridRef() const {
    // TODO: In some way we should pass orderingConvention to atlas when atlas will support
    //       this feature
    return grid_ ? grid_ : (grid_ = ::atlas::grid::detail::grid::Grid::create(name()));
}


bool HEALPix::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const HEALPix*>(&other);
    return (o != nullptr) && N_ == o->N_ && orderingConvention_ == o->orderingConvention_;
}


std::string HEALPix::name() const {
    return "H" + std::to_string(N_);
}


void HEALPix::makeName(std::ostream& out) const {
    out << name();
}


void HEALPix::fillGrib(grib_info& info) const {
    // TODO: This a temporary hack while waiting for eccode fully support
    info.grid.grid_type = CODES_UTIL_GRID_SPEC_REGULAR_GG;

    info.extra_set("gridType", "healpix");
    info.extra_set("N", static_cast<long>(N_));
    info.extra_set("orderingConvention", orderingConvention_.c_str());
}


void HEALPix::fillMeshGen(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        // TODO: should be healpix. Still to be clarified if the
        //       healpix mesh  generator is ready and which is
        //       the ordering strategy (0-based or 1-based )
        params.meshGenerator_ = "structured";
    }
}


void HEALPix::print(std::ostream& out) const {
    out << "HEALPix[name=" << name() << "]";
}


static const RepresentationBuilder<HEALPix> HEALPix_grid("healpix");


}  // namespace mir::repres::proxygrid