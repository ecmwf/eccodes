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

#include "mir/repres/Representation.h"
#include "mir/util/Atlas.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/Types.h"


namespace mir {
namespace repres {
namespace other {


class SpaceView : public Representation {
public:
    // -- Types

    using LinearSpacing = ::atlas::grid::LinearSpacing;
    using PointLonLat   = ::atlas::PointLonLat;

    struct Projection {
        Point2 xy(const Point2&) const;
        Point2 lonlat(const Point2&) const;
        void hash(eckit::Hash&) const;
    };

    // -- Exceptions
    // None

    // -- Constructors

    SpaceView(const param::MIRParametrisation&);
    SpaceView(const SpaceView&) = delete;

    // -- Destructor

    ~SpaceView() override;

    // -- Convertors
    // None

    // -- Operators

    SpaceView& operator=(const SpaceView&) = delete;

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

    double earthMajorAxis_;
    double earthMinorAxis_;

    util::BoundingBox bbox_;
    ::atlas::RegularGrid grid_;
    LinearSpacing x_;
    LinearSpacing y_;

    Projection projection_;

    // -- Methods
    // None

    // -- Overridden methods

    // from Representation
    bool sameAs(const Representation&) const override;

    bool includesNorthPole() const override { return false; }
    bool includesSouthPole() const override { return false; }
    bool isPeriodicWestEast() const override { return false; }

    void fill(grib_info&) const override;
    void fill(util::MeshGeneratorParameters&) const override;
    void reorder(long scanningMode, MIRValuesVector&) const override;
    void validate(const MIRValuesVector&) const override;
    void makeName(std::ostream&) const override;
    void print(std::ostream&) const override;
    bool extendBoundingBoxOnIntersect() const override { return true; }

    ::atlas::Grid atlasGrid() const override { return grid_; }
    Iterator* iterator() const override;
    size_t numberOfPoints() const override;

    util::Domain domain() const override;
    const util::BoundingBox& boundingBox() const override;
    bool getLongestElementDiagonal(double&) const override;
    void estimate(api::MIREstimation&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace other
}  // namespace repres
}  // namespace mir
