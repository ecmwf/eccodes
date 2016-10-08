/*
 * (C) Copyright 1996-2015 ECMWF.
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


#ifndef RotatedLLOffset_H
#define RotatedLLOffset_H

#include "mir/repres/latlon/RotatedLL.h"


namespace mir {
namespace repres {
namespace latlon {


class RotatedLLOffset : public RotatedLL {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    RotatedLLOffset(const util::BoundingBox &bbox,
        const util::Increments &increments,
        const util::Rotation &rotation,
        double northwards, double eastwards);

    // -- Destructor

    virtual ~RotatedLLOffset();

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

    util::Rotation rotation_;
    double northwards_;
    double eastwards_;

    // -- Methods

    void print(std::ostream &) const; // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

  private:

    // RotatedLLOffset();

    // No copy allowed

    RotatedLLOffset(const RotatedLLOffset &);
    RotatedLLOffset &operator=(const RotatedLLOffset &);

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    virtual atlas::grid::Grid *atlasGrid() const;
    virtual Iterator* unrotatedIterator() const; // Before rotation
    virtual Iterator* rotatedIterator() const; // Before rotation

    // From RegularLL
    virtual const RotatedLLOffset *cropped(const util::BoundingBox &bbox) const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None

};


}  // namespace latlon
}  // namespace repres
}  // namespace mir


#endif

