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

#include "atlas/interpolation/method/knn/GridBox.h"

#include "mir/repres/Iterator.h"
#include "mir/util/Atlas.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/Log.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir::repres::proxy {


HEALPix::HEALPix(size_t Nside, const std::string& orderingConvention) :
    Nside_(Nside), orderingConvention_(orderingConvention) {
    ASSERT(Nside_ > 0);
    ASSERT(orderingConvention_ == "ring");
}


HEALPix::HEALPix(const param::MIRParametrisation& param) : Nside_(0), orderingConvention_("none") {
    param.get("Nside", Nside_);
    ASSERT(Nside_ > 0);
    ASSERT(param.get("orderingConvention", orderingConvention_));
    ASSERT(orderingConvention_ == "ring");
}


HEALPix::~HEALPix() = default;


const ::atlas::Grid& HEALPix::atlasGridRef() const {
    ::atlas::util::Config config;
    config.set("name", name());
    config.set("Nside", Nside_);
    config.set("orderingConvention", orderingConvention_);
    return grid_ ? grid_ : (grid_ = ::atlas::grid::detail::grid::Grid::create(config));
}


bool HEALPix::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const HEALPix*>(&other);
    return (o != nullptr) && Nside_ == o->Nside_ && orderingConvention_ == o->orderingConvention_;
}


std::string HEALPix::name() const {
    return "H" + std::to_string(Nside_);
}


void HEALPix::makeName(std::ostream& out) const {
    out << name();
}


void HEALPix::fillGrib(grib_info& info) const {
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_HEALPIX;
    info.grid.N         = static_cast<long>(Nside_);
    info.grid.longitudeOfFirstGridPointInDegrees = 45.0;

    info.extra_set("orderingConvention", orderingConvention_.c_str());
}


void HEALPix::fillMeshGen(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "structured";
    }
}


void HEALPix::fillJob(api::MIRJob&) const {
    // Nothing to do. Just declare the method to avoid
    // a call to the default one
}


void HEALPix::print(std::ostream& out) const {
    out << "HEALPix[name=" << name() << "]";
}


std::vector<util::GridBox> HEALPix::gridBoxes() const {
    ::atlas::interpolation::method::GridBoxes boxes(atlasGridRef(), false);
    std::vector<util::GridBox> mirBoxes(boxes.size());
    std::transform(boxes.cbegin(), boxes.cend(), mirBoxes.begin(), [](const auto& other) {
        return util::GridBox{other.north(), other.west(), other.south(), other.east()};
    });
    return mirBoxes;
}


static const RepresentationBuilder<HEALPix> __grid("healpix");


}  // namespace mir::repres::proxy
