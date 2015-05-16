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


#ifndef RotatedFromPL_H
#define RotatedFromPL_H

#include "mir/repres/reduced/FromPL.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/Rotation.h"


namespace mir {
namespace repres {
namespace reduced {

class RotatedFromPL : public FromPL {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    RotatedFromPL(const param::MIRParametrisation &);
    RotatedFromPL(long, const std::vector<long> &, const util::BoundingBox &, const util::Rotation&);

    // -- Destructor

    virtual ~RotatedFromPL(); // Change to virtual if base class

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



    // No copy allowed

    RotatedFromPL(const RotatedFromPL &);
    RotatedFromPL &operator=(const RotatedFromPL &);

    // -- Members

    // -- Methods
    // None


    // -- Overridden methods

    virtual void fill(grib_info &) const;
    atlas::Grid *atlasGrid() const;
    virtual Representation *clone() const;
    virtual Gaussian *cropped(const util::BoundingBox &bbox, const std::vector<long> &) const ;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const RotatedFromPL& p)
    //  { p.print(s); return s; }

};

}
}  // namespace repres
}  // namespace mir
#endif

