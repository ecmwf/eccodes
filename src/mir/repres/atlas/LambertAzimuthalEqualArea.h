/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_repres_atlas_LambertAzimuthalEqualArea_h
#define mir_repres_atlas_LambertAzimuthalEqualArea_h

#include "mir/api/Atlas.h"

#include "mir/repres/Iterator.h"
#include "mir/repres/atlas/AtlasRegularGrid.h"


namespace mir {
namespace repres {
namespace atlas {


class LambertAzimuthalEqualArea : public AtlasRegularGrid {
public:
    // -- Exceptions
    // None

    // -- Contructors

    LambertAzimuthalEqualArea(const param::MIRParametrisation&);
    LambertAzimuthalEqualArea(double x0, double x1, size_t nx, double y0, double y1, size_t ny, double standardParallel,
                              double centralLongitude);

    LambertAzimuthalEqualArea(const LambertAzimuthalEqualArea&) = delete;
    LambertAzimuthalEqualArea& operator=(const LambertAzimuthalEqualArea&) = delete;

    // -- Destructor

    virtual ~LambertAzimuthalEqualArea();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
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
    // None
};


}  // namespace atlas
}  // namespace repres
}  // namespace mir


#endif
