/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_repres_atlas_Lambert_h
#define mir_repres_atlas_Lambert_h

#include "mir/repres/Gridded.h"


namespace mir {
namespace repres {
namespace atlas {


class Lambert : public Gridded {
public:
    // -- Exceptions
    // None

    // -- Contructors

    Lambert(const param::MIRParametrisation&);

    Lambert(const Lambert&) = delete;
    Lambert& operator=(const Lambert&) = delete;

    // -- Destructor

    virtual ~Lambert() = default;

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
    // None

    // -- Overridden methods

    virtual void fill(grib_info&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    // friend ostream& operator<<(ostream& s,const Lambert& p)
    // None
};


}  // namespace atlas
}  // namespace repres
}  // namespace mir


#endif
