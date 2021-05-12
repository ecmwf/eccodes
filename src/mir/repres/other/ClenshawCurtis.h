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

#include "mir/repres/Gridded.h"
#include "mir/util/Domain.h"


namespace mir {
namespace util {
class Rotation;
}
}  // namespace mir


namespace mir {
namespace repres {
namespace other {


class ClenshawCurtis : public Gridded {
public:
    // -- Exceptions
    // None

    // -- Constructors

    ClenshawCurtis(size_t N);
    ClenshawCurtis(const param::MIRParametrisation&);
    ClenshawCurtis(const ClenshawCurtis&) = delete;

    // -- Destructor

    ~ClenshawCurtis() override;

    // -- Convertors
    // None

    // -- Operators

    ClenshawCurtis& operator=(const ClenshawCurtis&) = delete;

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
    // None

    // -- Methods

    static const std::vector<double>& latitudes(size_t N);

    // -- Overridden methods

    // from Representation
    void print(std::ostream&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    const std::string name_;
    util::Domain domain_;
    std::vector<long> pl_;
    size_t N_;

    // -- Methods

    const std::vector<double>& latitudes() const { return latitudes(N_); }
    std::vector<double> calculateUnrotatedGridBoxLatitudeEdges() const;

    // -- Overridden methods

    // From Representation
    Iterator* iterator() const override;
    atlas::Grid atlasGrid() const override;
    bool extendBoundingBoxOnIntersect() const override;
    bool sameAs(const Representation&) const override;
    size_t numberOfPoints() const override;
    std::vector<util::GridBox> gridBoxes() const override;
    void fill(grib_info&) const override;
    void fill(util::MeshGeneratorParameters&) const override;
    void makeName(std::ostream&) const override;
    void validate(const MIRValuesVector&) const override;

    // From Gridded
    bool getLongestElementDiagonal(double&) const override;
    void estimate(api::MIREstimation&) const override;
    util::Domain domain() const override;

    // // From Representation
    // virtual const Representation* croppedRepresentation(const util::BoundingBox&) const;
    // virtual const Representation* globalise(data::MIRField&) const;
    // virtual const Representation* truncate(size_t truncation, const MIRValuesVector&, MIRValuesVector&) const;
    // virtual const std::string& uniqueName() const;
    // virtual size_t truncation() const;
    // virtual std::string factory() const;  // Return factory name
    // virtual util::BoundingBox extendBoundingBox(const util::BoundingBox&) const;
    // virtual util::Domain domain() const;
    // virtual void comparison(std::string&) const;
    // virtual void reorder(long scanningMode, MIRValuesVector&) const;
    // virtual void fill(api::MIRJob&) const;
    //
    // // From Gridded
    // virtual void setComplexPacking(grib_info&) const;
    // virtual void setSimplePacking(grib_info&) const;
    // virtual void setGivenPacking(grib_info&) const;

    // // ???
    // virtual bool includesNorthPole() const { return domain_.includesPoleNorth(); }
    // virtual bool includesSouthPole() const { return domain_.includesPoleSouth(); }
    // virtual bool isPeriodicWestEast() const { return domain_.isPeriodicWestEast(); }

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
