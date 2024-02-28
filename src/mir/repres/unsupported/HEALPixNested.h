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


class HEALPixNested final : public Gridded {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit HEALPixNested(size_t Nside) : ring_(Nside) {}
    explicit HEALPixNested(const param::MIRParametrisation& param) : ring_(param) {}

    HEALPixNested(const HEALPixNested&) = delete;
    HEALPixNested(HEALPixNested&&)      = delete;

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators

    HEALPixNested& operator=(const HEALPixNested&) = delete;
    HEALPixNested& operator=(HEALPixNested&&)      = delete;

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

    // -- Methods
    // None

    // -- Overridden methods

    bool sameAs(const Representation&) const override;
    void makeName(std::ostream&) const override;

    void fillGrib(grib_info&) const override;
    void fillMeshGen(util::MeshGeneratorParameters&) const override;
    void fillJob(api::MIRJob&) const override;

    void print(std::ostream&) const override;

    std::vector<util::GridBox> gridBoxes() const override;

    ::atlas::Grid atlasGrid() const override;

    void validate(const MIRValuesVector&) const override;

    size_t numberOfPoints() const override;

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }
    bool isPeriodicWestEast() const override { return true; }

    Iterator* iterator() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace mir::repres::unsupported
