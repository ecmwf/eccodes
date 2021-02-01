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


#include "mir/output/GribOutput.h"

#include <istream>
#include <memory>
#include <mutex>

#include "eckit/config/Resource.h"

#include "mir/action/context/Context.h"
#include "mir/action/io/Save.h"
#include "mir/action/io/Set.h"
#include "mir/action/plan/ActionPlan.h"
#include "mir/api/MIREstimation.h"
#include "mir/compat/GribCompatibility.h"
#include "mir/data/MIRField.h"
#include "mir/input/MIRInput.h"
#include "mir/packing/Packer.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Gridded.h"
#include "mir/repres/Representation.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/Grib.h"
#include "mir/util/Log.h"
#include "mir/util/MIRStatistics.h"
#include "mir/util/Trace.h"
#include "mir/util/Types.h"


namespace mir {
namespace output {


static std::mutex local_mutex;


#define X(a) Log::debug() << "  GRIB encoding: " << #a << " = " << a << std::endl
#define Y(a) oss << " " << #a << "=" << a


void eccodes_assertion(const char* message) {
    throw exception::SeriousBug(message);
}


GribOutput::GribOutput() : interpolated_(0), saved_(0) {}


GribOutput::~GribOutput() = default;


size_t GribOutput::interpolated() const {
    return interpolated_;
}

size_t GribOutput::saved() const {
    return saved_;
}

size_t GribOutput::copy(const param::MIRParametrisation&, context::Context& ctx) {  // No interpolation performed

    saved_++;

    const input::MIRInput& input = ctx.input();

    size_t total = 0;
    for (size_t i = 0; i < input.dimensions(); i++) {
        auto h = input.gribHandle(i);  // Base class throws if input cannot provide handle
        ASSERT(h);

        const void* message;
        size_t size;

        GRIB_CALL(codes_get_message(h, &message, &size));

        out(message, size, false);
        total += size;
    }

    return total;
}

void GribOutput::estimate(const param::MIRParametrisation& param, api::MIREstimation& estimator,
                          context::Context& ctx) const {

    const data::MIRField& field = ctx.field();
    ASSERT(field.dimensions() == 1);

    field.representation()->estimate(estimator);

    long bits = 0;
    if (param.get("accuracy", bits)) {
        estimator.accuracy(size_t(bits));
    }

    std::string packing;
    if (param.userParametrisation().get("packing", packing)) {

        estimator.packing(packing);
        // const packing::Packer &packer = packing::Packer::lookup(packing);
        // packer.estimate(estimator, *field.representation());
    }

    long edition;
    if (param.get("edition", edition)) {
        estimator.edition(size_t(edition));
    }
}


bool GribOutput::printParametrisation(std::ostream& out, const param::MIRParametrisation& param) const {
    bool ok = false;

    long bits = 0;
    if (param.userParametrisation().get("accuracy", bits)) {
        out << "accuracy=" << bits;
        ok = true;
    }

    std::string packing;
    if (param.userParametrisation().get("packing", packing)) {
        if (ok) {
            out << ",";
        }
        out << "packing=" << packing;
        ok = true;
    }

    long edition = 0;
    if (param.userParametrisation().get("edition", edition)) {
        if (ok) {
            out << ",";
        }
        out << "edition=" << edition;
        ok = true;
    }

    std::string compatibility;
    if (param.userParametrisation().get("compatibility", compatibility)) {
        if (ok) {
            out << ",";
        }
        out << "compatibility=" << compatibility;
        ok = true;

        const compat::GribCompatibility& c = compat::GribCompatibility::lookup(compatibility);
        c.printParametrisation(out, param);
    }

    return ok;
}


void GribOutput::prepare(const param::MIRParametrisation& param, action::ActionPlan& plan, input::MIRInput& input,
                         output::MIROutput& output) {
    ASSERT(!plan.ended());

    auto& user  = param.userParametrisation();
    auto& field = param.fieldParametrisation();

    std::string compatibility;
    if (user.get("compatibility", compatibility) && !compatibility.empty()) {
        plan.add(new action::io::Save(param, input, output));
        return;
    }

    bool todo = false;

    long bits1 = -1;
    if (user.get("accuracy", bits1)) {
        ASSERT(bits1 > 0);
        long bits2 = -1;
        todo       = field.get("accuracy", bits2) ? bits2 != bits1 : true;
    }

    if (!todo) {
        std::string packing1;
        if (user.get("packing", packing1)) {
            ASSERT(!packing1.empty());
            std::string packing2;
            todo = field.get("packing", packing2) ? packing2 != packing1 : true;
        }
    }

    if (!todo) {
        long edition1 = 0;
        if (user.get("edition", edition1)) {
            ASSERT(edition1 > 0);
            long edition2 = 0;
            todo          = field.get("edition", edition2) ? edition2 != edition1 : true;
        }
    }

    if (todo) {
        plan.add(plan.empty() ? static_cast<action::Action*>(new action::io::Set(param, input, output))
                              : new action::io::Save(param, input, output));
    }
}


bool GribOutput::sameParametrisation(const param::MIRParametrisation& param1,
                                     const param::MIRParametrisation& param2) const {
    long bits1 = -1;
    long bits2 = -1;

    param1.userParametrisation().get("accuracy", bits1);
    param2.userParametrisation().get("accuracy", bits2);

    if (bits1 != bits2) {
        return false;
    }

    std::string packing1;
    std::string packing2;

    param1.userParametrisation().get("packing", packing1);
    param2.userParametrisation().get("packing", packing2);

    if (packing1 != packing2) {
        return false;
    }

    long edition1 = -1;
    long edition2 = -1;

    param1.userParametrisation().get("edition", edition1);
    param2.userParametrisation().get("edition", edition2);

    if (edition1 != edition2) {
        return false;
    }

    std::string compatibility1;
    std::string compatibility2;

    param1.userParametrisation().get("compatibility", compatibility1);
    param2.userParametrisation().get("compatibility", compatibility2);

    if (compatibility1 != compatibility2) {
        return false;
    }

    if (!compatibility1.empty()) {
        const compat::GribCompatibility& c = compat::GribCompatibility::lookup(compatibility1);

        if (!c.sameParametrisation(param1, param2)) {
            return false;
        }
    }

    return true;
}


size_t GribOutput::save(const param::MIRParametrisation& parametrisation, context::Context& ctx) {
    trace::ResourceUsage usage("GribOutput::save");

    interpolated_++;

    const auto& field = ctx.field();
    const auto& input = ctx.input();

    field.validate();

    size_t total = 0;

    util::MIRStatistics::Timing saveTimer;
    auto timer(ctx.statistics().gribEncodingTimer());

    for (size_t i = 0; i < field.dimensions(); i++) {

        // Protect ecCodes and set error callback handling (throws)
        std::lock_guard<std::mutex> lock(local_mutex);
        codes_set_codes_assertion_failed_proc(&eccodes_assertion);

        // Special case where only values are changing; handle is cloned, and new values are set
        if (parametrisation.userParametrisation().has("filter")) {

            // Make sure handle deleted even in case of exception
            auto h = codes_handle_clone(input.gribHandle(i));
            HandleDeleter hf(h);

            long numberOfValues;
            GRIB_CALL(codes_get_long(h, "numberOfValues", &numberOfValues));
            if (size_t(numberOfValues) != field.values(i).size()) {
                throw exception::UserError("Using 'filter' requires preserving the number of points from input");
            }

            GRIB_CALL(codes_set_double(h, "missingValue", field.missingValue()));
            GRIB_CALL(codes_set_long(h, "bitmapPresent", field.hasMissing()));
            GRIB_CALL(codes_set_double_array(h, "values", field.values(i).data(), field.values(i).size()));

            const void* message;
            size_t size;
            GRIB_CALL(codes_get_message(h, &message, &size));

            GRIB_CALL(codes_check_message_header(message, size, PRODUCT_GRIB));
            GRIB_CALL(codes_check_message_footer(message, size, PRODUCT_GRIB));

            out(message, size, true);
            total += size;

            continue;
        }

        auto h = input.gribHandle(field.handle(i));  // Base class throws if input cannot provide handle

        grib_info info;

        // missing values
        info.grid.bitmapPresent = field.hasMissing() ? 1 : 0;
        info.grid.missingValue  = field.missingValue();

        // Packing
        info.packing.packing  = CODES_UTIL_PACKING_SAME_AS_INPUT;
        info.packing.accuracy = CODES_UTIL_ACCURACY_SAME_BITS_PER_VALUES_AS_INPUT;

        long bits;
        if (parametrisation.userParametrisation().get("accuracy", bits)) {
            info.packing.accuracy     = CODES_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES;
            info.packing.bitsPerValue = bits;
        }

        long edition;
        if (parametrisation.userParametrisation().get("edition", edition)) {
            info.packing.editionNumber = edition;
        }

        // Ask representation to update info
        field.representation()->fill(info);

        // Extra settings (paramId comes from here)
        for (const auto& k : field.metadata(i)) {
            long j = info.packing.extra_settings_count++;
            ASSERT(j < long(sizeof(info.packing.extra_settings) / sizeof(info.packing.extra_settings[0])));

            info.packing.extra_settings[j].name       = k.first.c_str();
            info.packing.extra_settings[j].type       = CODES_TYPE_LONG;
            info.packing.extra_settings[j].long_value = k.second;
        }

        std::string packing;
        if (parametrisation.userParametrisation().get("packing", packing)) {
            std::unique_ptr<packing::Packer> packer(packing::PackerFactory::build(
                packing, parametrisation.userParametrisation(), parametrisation.fieldParametrisation()));
            packer->fill(info, *field.representation());
        }

        bool remove = false;
        parametrisation.get("delete-local-definition", remove);
        info.packing.deleteLocalDefinition = remove ? 1 : 0;

        // Give a chance to a sub-class to modify info
        fill(h, info);

        std::string compatibility;
        if (parametrisation.userParametrisation().get("compatibility", compatibility)) {
            const compat::GribCompatibility& c = compat::GribCompatibility::lookup(compatibility);
            c.execute(*this, parametrisation, h, info);
        }

        if (Log::debug()) {
            const std::streamsize p(Log::debug().precision(12));
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
            for (long j = 0; j < info.grid.pl_size && j < 4; j++) {
                X(info.grid.pl[j]);
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
            for (long j = 0; j < info.packing.extra_settings_count; j++) {
                X(info.packing.extra_settings[j].name);
                switch (info.packing.extra_settings[j].type) {
                    case CODES_TYPE_LONG:
                        X(info.packing.extra_settings[j].long_value);
                        break;
                    case CODES_TYPE_DOUBLE:
                        X(info.packing.extra_settings[j].double_value);
                        break;
                    case CODES_TYPE_STRING:
                        X(info.packing.extra_settings[j].string_value);
                        break;
                }
            }
            Log::debug().precision(p);
        }


        auto& values = field.values(i);
        int flags    = 0;
        int err      = 0;

        auto result = codes_grib_util_set_spec(h, &info.grid, &info.packing, flags, &values[0], values.size(), &err);
        HandleDeleter hf(result);  // Make sure handle deleted even in case of exception


        if (err == CODES_WRONG_GRID) {
            std::ostringstream oss;

            oss << "CODES_WRONG_GRID: ";

            Y(info.grid.grid_type);
            Y(info.grid.Ni);
            Y(info.grid.Nj);
            Y(info.grid.iDirectionIncrementInDegrees);
            Y(info.grid.jDirectionIncrementInDegrees);
            Y(info.grid.longitudeOfFirstGridPointInDegrees);
            Y(info.grid.longitudeOfLastGridPointInDegrees);
            Y(info.grid.latitudeOfFirstGridPointInDegrees);
            Y(info.grid.latitudeOfLastGridPointInDegrees);

            throw exception::SeriousBug(oss.str());
        }

        GRIB_CALL(err);

        const void* message;
        size_t size;
        GRIB_CALL(codes_get_message(result, &message, &size));

        GRIB_CALL(codes_check_message_header(message, size, PRODUCT_GRIB));
        GRIB_CALL(codes_check_message_footer(message, size, PRODUCT_GRIB));

        {  // Remove
            auto timing(ctx.statistics().saveTimer());
            out(message, size, true);
        }

        total += size;


        static bool checkArea = eckit::Resource<bool>("$MIR_CHECK_AREA", false);
        if (checkArea) {
            std::vector<double> v;
            if (parametrisation.userParametrisation().get("area", v) && v.size() == 4) {

                util::BoundingBox user(v[0], v[1], v[2], v[3]);

                util::BoundingBox before(
                    info.grid.latitudeOfFirstGridPointInDegrees, info.grid.longitudeOfFirstGridPointInDegrees,
                    info.grid.latitudeOfLastGridPointInDegrees, info.grid.longitudeOfLastGridPointInDegrees);

                // input::GribMemoryInput g(message, size);
                // util::BoundingBox after(g);

                if (user != before /*|| user != after || before != after*/) {
                    Log::info() << "MIR_CHECK_AREA:"
                                << " request=" << user << " result="
                                << before
                                // << " grib=" << after
                                << std::endl;
                }
            }
        }
    }

    ctx.statistics().gribEncodingTiming() -= saveTimer;

    return total;
}


size_t GribOutput::set(const param::MIRParametrisation& param, context::Context& ctx) {
    trace::ResourceUsage usage("GribOutput::set");

    interpolated_++;

    const auto& field = ctx.field();
    const auto& input = ctx.input();

    field.validate();

    size_t total = 0;

    util::MIRStatistics::Timing saveTimer;
    auto timer(ctx.statistics().gribEncodingTimer());

    ASSERT(field.dimensions() == 1);

    for (size_t i = 0; i < field.dimensions(); i++) {

        // Protect ecCodes and set error callback handling (throws)
        std::lock_guard<std::mutex> lock(local_mutex);
        codes_set_codes_assertion_failed_proc(&eccodes_assertion);

        // Make sure handle deleted even in case of exception
        auto h = codes_handle_clone(input.gribHandle(field.handle(i)));
        HandleDeleter hf(h);


        // set new handle key/values
        std::string packing;
        if (param.userParametrisation().get("packing", packing)) {
            std::unique_ptr<packing::Packer> packer(
                packing::PackerFactory::build(packing, param.userParametrisation(), param.fieldParametrisation()));
            auto type = packer->type(field.representation());
            auto len  = type.length();
            if (len > 0) {
                GRIB_CALL(codes_set_string(h, "packingType", type.c_str(), &len));
            }
        }

        long bitsPerValue = 0;
        if (param.userParametrisation().get("accuracy", bitsPerValue)) {
            GRIB_CALL(codes_set_long(h, "bitsPerValue", bitsPerValue));
        }

        long edition = 0;
        if (param.userParametrisation().get("edition", edition)) {
            GRIB_CALL(codes_set_long(h, "edition", edition));
        }


        // set values
        GRIB_CALL(codes_set_double(h, "missingValue", field.missingValue()));
        GRIB_CALL(codes_set_long(h, "bitmapPresent", field.hasMissing()));
        GRIB_CALL(codes_set_double_array(h, "values", field.values(i).data(), field.values(i).size()));


        const void* message;
        size_t size;
        GRIB_CALL(codes_get_message(h, &message, &size));

        GRIB_CALL(codes_check_message_header(message, size, PRODUCT_GRIB));
        GRIB_CALL(codes_check_message_footer(message, size, PRODUCT_GRIB));

        {  // Remove
            auto timing(ctx.statistics().saveTimer());
            out(message, size, true);
        }

        total += size;
    }

    ctx.statistics().gribEncodingTiming() -= saveTimer;

    return total;
}


void GribOutput::fill(grib_handle*, grib_info&) const {}


#undef Y
#undef X


}  // namespace output
}  // namespace mir
