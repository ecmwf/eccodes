/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "G2WavePeriodRange.h"

// eccodes::accessor::G2WavePeriodRange _grib_accessor_g2_wave_period_range;
// eccodes::Accessor* grib_accessor_g2_wave_period_range = &_grib_accessor_g2_wave_period_range;
eccodes::AccessorBuilder<eccodes::accessor::G2WavePeriodRange> _grib_accessor_g2_wave_period_range{};

namespace eccodes::accessor
{

void G2WavePeriodRange::init(const long l, grib_arguments* c)
{
    Unsigned::init(l, c);
    grib_handle* hand = get_enclosing_handle();
    int n = 0;

    productDefinitionTemplateNumber_ = c->get_name(hand, n++);
    stepType_ = c->get_name(hand, n++);
}

int G2WavePeriodRange::unpack_long(long* val, size_t* len)
{
    long productDefinitionTemplateNumber = 0;
    grib_get_long(get_enclosing_handle(), productDefinitionTemplateNumber_, &productDefinitionTemplateNumber);
    *val = (productDefinitionTemplateNumber == 103 || // instant det
            productDefinitionTemplateNumber == 104 || // instant ens
            productDefinitionTemplateNumber == 139 || // instant hindcast
            productDefinitionTemplateNumber == 140 || // instant hindcast ens
            productDefinitionTemplateNumber == 144 || // interval det
            productDefinitionTemplateNumber == 145);  // interval ens

    return GRIB_SUCCESS;
}

int G2WavePeriodRange::pack_long(const long* val, size_t* len)
{
    grib_handle* hand = get_enclosing_handle();
    long productDefinitionTemplateNumber    = -1;
    long productDefinitionTemplateNumberNew = -1;
    char stepType[15] = {0,};
    size_t slen = 15;
    int isInstant = 0;

    if (grib_get_long(hand, productDefinitionTemplateNumber_, &productDefinitionTemplateNumber) != GRIB_SUCCESS)
        return GRIB_SUCCESS;

    int err = grib_get_string(hand, stepType_, stepType, &slen);
    if (err) return err;

    const int ensemble = grib_is_defined(hand, "perturbationNumber");
    long hindcastValue = -1;
    err = grib_get_long(hand, "isHindcast", &hindcastValue);
    const int hindcast = (!err && hindcastValue == 1);

    if (!strcmp(stepType, "instant"))
        isInstant = 1;

    if (ensemble == 0) { // deterministic
        if (isInstant) {
            productDefinitionTemplateNumberNew = 103;
            if (hindcast) productDefinitionTemplateNumberNew = 139;
        } else {
            productDefinitionTemplateNumberNew = 144;
        }
    } else {
        // ensemble
        if (isInstant) {
            productDefinitionTemplateNumberNew = 104;
            if (hindcast) productDefinitionTemplateNumberNew = 140;
        } else {
            productDefinitionTemplateNumberNew = 145;
        }
    }

    if (productDefinitionTemplateNumber != productDefinitionTemplateNumberNew) {
        grib_set_long(hand, productDefinitionTemplateNumber_, productDefinitionTemplateNumberNew);
    }

    return GRIB_SUCCESS;
}

int G2WavePeriodRange::value_count(long* count)
{
    *count = 1;
    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
