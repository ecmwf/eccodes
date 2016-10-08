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


#ifndef RegularLLOffset_H
#define RegularLLOffset_H

#include "mir/repres/latlon/RegularLL.h"
#include "mir/util/Rotation.h"


namespace mir {
namespace repres {
namespace latlon {


class RegularLLOffset : public RegularLL {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    RegularLLOffset(const util::BoundingBox &bbox,
        const util::Increments &increments,
        double northwards, double eastwards);

    // -- Destructor

    virtual ~RegularLLOffset();

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

    double northwards_;
    double eastwards_;

    // -- Methods

    void print(std::ostream &) const; // Change to virtual if base class

    // -- Overridden methods
    // None
    virtual atlas::grid::Grid *atlasGrid() const;

    // -- Class members
    // None

    // -- Class methods
    // None

  private:

    // RegularLLOffset();

    // No copy allowed

    RegularLLOffset(const RegularLLOffset &);
    RegularLLOffset &operator=(const RegularLLOffset &);

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    virtual Iterator* unrotatedIterator() const; // Before rotation

    // From RegularLL
    virtual const RegularLLOffset *cropped(const util::BoundingBox &bbox) const;

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

