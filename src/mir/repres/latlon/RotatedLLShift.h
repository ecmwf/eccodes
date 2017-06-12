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


#ifndef RotatedLLShift_H
#define RotatedLLShift_H

#include "mir/repres/latlon/RegularLLShift.h"


namespace mir {
namespace repres {
namespace latlon {


class RotatedLLShift : public RegularLLShift {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    RotatedLLShift(const util::BoundingBox &bbox,
        const util::Increments &increments,
        const util::Rotation &rotation,
        const util::Shift& shift);

    // -- Destructor

    virtual ~RotatedLLShift();

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

    // -- Methods

    void print(std::ostream &) const; // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

  private:

    // RotatedLLShift();

    // No copy allowed

    RotatedLLShift(const RotatedLLShift &);
    RotatedLLShift &operator=(const RotatedLLShift &);

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    virtual void fill(grib_info &) const;
    virtual void fill(api::MIRJob &) const;

    virtual atlas::Grid atlasGrid() const;
    virtual Iterator* rotatedIterator() const; // Before rotation

    // From RegularLL
    virtual const RotatedLLShift *cropped(const util::BoundingBox &bbox) const;
    virtual void makeName(std::ostream&) const;
    virtual bool sameAs(const Representation& other) const;

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

