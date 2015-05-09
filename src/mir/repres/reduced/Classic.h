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


#ifndef Classic_H
#define Classic_H

#include "mir/repres/Gridded.h"
#include "mir/util/BoundingBox.h"


namespace mir {
namespace repres {
namespace reduced {

class Classic : public Gridded {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    Classic(size_t);

    // -- Destructor

    virtual ~Classic(); // Change to virtual if base class

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

    Classic(size_t, const util::BoundingBox &);

    // -- Members

    size_t N_;
    util::BoundingBox bbox_;

    // -- Methods

    // void print(std::ostream &) const; // Change to virtual if base class

    // -- Overridden methods
    virtual void fill(grib_info &) const;
    virtual atlas::Grid *atlasGrid() const;

    // -- Class members
    // None

    // -- Class methods
    // None

  private:



    // No copy allowed

    Classic(const Classic &);
    Classic &operator=(const Classic &);

    // -- Members


    // -- Methods
    // None


    // -- Overridden methods



    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const Classic& p)
    //  { p.print(s); return s; }

};

} // namespace
}  // namespace repres
}  // namespace mir
#endif

