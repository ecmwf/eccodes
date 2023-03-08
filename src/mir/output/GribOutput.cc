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

#include <ostream>
#include <sstream>

#include "eckit/config/Resource.h"

#include "mir/action/context/Context.h"
#include "mir/action/io/Copy.h"
#include "mir/action/io/Save.h"
#include "mir/action/io/Set.h"
#include "mir/action/plan/ActionPlan.h"
#include "mir/api/MIREstimation.h"
#include "mir/compat/GribCompatibility.h"
#include "mir/data/MIRField.h"
#include "mir/grib/BasicAngle.h"
#include "mir/grib/Packing.h"
#include "mir/input/MIRInput.h"
#include "mir/key/Area.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Gridded.h"
#include "mir/repres/Representation.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/Grib.h"
#include "mir/util/Log.h"
#include "mir/util/MIRStatistics.h"
#include "mir/util/Mutex.h"
#include "mir/util/Trace.h"
#include "mir/util/Types.h"


namespace mir::output {


static util::recursive_mutex local_mutex;


#define X(a) Log::debug() << "  GRIB encoding: " << #a << " = " << (a) << std::endl
#define Y(a) oss << " " << #a << "=" << a


[[noreturn]] void eccodes_assertion(const char* message) {
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


size_t GribOutput::copy(const param::MIRParametrisation& /*unused*/, context::Context& ctx) {
    saved_++;

    const auto& input = ctx.input();

    size_t total = 0;
    for (size_t i = 0; i < input.dimensions(); i++) {
        auto* h = input.gribHandle(i);  // Base class throws if input cannot provide handle
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
    if (param.get(packing, packing)) {
        estimator.packing(packing);
        // const grib::Packing &packer = grib::Packing::lookup(packing);
        // packer.estimate(estimator, *field.representation());
    }

    long edition;
    if (param.get("edition", edition)) {
        estimator.edition(size_t(edition));
    }
}


bool GribOutput::printParametrisation(std::ostream& out, const param::MIRParametrisation& param) const {
    std::unique_ptr<grib::Packing> pack(grib::Packing::build(param));
    ASSERT(pack);

    bool ok = pack->printParametrisation(out);

    std::string compatibility;
    if (param.userParametrisation().get("compatibility", compatibility)) {
        out << (ok ? "," : "") << "compatibility=" << compatibility;
        ok = true;

        const auto& c = compat::GribCompatibility::lookup(compatibility);
        c.printParametrisation(out, param);
    }

    return ok;
}


void GribOutput::prepare(const param::MIRParametrisation& param, action::ActionPlan& plan, MIROutput& output) {
    if (plan.ended()) {
        return;
    }

    auto compatibility_empty = [&param]() {
        std::string compatibility;
        param.userParametrisation().get("compatibility", compatibility);
        return compatibility.empty();
    };

    auto packing_empty = [&param]() {
        std::unique_ptr<grib::Packing> pack(grib::Packing::build(param));
        ASSERT(pack);
        return pack->empty();
    };

    plan.add(!plan.empty()            ? new action::io::Save(param, output)
             : !compatibility_empty() ? new action::io::Save(param, output)
             : !packing_empty()       ? new action::io::Set(param, output)
                                      : static_cast<action::Action*>(new action::io::Copy(param, output)));
}


bool GribOutput::sameParametrisation(const param::MIRParametrisation& param1,
                                     const param::MIRParametrisation& param2) const {
    std::unique_ptr<grib::Packing> packing1(grib::Packing::build(param1));
    std::unique_ptr<grib::Packing> packing2(grib::Packing::build(param2));

    if (!packing1->sameAs(packing2.get())) {
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
        const auto& c = compat::GribCompatibility::lookup(compatibility1);
        if (!c.sameParametrisation(param1, param2)) {
            return false;
        }
    }

    return true;
}


size_t GribOutput::save(const param::MIRParametrisation& param, context::Context& ctx) {
    trace::ResourceUsage usage("GribOutput::save");

    interpolated_++;

    const auto& field = ctx.field();
    const auto& input = ctx.input();

    field.validate();

    size_t total = 0;

    util::MIRStatistics::Timing saveTimer;
    auto timer(ctx.statistics().gribEncodingTimer());

    std::unique_ptr<grib::Packing> pack(grib::Packing::build(param));
    ASSERT(pack);

    for (size_t i = 0; i < field.dimensions(); i++) {

        // Protect ecCodes and set error callback handling (throws)
        util::lock_guard<util::recursive_mutex> lock(local_mutex);
        codes_set_codes_assertion_failed_proc(&eccodes_assertion);

        // Special case where only values are changing; handle is cloned, and new values are set
        if (param.userParametrisation().has("filter")) {

            // Make sure handle deleted even in case of exception
            auto* h = codes_handle_clone(input.gribHandle(i));
            HandleDeleter hf(h);

            long n;
            GRIB_CALL(codes_get_long(h, "numberOfDataPoints", &n));
            if (size_t(n) != field.values(i).size()) {
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

        auto* h = input.gribHandle(field.handle(i));  // Base class throws if input cannot provide handle

        grib_info info;

        // missing values
        if (field.hasMissing() && field.missingValue() == 0.) {
            info.grid.bitmapPresent = 1L;
            info.extra_set("missingValue", 0.);  // ecCodes workaround for missingValue==0
        }
        else {
            info.grid.bitmapPresent = field.hasMissing() ? 1L : 0L;
            info.grid.missingValue  = field.missingValue();
        }

        // Ask representation to update info
        repres::RepresentationHandle repres(field.representation());
        repres->fillGrib(info);

        // Packing, accuracy, edition
        pack->fill(repres, info);

        // Basic angle (after representation), support only for gridType=regular_ll
        std::string basicAngle = "decimal";
        param.get("basic-angle", basicAngle);

        if (basicAngle == "as-input") {
            ASSERT(info.grid.grid_type == CODES_UTIL_GRID_SPEC_REGULAR_LL);

            std::vector<long> fraction(2);
            GRIB_CALL(codes_get_long(h, "basicAngleOfTheInitialProductionDomain", &fraction[0]));
            GRIB_CALL(codes_get_long(h, "subdivisionsOfBasicAngle", &fraction[1]));

            grib::BasicAngle basic(fraction[0], fraction[1]);
            basic.fillGrib(info);
        }
        else if (basicAngle == "fraction") {
            ASSERT(info.grid.grid_type == CODES_UTIL_GRID_SPEC_REGULAR_LL);

            grib::BasicAngle basic(info);
            basic.fillGrib(info);
        }
        else {
            // codes_grib_util_set_spec does not need anything here (GRIB standard)
            ASSERT(basicAngle == "decimal");
        }

        // Extra settings (paramId comes from here)
        for (const auto& k : field.metadata(i)) {
            info.extra_set(k.first.c_str(), k.second);
        }


        bool remove = false;
        param.get("delete-local-definition", remove);
        info.packing.deleteLocalDefinition = remove ? 1 : 0;

        // Give a chance to a sub-class to modify info
        fill(h, info);

        std::string compatibility;
        if (param.userParametrisation().get("compatibility", compatibility)) {
            const auto& c = compat::GribCompatibility::lookup(compatibility);
            c.execute(*this, param, h, info);
        }

        if (Log::debug_active()) {
            auto p(Log::debug().precision(12));
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


        const auto& values = field.values(i);
        int flags          = 0;
        int err            = 0;

        auto* result =
            codes_grib_util_set_spec(h, &info.grid, &info.packing, flags, values.data(), values.size(), &err);
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
            util::BoundingBox user;
            if (key::Area::get(param.userParametrisation(), user)) {

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

    std::unique_ptr<grib::Packing> pack(grib::Packing::build(param));
    ASSERT(pack);

    ASSERT(field.dimensions() == 1);

    for (size_t i = 0; i < field.dimensions(); i++) {

        // Protect ecCodes and set error callback handling (throws)
        util::lock_guard<util::recursive_mutex> lock(local_mutex);
        codes_set_codes_assertion_failed_proc(&eccodes_assertion);

        // Make sure handle deleted even in case of exception
        auto* h = codes_handle_clone(input.gribHandle(field.handle(i)));
        HandleDeleter hf(h);

        repres::RepresentationHandle repres(field.representation());

        // Packing, accuracy, edition
        pack->set(repres, h);

        // Values
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


void GribOutput::fill(grib_handle* /*unused*/, grib_info& /*unused*/) const {}


#undef Y
#undef X


}  // namespace mir::output
