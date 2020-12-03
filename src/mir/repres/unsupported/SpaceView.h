/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef SpaceView_H
#define SpaceView_H

#include "mir/repres/Gridded.h"


namespace mir {
namespace repres {


class SpaceView : public Gridded {
public:
    // -- Exceptions
    // None

    // -- Constructors

    SpaceView(const param::MIRParametrisation&);

    // -- Destructor

    virtual ~SpaceView() override;  // Change to virtual if base class

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

    void print(std::ostream&) const override;  // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    SpaceView();

    // No copy allowed

    SpaceView(const SpaceView&);
    SpaceView& operator=(const SpaceView&);

    // -- Members


    // -- Methods
    // None


    // -- Overridden methods

    virtual void fill(grib_info&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    // friend ostream& operator<<(ostream& s,const SpaceView& p)
    //	{ p.print(s); return s; }
};


}  // namespace repres
}  // namespace mir
#endif
