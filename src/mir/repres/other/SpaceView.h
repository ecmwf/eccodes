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


namespace eckit {
class MD5;
}


namespace mir {
namespace repres {
namespace other {


class SpaceView : public Representation {
public:
    // -- Types

    using LinearSpacing = ::atlas::grid::LinearSpacing;
    using PointLonLat   = ::atlas::PointLonLat;
    using Projection    = ::atlas::Projection;

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

    static void remove_invalid_values(const param::MIRParametrisation&, MIRValuesVector&);

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Types

    struct space_view_t {
        space_view_t(const param::MIRParametrisation&);

        Projection projection() const { return projection_; }
        LinearSpacing x() const { return x_; }
        LinearSpacing y() const { return y_; }
        double h() const { return h_; }
        double Lop() const { return Lop_; }

        void hash(eckit::MD5& md5) const;

    private:
        Projection projection_;
        LinearSpacing x_;
        LinearSpacing y_;
        double h_;
        double Lop_;
    };

    // -- Members

    space_view_t sv_;
    util::BoundingBox bbox_;
    size_t numberOfPoints_;

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
