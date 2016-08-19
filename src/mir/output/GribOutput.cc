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

#include "mir/output/GribOutput.h"

#include <istream>

#include "eckit/io/DataHandle.h"

#include "mir/data/MIRField.h"
#include "mir/input/MIRInput.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Representation.h"
#include "mir/util/Grib.h"
#include "mir/packing/Packer.h"
#include "eckit/log/Plural.h"
#include "mir/config/LibMir.h"
#include "mir/action/context/Context.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/AutoLock.h"


namespace mir {
namespace output {

static eckit::Mutex local_mutex;


#define X(a) eckit::Log::debug<LibMir>() << "  GRIB encoding: " << #a << " = " << a << std::endl
#define Y(a) oss << " " << #a << "=" << a


class HandleFree {
    grib_handle *h_;
public:
    HandleFree(grib_handle *h): h_(h) {}
    ~HandleFree() {
        if (h_) grib_handle_delete(h_);
    }
};


GribOutput::GribOutput() {
}


GribOutput::~GribOutput() {
}


size_t GribOutput::copy(const param::MIRParametrisation &param, context::Context& ctx) { // Not iterpolation performed

    input::MIRInput& input = ctx.input();

    size_t total = 0;
    for (size_t i = 0; i < input.dimensions(); i++) {
        grib_handle *h = input.gribHandle(i); // Base class will throw an exception is input cannot provide a grib_handle

        ASSERT(h);

        const void *message;
        size_t size;

        GRIB_CALL(grib_get_message(h, &message, &size));

        out(message, size, false);
        total += size;
    }

    return total;
}

size_t GribOutput::save(const param::MIRParametrisation &parametrisation, context::Context& ctx) {



    data::MIRField& field = ctx.field();
    input::MIRInput& input = ctx.input();

    field.validate();

    size_t total = 0;

    for (size_t i = 0; i < field.dimensions(); i++) {

        // Protect grib_api
        eckit::AutoLock<eckit::Mutex> lock(local_mutex);

        grib_handle *h = input.gribHandle(i); // Base class will throw an exception is input cannot provide a grib_handle

#if 0

        static const char *dump = getenv("MIR_DUMP_GRIB_HANDLES");
        if (dump) {

            static int n = 0;
            char fname[1024];
            sprintf(fname, "grib-dump-%04d.txt", n++);
            FILE *f = fopen(fname, "w");
            if (f) {
                grib_dump_content(h, f, NULL, 0, NULL);
                fclose(f);
            }
        }

#endif

        grib_info info = {{0},};



        /* bitmap */
        info.grid.bitmapPresent = field.hasMissing() ? 1 : 0;
        info.grid.missingValue = field.missingValue();

        /* Packing options */

        info.packing.packing = GRIB_UTIL_PACKING_SAME_AS_INPUT;
        info.packing.accuracy = GRIB_UTIL_ACCURACY_SAME_BITS_PER_VALUES_AS_INPUT;

        long bits;
        if (parametrisation.get("user.accuracy", bits)) {
            info.packing.accuracy = GRIB_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES;
            info.packing.bitsPerValue = bits;
        }

        // Ask last representation to update info

        field.representation()->fill(info);

        long paramId = field.paramId(i);
        if (paramId) {
            int n = info.packing.extra_settings_count++;
            info.packing.extra_settings[n].name = "paramId";
            info.packing.extra_settings[n].type = GRIB_TYPE_LONG;
            info.packing.extra_settings[n].long_value = paramId;
        }

        std::string packing;
        if (parametrisation.get("user.packing", packing)) {
            const packing::Packer &packer = packing::Packer::lookup(packing);
#if 0
            packer.fill(info, *field.representation());
#else
            if (field.values(i).size() < 4) {

                // There is a bug in grib_api if the user ask 1 value and select second-order
                // Once this fixed, remove this code
                eckit::Log::debug<LibMir>() << "Field has "
                                         << eckit::Plural(field.values(i).size(), "value")
                                         << ", ignoring packer "
                                         << packer
                                         << std::endl;


            } else {
                packer.fill(info, *field.representation());
            }

#endif
        }

        if (LibMir::instance().tracing()) {
            X(info.grid.grid_type);
            X(info.grid.Ni);
            X(info.grid.Nj);
            X(info.grid.iDirectionIncrementInDegrees);
            X(info.grid.jDirectionIncrementInDegrees);
            X(info.grid.longitudeOfFirstGridPointInDegrees);
            X(info.grid.longitudeOfLastGridPointInDegrees);
            X(info.grid.latitudeOfFirstGridPointInDegrees);
            X(info.grid.latitudeOfLastGridPointInDegrees);
            X(info.grid.uvRelativeToGrid);
            X(info.grid.latitudeOfSouthernPoleInDegrees);
            X(info.grid.longitudeOfSouthernPoleInDegrees);
            X(info.grid.iScansNegatively);
            X(info.grid.jScansPositively);
            X(info.grid.N);
            X(info.grid.bitmapPresent);
            X(info.grid.missingValue);
            X(info.grid.pl_size);
            for (size_t i = 0; i < info.grid.pl_size; i++) {
                X(info.grid.pl[i]);
                if (i > 4) break;
            }

            X(info.grid.truncation);
            X(info.grid.orientationOfTheGridInDegrees);
            X(info.grid.DyInMetres);
            X(info.grid.DxInMetres);
            X(info.packing.packing_type);
            X(info.packing.packing);
            X(info.packing.boustrophedonic);
            X(info.packing.editionNumber);
            X(info.packing.accuracy);
            X(info.packing.bitsPerValue);
            X(info.packing.decimalScaleFactor);
            X(info.packing.computeLaplacianOperator);
            X(info.packing.truncateLaplacian);
            X(info.packing.laplacianOperator);
            X(info.packing.deleteLocalDefinition);
            // X(info.packing.extra_settings);
            X(info.packing.extra_settings_count);
            for (size_t i = 0; i < info.packing.extra_settings_count; i++) {
                X(info.packing.extra_settings[i].name);
                switch (info.packing.extra_settings[i].type) {
                case GRIB_TYPE_LONG:
                    X(info.packing.extra_settings[i].long_value);
                    break;
                case GRIB_TYPE_DOUBLE:
                    X(info.packing.extra_settings[i].double_value);
                    break;
                case GRIB_TYPE_STRING:
                    X(info.packing.extra_settings[i].string_value);
                    break;
                }

            }
        }


        int flags = 0;
        int err = 0;

        const std::vector<double> &values = field.values(i);


        for(auto j = values.begin(); j != values.end(); ++j) {
            double x = *j;
            ASSERT(x < 3e38);
            ASSERT(x > -3e38);
        }


        grib_handle *result = grib_util_set_spec(h, &info.grid, &info.packing, flags, &values[0], values.size(), &err);
        HandleFree hf(result); // Make sure handle deleted even in case of exception


        if(err == GRIB_WRONG_GRID) {
            std::ostringstream oss;

            oss << "GRIB_WRONG_GRID: ";

            Y(info.grid.grid_type);
            Y(info.grid.Ni);
            Y(info.grid.Nj);
            Y(info.grid.iDirectionIncrementInDegrees);
            Y(info.grid.jDirectionIncrementInDegrees);
            Y(info.grid.longitudeOfFirstGridPointInDegrees);
            Y(info.grid.longitudeOfLastGridPointInDegrees);
            Y(info.grid.latitudeOfFirstGridPointInDegrees);
            Y(info.grid.latitudeOfLastGridPointInDegrees);

            throw eckit::SeriousBug(oss.str());
        }


        GRIB_CALL(err);

        const void *message;
        size_t size;

        GRIB_CALL(grib_get_message(result, &message, &size));

        out(message, size, true);
        total += size;

    }

    return total;
}


#undef Y
#undef X


}  // namespace output
}  // namespace mir

