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


#ifndef Regular_H
#define Regular_H

#include "mir/repres/Gridded.h"
#include "mir/util/BoundingBox.h"


namespace mir {
namespace repres {
namespace regular {

class Regular : public Gridded {
  public:

// -- Exceptions
    // None

// -- Contructors

    Regular(const param::MIRParametrisation&);
    Regular(size_t);
    Regular(size_t, const util::BoundingBox& bbox);

// -- Destructor

    virtual ~Regular(); // Change to virtual if base class

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
    size_t N_;
    util::BoundingBox bbox_;


// -- Methods

    virtual void fill(grib_info&) const;
    virtual atlas::Grid* atlasGrid() const;

// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

  private:

    Regular();

// No copy allowed

    Regular(const Regular&);
    Regular& operator=(const Regular&);

// -- Members



// -- Methods
    // None


// -- Overridden methods

    virtual void validate(const std::vector<double>&) const;


// -- Class members
    // None

// -- Class methods
    // None

// -- Friends

    //friend ostream& operator<<(ostream& s,const Regular& p)
    //  { p.print(s); return s; }

};

} // namespace regular

}  // namespace repres
}  // namespace mir
#endif

