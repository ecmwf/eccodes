/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#pragma once

#include "Gen.h"

namespace eccodes::accessor
{

// Read-only ISO 8601 date and time, e.g. "2026-11-21T11:30:00+00:00".
// GRIB times are UTC, so the offset is always "+00:00".
// The string is computed from the referenced keys on every read.
//
// Arguments:
//   iso_datetime(year, month, day, hour, minute, second)
//   iso_datetime(date, time [, second [, step, stepUnits]])
//       date=YYYYMMDD, time=hhmm. With step and stepUnits (validity time), the seconds of a
//       step in seconds are added: validityTime has only minutes.
// 24:00 is given as 00:00 of the next day.
class IsoDateTime : public Gen
{
public:
    IsoDateTime() :
        Gen() {}
    const AccessorType& accessor_type() const override { return accessor_type_; }
    long get_native_type() override { return GRIB_TYPE_STRING; }
    int unpack_string(char*, size_t* len) override;
    size_t string_length() override { return 25; }  // "YYYY-MM-DDThh:mm:ss+00:00"
    void init(const long, grib_arguments*) override;

private:
    int get_components(long* date, long* hour, long* minute, long* second, long* extra_seconds);

    // Either year_..second_ or date_, time_ [, second_ [, step_, stepUnits_]]
    const char* year_ = nullptr;
    const char* month_ = nullptr;
    const char* day_ = nullptr;
    const char* hour_ = nullptr;
    const char* minute_ = nullptr;
    const char* date_ = nullptr;
    const char* time_ = nullptr;
    const char* second_ = nullptr;
    const char* step_ = nullptr;
    const char* stepUnits_ = nullptr;

public:
    static constexpr AccessorType accessor_type_{ "iso_datetime" };
};

}  // namespace eccodes::accessor
