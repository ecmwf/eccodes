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


#pragma once

#include "mir/repres/Gridded.h"

#include "mir/repres/proxy/HEALPix.h"


namespace mir::repres::unsupported {


// NOTE: delete class altogether once we can build HEALPix nested-ordering atlas::Grid
class HEALPixNested final : public Gridded {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit HEALPixNested(size_t Nside) : ring_(Nside) {}

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    proxy::HEALPix ring_;
    mutable std::vector<double> longitudes_;
    mutable std::vector<double> latitudes_;

    // -- Methods

    inline const Representation& ring() const { return static_cast<const Representation&>(ring_); }

    // -- Overridden methods

    bool sameAs(const Representation& other) const override {
        const auto* o = dynamic_cast<const HEALPixNested*>(&other);
        return (o != nullptr) && ring_.Nside() == o->ring_.Nside();
    }

    void makeName(std::ostream&) const override;

    void fillGrib(grib_info&) const override;
    void fillMeshGen(util::MeshGeneratorParameters& param) const override { ring().fillMeshGen(param); }
    void fillJob(api::MIRJob&) const override;

    void json(eckit::JSON&) const override;
    void print(std::ostream&) const override;

    std::vector<util::GridBox> gridBoxes() const override;

    ::atlas::Grid atlasGrid() const override;

    void validate(const MIRValuesVector& values) const override { ring().validate(values); }

    size_t numberOfPoints() const override { return ring().numberOfPoints(); }

    bool includesNorthPole() const override { return ring().includesNorthPole(); }
    bool includesSouthPole() const override { return ring().includesSouthPole(); }
    bool isPeriodicWestEast() const override { return ring().isPeriodicWestEast(); }

    Iterator* iterator() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace mir::repres::unsupported
