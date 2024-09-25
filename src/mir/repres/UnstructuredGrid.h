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

#include <vector>

#include "mir/repres/Gridded.h"


namespace eckit {
class PathName;
}


namespace mir::repres {


class UnstructuredGrid : public Gridded {
public:
    // -- Exceptions
    // None

    // -- Constructors

    explicit UnstructuredGrid(const eckit::PathName&);
    explicit UnstructuredGrid(const param::MIRParametrisation&);
    UnstructuredGrid(const std::vector<double>& latitudes, const std::vector<double>& longitudes,
                     const util::BoundingBox& = util::BoundingBox());

    UnstructuredGrid(const UnstructuredGrid&) = delete;
    UnstructuredGrid(UnstructuredGrid&&)      = delete;

    // -- Destructor

    ~UnstructuredGrid() override;

    // -- Convertors
    // None

    // -- Operators

    void operator=(const UnstructuredGrid&)  = delete;
    void operator=(const UnstructuredGrid&&) = delete;

    // -- Methods

    static void save(const eckit::PathName&, const std::vector<double>& latitudes,
                     const std::vector<double>& longitudes, bool binary);

    // -- Overridden methods


    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members
    // None

    // -- Methods

    // From Representation
    bool extendBoundingBoxOnIntersect() const override;
    void print(std::ostream&) const override;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    std::vector<double> latitudes_;
    std::vector<double> longitudes_;

    // -- Methods
    // None

    // -- Overridden methods

    void fillGrib(grib_info&) const override;
    void fillJob(api::MIRJob&) const override;
    void fillMeshGen(util::MeshGeneratorParameters&) const override;

    atlas::Grid atlasGrid() const override;
    void validate(const MIRValuesVector&) const override;

    util::Domain domain() const override;
    Iterator* iterator() const override;
    void makeName(std::ostream&) const override;
    bool sameAs(const Representation&) const override;

    // Domain operations
    bool isPeriodicWestEast() const override;
    bool includesNorthPole() const override;
    bool includesSouthPole() const override;

    size_t numberOfPoints() const override;
    const Gridded* croppedRepresentation(const util::BoundingBox&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace mir::repres
