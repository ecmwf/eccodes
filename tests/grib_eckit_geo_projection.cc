/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


// Print the projection spec of the grid (by eckit::geo) of each GRIB message, from its gridSpec


#include <cstdio>
#include <exception>
#include <memory>

#include "eckit/geo/Grid.h"
#include "eckit/geo/Projection.h"

#include "eccodes/eccodes.h"
#include "eccodes/geo/eckit.h"


int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::fprintf(stderr, "usage: %s file.grib\n", argv[0]);
        return 1;
    }

    auto* in = std::fopen(argv[1], "rb");
    if (in == nullptr) {
        std::perror(argv[1]);
        return 1;
    }

    eccodes::geo::eckit_main_init();

    int err = 0;
    for (codes_handle* h = nullptr; nullptr != (h = codes_handle_new_from_file(nullptr, in, PRODUCT_GRIB, &err));) {
        std::unique_ptr<codes_handle, decltype(&codes_handle_delete)> handle(h, &codes_handle_delete);

        char spec[10240] = {
            0,
        };
        size_t length = sizeof(spec);
        if (codes_get_string(h, "gridSpec", spec, &length) != CODES_SUCCESS) {
            return 1;
        }

        try {
            std::unique_ptr<const eckit::geo::Grid> grid(eckit::geo::GridFactory::make_from_string(spec));
            std::printf("%s\n", grid->projection().spec_str().c_str());
        }
        catch (std::exception& e) {
            std::fprintf(stderr, "%s\n", e.what());
            return 1;
        }
    }

    std::fclose(in);
    return err == CODES_SUCCESS ? 0 : 1;
}
