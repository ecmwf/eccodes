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


#ifndef RegularLL_H
#define RegularLL_H

#include "mir/repres/LatLon.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/Increments.h"


namespace mir {
namespace repres {


class RegularLL : public LatLon {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    RegularLL(const param::MIRParametrisation &);
    RegularLL(const util::BoundingBox &bbox, const util::Increments &increments);


    // -- Destructor

    virtual ~RegularLL(); // Change to virtual if base class

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

  protected:

    // -- Members

    // -- Methods


    // -- Overridden methods
    void print(std::ostream &) const; // Change to virtual if base class
    virtual atlas::Grid *atlasGrid() const;
    virtual void fill(grib_info &) const;

    // -- Class members
    // None

    // -- Class methods
    // None

  private:


    // No copy allowed

    RegularLL(const RegularLL &);
    RegularLL &operator=(const RegularLL &);

    // -- Members


    // -- Methods
    // None


    // Called by crop()
    virtual RegularLL *cropped(const util::BoundingBox &bbox) const;



    // -- Overridden methods

    virtual Representation *clone() const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const RegularLL& p)
    //  { p.print(s); return s; }

};


}  // namespace repres
}  // namespace mir
#endif

