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
#include "mir/util/Atlas.h"
#include "mir/util/Log.h"


namespace mir::repres::proxy {


class ProxyGrid : public Gridded {
public:
    // -- Exceptions
    // None

    // -- Constructors

    ProxyGrid(const ProxyGrid&) = delete;
    ProxyGrid(ProxyGrid&&)      = delete;

    // -- Destructor

    ~ProxyGrid() override = default;

    // -- Convertors
    // None

    // -- Operators

    ProxyGrid& operator=(const ProxyGrid&) = delete;
    ProxyGrid& operator=(ProxyGrid&&)      = delete;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Constructors

    ProxyGrid();

    // -- Members
    // None

    // -- Methods

    virtual const ::atlas::Grid& atlasGridRef() const = 0;
    ::atlas::Grid atlasGrid() const override;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    // from Representation
    void validate(const MIRValuesVector& values) const override;

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


}  // namespace mir::repres::proxy
