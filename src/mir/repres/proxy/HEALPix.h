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


#include <string>
#include <vector>

#include "mir/repres/proxy/ProxyGrid.h"


namespace mir::repres::unsupported {
class HEALPixNested;
}


namespace mir::repres::proxy {


class HEALPix final : public ProxyGrid {
public:
    // -- Types

    class Reorder {
    public:
        explicit Reorder(int Nside);

        int size() const { return 12 * Nside_ * Nside_; }
        int nside() const { return Nside_; }

        int nest_to_ring(int) const;
        int ring_to_nest(int) const;

    private:
        const int Nside_;  // up to 2^13
        const int Npix_;
        const int Ncap_;
        const int k_;
    };

    // -- Exceptions
    // None

    // -- Constructors

    explicit HEALPix(size_t Nside, const std::string& orderingConvention = "ring");
    explicit HEALPix(const param::MIRParametrisation&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    size_t Nside() const { return Nside_; }

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    const ::atlas::Grid::Spec spec_;
    mutable ::atlas::Grid grid_;
    size_t Nside_;
    std::string orderingConvention_;

    // -- Methods

    std::string name() const;

    // -- Overridden methods

    // from Representation
    bool sameAs(const Representation&) const override;
    void makeName(std::ostream&) const override;

    void fillGrib(grib_info&) const override;
    void fillMeshGen(util::MeshGeneratorParameters&) const override;
    void fillJob(api::MIRJob&) const override;

    void print(std::ostream&) const override;

    std::vector<util::GridBox> gridBoxes() const override;

    // from ProxyGrid
    const ::atlas::Grid& atlasGridRef() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace mir::repres::proxy
