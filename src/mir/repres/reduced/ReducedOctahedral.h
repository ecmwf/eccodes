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


#ifndef ReducedOctahedral_H
#define ReducedOctahedral_H

#include "mir/repres/reduced/Octahedral.h"


namespace mir {
namespace repres {
namespace reduced {

class ReducedOctahedral : public Octahedral {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    ReducedOctahedral(size_t);

    // -- Destructor

    virtual ~ReducedOctahedral(); // Change to virtual if base class

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
    // None

    // -- Methods

    void print(std::ostream &) const; // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

  private:

    ReducedOctahedral(long, const util::BoundingBox &);


    // No copy allowed

    ReducedOctahedral(const ReducedOctahedral &);
    ReducedOctahedral &operator=(const ReducedOctahedral &);

    // -- Members


    // -- Methods
    // None


    // -- Overridden methods


    virtual Representation *clone() const;
    virtual Reduced *cropped(const util::BoundingBox &bbox, const std::vector<long> &) const ;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const ReducedOctahedral& p)
    //  { p.print(s); return s; }

};

}
}  // namespace repres
}  // namespace mir
#endif

