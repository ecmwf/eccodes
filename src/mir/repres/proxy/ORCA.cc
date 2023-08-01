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


#include "mir/repres/proxy/ORCA.h"


#include <ostream>
#include <utility>
#include <vector>

#include "mir/repres/Iterator.h"
#include "mir/util/Atlas.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir::repres::proxy {


// order is important for makeName()
static const std::vector<std::pair<std::string, std::string>> grib_keys{
    {"orca_name", "unstructuredGridType"}, {"orca_arrangement", "unstructuredGridSubtype"}, {"uid", "uuidOfHGrid"}};


ORCA::ORCA(const std::string& uid) : spec_(atlas::grid::SpecRegistry::get(uid)) {}


ORCA::ORCA(const param::MIRParametrisation& param) :
    ORCA([&param]() {
        std::string uid;
        ASSERT(param.get("uid", uid));
        return uid;
    }()) {}


ORCA::~ORCA() = default;


bool ORCA::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const ORCA*>(&other);
    return (o != nullptr) && spec_.getString("uid") == o->spec_.getString("uid");
}


void ORCA::makeName(std::ostream& out) const {
    const auto* sep = "";
    for (const auto& key : grib_keys) {
        out << sep << spec_.getString(key.first);
        sep = "_";
    }
}


void ORCA::print(std::ostream& out) const {
    out << "ORCA[spec=" << spec_ << "]";
}


const atlas::Grid& ORCA::atlasGridRef() const {
    return grid_ ? grid_ : (grid_ = atlas::Grid(spec_));
}


void ORCA::fillMeshGen(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "orca";
    }
}


void ORCA::fillGrib(grib_info& info) const {
    info.grid.grid_type        = GRIB_UTIL_GRID_SPEC_UNSTRUCTURED;
    info.packing.editionNumber = 2;

    for (const auto& key : grib_keys) {
        auto value = spec_.getString(key.first);
        info.extra_set(key.second.c_str(), value.c_str());
    }
}


void ORCA::fillJob(api::MIRJob& job) const {
    // Just an empty function for now
}


static const RepresentationBuilder<ORCA> __grid("orca");


}  // namespace mir::repres::proxy
