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

#include "grib_accessor_class_ascii.h"

class grib_accessor_getenv_t : public grib_accessor_ascii_t
{
public:
    grib_accessor_getenv_t() :
        grib_accessor_ascii_t() {}
    static inline const AccessorType accessor_type{"getenv"};
    const AccessorType& getClassName() const override { return accessor_type; }
    int pack_string(const char*, size_t* len) override;
    int unpack_string(char*, size_t* len) override;
    size_t string_length() override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

private:
    // TODO(maee): shadows name_?
    const char* name_;
    char* value_;
    const char* default_value_;
};
