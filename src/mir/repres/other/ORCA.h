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

#include "atlas/util/Spec.h"

#include "mir/repres/Gridded.h"
#include "mir/util/Atlas.h"


namespace mir {
namespace repres {
namespace other {


class ORCA : public Gridded {
public:
    // -- Exceptions
    // None

    // -- Constructors

    ORCA(const std::string& id);
    ORCA(const param::MIRParametrisation&);
    ORCA(const ORCA&) = delete;

    // -- Destructor

    ~ORCA() override;

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
    void print(std::ostream&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    const atlas::util::Spec spec_;
    mutable atlas::Grid grid_;

    // -- Methods

    const atlas::Grid& atlasGridRef() const;

    // -- Overridden methods

    // from Representation
    bool sameAs(const Representation&) const override;
    void validate(const MIRValuesVector&) const override;
    size_t numberOfPoints() const override;
    void makeName(std::ostream&) const override;

    void fill(grib_info&) const override;
    void fill(util::MeshGeneratorParameters&) const override;

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }
    bool isPeriodicWestEast() const override { return true; }

    Iterator* iterator() const override;
    atlas::Grid atlasGrid() const override;

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
