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


#include "eckit/geo/Exceptions.h"
#include "eckit/geo/spec/Custom.h"

#include "eccodes/eccodes.h"
#include "eccodes/geo/GribFromSpec.h"
#include "eccodes/grib_api_internal.h"


bool get_string(const grib_handle* h, const char* key, std::string& value)
{
    char buffer[10240];
    size_t size = sizeof(buffer);
    ASSERT(h);
    int err = codes_get_string(h, key, buffer, &size);

    if (err == CODES_NOT_FOUND) {
        return false;
    }

    ASSERT(err == 0);

    value = buffer;
    return true;
}


int main(int argc, char* argv[])
{
    codes_handle* h = nullptr;

    ::eckit::geo::spec::Custom spec{
        // { "grid", "1/1" },
        { "grid", "o2" },
    };

    CODES_CHECK(eccodes::geo::GribFromSpec::set(h, spec), nullptr);
    ASSERT(h != nullptr);

    const void* message = nullptr;
    size_t size         = 0;
    CODES_CHECK(codes_get_message(h, &message, &size), nullptr);

    // message should be valid
    CODES_CHECK(codes_check_message_header(message, size, PRODUCT_GRIB), nullptr);
    CODES_CHECK(codes_check_message_footer(message, size, PRODUCT_GRIB), nullptr);

    long valid = 1;
    ASSERT(codes_get_long(h, "isMessageValid", &valid) == CODES_SUCCESS);
    ASSERT(valid == 1);

    std::string type;
    ASSERT(get_string(h, "gridType", type));

    if (type == "regular_ll") {
        long Ni                 = 0;
        long Nj                 = 0;
        long numberOfDataPoints = 0;
        CODES_CHECK(codes_get_long(h, "Ni", &Ni), nullptr);
        CODES_CHECK(codes_get_long(h, "Nj", &Nj), nullptr);
        CODES_CHECK(codes_get_long(h, "numberOfDataPoints", &numberOfDataPoints), nullptr);

        ASSERT(Ni * Nj == numberOfDataPoints);
    }
    else if (type == "reduced_gg") {
        std::string name;
        ASSERT(get_string(h, "gridName", name));
        ASSERT(name == "O2");

        long N;
        CODES_CHECK(codes_get_long(h, "N", &N), nullptr);
        ASSERT(N == 2);

        long numberOfDataPoints = 0;
        CODES_CHECK(codes_get_long(h, "numberOfDataPoints", &numberOfDataPoints), nullptr);
        ASSERT(numberOfDataPoints == 88);
    }
    else {
        std::cerr << "Unexpected grid type: " << type << std::endl;
        return 1;
    }

    codes_handle_delete(h);

    return 0;
}
