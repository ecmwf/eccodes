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


#include "eccodes/geo/Shape.h"

#include "eckit/geo/Figure.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"


namespace eccodes::geo
{


Shape::Shape(const ::eckit::geo::Figure& figure) :
    figure_(figure) {}


void Shape::fillGrib(grib_info& info) const
{
    static const std::string EDITION{ "edition" };
    static const std::string SHAPE{ "shapeOfTheEarth" };
    static const std::string A{ "earthMajorAxis" };
    static const std::string B{ "earthMinorAxis" };

    // GRIB edition=2 only, check if shape is already set/provided
    auto edition  = info.packing.editionNumber;
    auto code     = 6L;
    bool provided = false;

    for (long j = 0; j < info.packing.extra_settings_count; ++j) {
        const auto& set = info.packing.extra_settings[j];

        if (set.name == EDITION && set.type == CODES_TYPE_LONG) {
            edition = set.long_value;
        }
        else if (set.name == SHAPE && set.type == CODES_TYPE_LONG) {
            code     = set.long_value;
            provided = true;
        }
    }

    if (edition != 2) {
        return;
    }

    if (!provided) {
        code = figure_.spherical() ? 1L : 7L;
        info.extra_set(SHAPE.c_str(), code);
    }

    switch (code) {
        case 1:
            info.extra_set("radius", figure_.R());
            break;
        case 3:
            info.extra_set(A.c_str(), figure_.a() / 1000.);
            info.extra_set(B.c_str(), figure_.b() / 1000.);
        case 6:
            break;
        case 7:
            info.extra_set(A.c_str(), figure_.a());
            info.extra_set(B.c_str(), figure_.b());
            break;
        default:
            throw ::eckit::geo::exception::FigureError("Shape: unsupported " + SHAPE + ": " + std::to_string(code), Here());
    }
}


}  // namespace eccodes::geo
