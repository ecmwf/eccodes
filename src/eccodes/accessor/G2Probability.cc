/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "G2Probability.h"

eccodes::accessor::G2Probability _grib_accessor_g2_probability;
eccodes::Accessor* grib_accessor_g2_probability = &_grib_accessor_g2_probability;

namespace eccodes::accessor
{

void G2Probability::init(const long l, grib_arguments* c)
{
    Unsigned::init(l, c);
    grib_handle* hand = get_enclosing_handle();
    int n = 0;

    productDefinitionTemplateNumber_ = c->get_name(hand, n++);
    stepType_ = c->get_name(hand, n++);
}

int G2Probability::unpack_long(long* val, size_t* len)
{
    long productDefinitionTemplateNumber = 0;
    grib_get_long(get_enclosing_handle(), productDefinitionTemplateNumber_, &productDefinitionTemplateNumber);
    *val = (productDefinitionTemplateNumber == 5 || productDefinitionTemplateNumber == 9);

    return GRIB_SUCCESS;
}

int G2Probability::pack_long(const long* val, size_t* len)
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

    // long eps = grib_is_defined(hand, "perturbationNumber");

    if (!strcmp(stepType, "instant"))
        isInstant = 1;

    if (isInstant) {
        productDefinitionTemplateNumberNew = 5;
    }
    else {
        productDefinitionTemplateNumberNew = 9;
    }

    if (productDefinitionTemplateNumber != productDefinitionTemplateNumberNew) {
        grib_set_long(hand, productDefinitionTemplateNumber_, productDefinitionTemplateNumberNew);
    }

    return GRIB_SUCCESS;
}

int G2Probability::value_count(long* count)
{
    *count = 1;
    return GRIB_SUCCESS;
}

}  // namespace eccodes::accessor
