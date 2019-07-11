/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_repres_atlas_AtlasRegularGrid_h
#define mir_repres_atlas_AtlasRegularGrid_h

#include "mir/api/Atlas.h"

#include "mir/repres/Gridded.h"
#include "mir/repres/Iterator.h"

namespace mir {
namespace repres {
namespace atlas {

using eckit::geometry::XYZCOORDS;

class AtlasRegularGrid : public Gridded {
public:
    // -- Types

    using RegularGrid   = ::atlas::RegularGrid;
    using LinearSpacing = ::atlas::grid::LinearSpacing;
    using Projection    = ::atlas::Projection;

    // -- Exceptions
    // None

    // -- Contructors

    AtlasRegularGrid(const param::MIRParametrisation&, Projection);
    AtlasRegularGrid(const AtlasRegularGrid&) = delete;
    AtlasRegularGrid& operator=(const AtlasRegularGrid&) = delete;

    // -- Destructor

    virtual ~AtlasRegularGrid();

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

protected:
    // -- Members

    RegularGrid grid_;
    LinearSpacing x_;
    LinearSpacing y_;

    // -- Methods
    // None

    // -- Overridden methods

    // from Representation
    virtual bool sameAs(const Representation&) const;

    virtual bool includesNorthPole() const;
    virtual bool includesSouthPole() const;
    virtual bool isPeriodicWestEast() const;

    virtual void fill(grib_info&) const;
    virtual void fill(util::MeshGeneratorParameters&) const;
    virtual void reorder(long scanningMode, MIRValuesVector&) const;
    virtual void validate(const MIRValuesVector&) const;
    virtual void makeName(std::ostream&) const;
    virtual void print(std::ostream&) const;

    virtual ::atlas::Grid atlasGrid() const;
    virtual Iterator* iterator() const;
    virtual size_t numberOfPoints() const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace atlas
}  // namespace repres
}  // namespace mir


#endif
