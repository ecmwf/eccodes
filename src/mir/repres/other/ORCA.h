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

    std::string name_;
    std::string type_;
    char subtype_;

    std::vector<double> latitudes_;
    std::vector<double> longitudes_;

    ::atlas::Grid grid_;

    // -- Methods
    // None

    // -- Overridden methods

    // from Representation
    bool sameAs(const Representation&) const;
    void validate(const MIRValuesVector&) const;
    size_t numberOfPoints() const;
    void makeName(std::ostream&) const;

    void fill(grib_info&) const;
    void fill(util::MeshGeneratorParameters&) const;

    bool includesNorthPole() const { return true; }
    bool includesSouthPole() const { return true; }
    bool isPeriodicWestEast() const { return true; }

    Iterator* iterator() const;
    atlas::Grid atlasGrid() const;

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
