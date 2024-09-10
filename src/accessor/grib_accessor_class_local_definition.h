
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

#include "grib_accessor_class_unsigned.h"

class grib_accessor_local_definition_t : public grib_accessor_unsigned_t
{
public:
    grib_accessor_local_definition_t() :
        grib_accessor_unsigned_t() {}
    static inline const AccessorType accessor_type{"local_definition"};
    const AccessorType& getClassName() const override { return accessor_type; }
    int pack_long(const long* val, size_t* len) override;
    int unpack_long(long* val, size_t* len) override;
    int value_count(long*) override;
    void init(const long, grib_arguments*) override;

private:
    const char* productDefinitionTemplateNumber_;
    const char* productDefinitionTemplateNumberInternal_;
    const char* grib2LocalSectionNumber_;
    const char* type_;
    const char* stream_;
    const char* the_class_;
    const char* eps_;
    const char* stepType_;
    const char* derivedForecast_;
};
