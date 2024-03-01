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

#include <algorithm>
#include <cmath>
#include <ostream>

#include "eckit/log/JSON.h"
#include "eckit/types/FloatCompare.h"

#include "atlas/interpolation/method/knn/GridBox.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/unsupported/HEALPixNested.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/GridBox.h"
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

    double lon1 = 0.;
    ASSERT(param.get("longitudeOfFirstGridPointInDegrees", lon1));
    ASSERT(eckit::types::is_approximately_equal(lon1, 45.));
}


const ::atlas::Grid& HEALPix::atlasGridRef() const {
    if (!grid_) {
        grid_ = atlas::HealpixGrid(static_cast<int>(Nside_), orderingConvention_);
    }
    return grid_;
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

    info.grid.longitudeOfFirstGridPointInDegrees = 45.;

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


void HEALPix::json(eckit::JSON& j) const {
    j.startObject();
    j << "grid" << name();
    j << "type" << "healpix";
    j << "ordering" << orderingConvention_;
    j.endObject();
}


void HEALPix::print(std::ostream& out) const {
    out << "HEALPix[name=" << name() << ",ordering=" << orderingConvention_ << "]";
}


std::vector<util::GridBox> HEALPix::gridBoxes() const {
    ::atlas::interpolation::method::GridBoxes boxes(atlasGridRef(), false);
    std::vector<util::GridBox> mirBoxes(boxes.size());
    std::transform(boxes.cbegin(), boxes.cend(), mirBoxes.begin(), [](const auto& other) {
        return util::GridBox{other.north(), other.west(), other.south(), other.east()};
    });
    return mirBoxes;
}


static const struct HEALPixRepresentationBuilder : public RepresentationFactory {
    Representation* make(const param::MIRParametrisation& param) override {
        std::string orderingConvention;
        param.get("orderingConvention", orderingConvention);

        if (orderingConvention == "nested") {
            size_t Nside = 0;
            ASSERT(param.get("Nside", Nside) && Nside > 0);

            return new unsupported::HEALPixNested(Nside);
        }

        return new HEALPix(param);
    }

    explicit HEALPixRepresentationBuilder(const std::string& name) : RepresentationFactory(name) {}
} __grid("healpix");


}  // namespace mir::repres::proxy
