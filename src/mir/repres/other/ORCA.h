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


#ifndef mir_repres_other_ORCA_h
#define mir_repres_other_ORCA_h

#include <string>

#include "mir/api/Atlas.h"
#include "mir/repres/Gridded.h"
#include "mir/util/Domain.h"


namespace mir {
namespace repres {
namespace other {


class ORCA : public Gridded {
public:
    // -- Exceptions
    // None

    // -- Constructors

    ORCA(const std::string& name);
    ORCA(const param::MIRParametrisation&);
    ORCA(const ORCA&) = delete;

    // -- Destructor

    virtual ~ORCA();

    // -- Convertors
    // None

    // -- Operators

    ORCA& operator=(const ORCA&) = delete;

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
    // None

    // -- Methods
    // None

    // -- Overridden methods

    // from Representation
    void print(std::ostream&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    const std::string name_;
    ::atlas::Grid grid_;
    util::Domain domain_;

    // -- Methods
    // None

    // -- Overridden methods

    // from Representation
    bool sameAs(const Representation&) const;
    void validate(const MIRValuesVector&) const;
    size_t numberOfPoints() const;
    void fill(grib_info&) const;
    void makeName(std::ostream&) const;

    bool includesNorthPole() const { return domain_.includesPoleNorth(); }
    bool includesSouthPole() const { return domain_.includesPoleSouth(); }
    bool isPeriodicWestEast() const { return domain_.isPeriodicWestEast(); }

    Iterator* iterator() const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace other
}  // namespace repres
}  // namespace mir


#endif
