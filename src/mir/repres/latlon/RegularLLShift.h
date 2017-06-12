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


#ifndef RegularLLShift_H
#define RegularLLShift_H

#include "mir/repres/latlon/RegularLL.h"
#include "mir/util/Rotation.h"
#include "mir/util/Shift.h"


namespace mir {
namespace repres {
namespace latlon {


class RegularLLShift : public RegularLL {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    RegularLLShift(const util::BoundingBox &bbox,
        const util::Increments &increments,
        const util::Shift& shift);

    // -- Destructor

    virtual ~RegularLLShift();

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

    util::Shift shift_;

    // -- Methods

    void print(std::ostream &) const; // Change to virtual if base class
    virtual void makeName(std::ostream&) const;
    virtual bool sameAs(const Representation& other) const;
    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

  private:

    // RegularLLShift();

    // No copy allowed

    RegularLLShift(const RegularLLShift &);
    RegularLLShift &operator=(const RegularLLShift &);

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    virtual Iterator* unrotatedIterator() const; // Before rotation

    // From RegularLL
    virtual const RegularLLShift *cropped(const util::BoundingBox &bbox) const;


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

