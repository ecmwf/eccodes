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


#ifndef ReducedGG_H
#define ReducedGG_H

#include "mir/repres/Gridded.h"
#include "mir/util/BoundingBox.h"


namespace mir {
namespace repres {


class ReducedGG : public Gridded {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    ReducedGG(const param::MIRParametrisation &);
    ReducedGG(size_t);

    // -- Destructor

    virtual ~ReducedGG(); // Change to virtual if base class

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

    ReducedGG(long, const std::vector<long> &, const util::BoundingBox &);


    // No copy allowed

    ReducedGG(const ReducedGG &);
    ReducedGG &operator=(const ReducedGG &);

    // -- Members

    size_t N_;
    std::vector<long> pl_;
    util::BoundingBox bbox_;

    // -- Methods
    // None


    // -- Overridden methods

    virtual void fill(grib_info &) const;
    atlas::Grid *atlasGrid() const;
    virtual Representation *clone() const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const ReducedGG& p)
    //  { p.print(s); return s; }

};


}  // namespace repres
}  // namespace mir
#endif

