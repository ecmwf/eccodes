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


#pragma once

#include <eccodes.h>

#include <algorithm>
#include <cstring>
#include <ios>
#include <sstream>
#include <string>
#include <utility>
#include <vector>


namespace eccodes::geo::util
{


bool grib_call(int e, const char* call, bool NOT_FOUND_IS_OK = false)
{
    if (static_cast<bool>(e)) {
        if (NOT_FOUND_IS_OK && (e == CODES_NOT_FOUND)) {
            return false;
        }

        std::ostringstream os;
        os << call << ": " << codes_get_error_message(e);
        throw mir::exception::SeriousBug(os.str());
    }
    return true;
}


#define GRIB_CALL(a) grib_call(a, #a)


struct grib_info
{
    grib_info();
    grib_info(const grib_info&) = delete;
    grib_info(grib_info&&)      = delete;

    ~grib_info() = default;

    void operator=(grib_info&&)      = delete;
    void operator=(const grib_info&) = delete;

    void extra_set(const char* key, long);
    void extra_set(const char* key, double);
    void extra_set(const char* key, const char*);

    codes_util_grid_spec grid;
    codes_util_packing_spec packing;

private:
    std::vector<std::string> strings_;
    const size_t extra_settings_size_;
};


grib_info::grib_info() :
    grid{}, packing{}, extra_settings_size_(sizeof(packing.extra_settings) / sizeof(packing.extra_settings[0]))
{
    // NOTE low-level initialisation only necessary for C interface
    std::memset(&grid, 0, sizeof(grid));
    std::memset(&packing, 0, sizeof(packing));

    strings_.reserve(extra_settings_size_);
}


void grib_info::extra_set(const char* key, long value)
{
    auto j = static_cast<size_t>(packing.extra_settings_count++);
    ASSERT(j < extra_settings_size_);

    auto& set      = packing.extra_settings[j];
    set.name       = key;
    set.type       = CODES_TYPE_LONG;
    set.long_value = value;
}


void grib_info::extra_set(const char* key, double value)
{
    auto j = static_cast<size_t>(packing.extra_settings_count++);
    ASSERT(j < extra_settings_size_);

    auto& set        = packing.extra_settings[j];
    set.name         = key;
    set.type         = CODES_TYPE_DOUBLE;
    set.double_value = value;
}


void grib_info::extra_set(const char* key, const char* value)
{
    auto j = static_cast<size_t>(packing.extra_settings_count++);
    ASSERT(j < extra_settings_size_);

    auto& set = packing.extra_settings[j];
    set.name  = key;
    set.type  = CODES_TYPE_STRING;

    strings_.emplace_back(value);
    set.string_value = strings_.back().c_str();
}


size_t GribOutput::save(const param::MIRParametrisation& param, context::Context& ctx)
{
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

        // Basic angle (after representation), support only for gridType=regular_ll
        std::string basicAngle = "decimal";
        param.get("basic-angle", basicAngle);

        if (basicAngle == "as-input") {
            std::vector<long> fraction(2);
            GRIB_CALL(codes_get_long(h, "basicAngleOfTheInitialProductionDomain", &fraction[0]));
            GRIB_CALL(codes_get_long(h, "subdivisionsOfBasicAngle", &fraction[1]));

            grib::BasicAngle basic(fraction[0], fraction[1]);
            basic.fillGrib(info);
        }
        else if (basicAngle == "fraction") {
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

        const auto& values = field.values(i);
        int flags          = 0;
        int err            = 0;

        auto* result =
            codes_grib_util_set_spec(h, &info.grid, &info.packing, flags, values.data(), values.size(), &err);
        HandleDeleter hf(result);  // Make sure handle deleted even in case of exception


        GRIB_CALL(err);  // err == CODES_WRONG_GRID

        const void* message = nullptr;
        size_t size         = 0;
        GRIB_CALL(codes_get_message(result, &message, &size));

        GRIB_CALL(codes_check_message_header(message, size, PRODUCT_GRIB));
        GRIB_CALL(codes_check_message_footer(message, size, PRODUCT_GRIB));

        {  // Remove
            auto timing(ctx.statistics().saveTimer());
            out(message, size, true);
        }

        total += size;
        return total;
}


#undef GRIB_CALL


}  // namespace mir::output
