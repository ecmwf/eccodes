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


#ifndef FromPL_H
#define FromPL_H

#include "mir/repres/reduced/Reduced.h"
#include "mir/util/BoundingBox.h"


namespace mir {
namespace repres {
namespace reduced {

class FromPL : public Reduced {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    FromPL(const param::MIRParametrisation &);

    // -- Destructor

    virtual ~FromPL(); // Change to virtual if base class

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

    FromPL(size_t, const std::vector<long> &, const util::BoundingBox &);

    // -- Members

    std::vector<long> pl_;

    // -- Methods

    // void print(std::ostream &) const; // Change to virtual if base class

    // -- Overridden methods
    virtual void fill(grib_info &) const;
    virtual atlas::Grid *atlasGrid() const;
    virtual const std::vector<long>& pls() const;


    // -- Class members
    // None

    // -- Class methods
    // None

  private:



    // No copy allowed

    FromPL(const FromPL &);
    FromPL &operator=(const FromPL &);

    // -- Members



    // -- Methods
    // None


    // -- Overridden methods



    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const FromPL& p)
    //  { p.print(s); return s; }

};

}
}  // namespace repres
}  // namespace mir
#endif

