/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @date Apr 2015


#ifndef mir_repres_LambertAzimuthalEqualArea_h
#define mir_repres_LambertAzimuthalEqualArea_h

#include "mir/repres/Gridded.h"
#include "mir/repres/Iterator.h"
#include "mir/util/BoundingBox.h"


namespace mir {
namespace repres {


class LambertAzimuthalEqualArea : public Gridded {
public:

    // -- Exceptions
    // None

    // -- Contructors

    LambertAzimuthalEqualArea(const param::MIRParametrisation&);
    LambertAzimuthalEqualArea(const LambertAzimuthalEqualArea&) = delete;
    LambertAzimuthalEqualArea& operator=(const LambertAzimuthalEqualArea&) = delete;

    // -- Destructor

    ~LambertAzimuthalEqualArea();

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
    // None

    // -- Methods

    void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members

    PointLatLon reference_;
    PointLatLon firstLL_;
    Point2 firstXY_;
    double radius_;
    double Dx_;
    double Dy_;
    size_t nx_;
    size_t ny_;

    // -- Methods
    // None

    // -- Overridden methods

    virtual size_t numberOfPoints() const;
    virtual atlas::Grid atlasGrid() const;

    virtual bool isPeriodicWestEast() const;
    virtual bool includesNorthPole() const;
    virtual bool includesSouthPole() const;

    virtual void fill(grib_info&) const;
    void validate(const MIRValuesVector& values) const;
    Iterator* iterator() const;

    virtual void makeName(std::ostream&) const;
    virtual bool sameAs(const Representation&) const;
    virtual void fill(util::MeshGeneratorParameters&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None

};


}  // namespace repres
}  // namespace mir


#endif

