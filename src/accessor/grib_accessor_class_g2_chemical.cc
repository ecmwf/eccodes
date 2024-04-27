
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_accessor_class_g2_chemical.h"

grib_accessor_class_g2_chemical_t _grib_accessor_class_g2_chemical{"g2_chemical"};
grib_accessor_class* grib_accessor_class_g2_chemical = &_grib_accessor_class_g2_chemical;


void grib_accessor_class_g2_chemical_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_class_unsigned_t::init(a, l, c);
    grib_accessor_g2_chemical_t* self = (grib_accessor_g2_chemical_t*)a;
    grib_handle* hand               = grib_handle_of_accessor(a);
    int n                           = 0;

    self->productDefinitionTemplateNumber = grib_arguments_get_name(hand, c, n++);
    self->stepType                        = grib_arguments_get_name(hand, c, n++);
    self->chemical_type                    = grib_arguments_get_long(hand, c, n++);
}

int grib_accessor_class_g2_chemical_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_g2_chemical_t* self      = (grib_accessor_g2_chemical_t*)a;
    long productDefinitionTemplateNumber = 0;
    grib_get_long(grib_handle_of_accessor(a), self->productDefinitionTemplateNumber, &productDefinitionTemplateNumber);

    /*
     * 0 = atmospheric chemical constituents
     * 1 = atmospheric chemical constituents based on a distribution function
     * 2 = atmospheric chemical constituents with source or sink
     */
    Assert(self->chemical_type == 0 || self->chemical_type == 1 || self->chemical_type == 2);
    if (self->chemical_type == 1)
        *val = grib2_is_PDTN_ChemicalDistFunc(productDefinitionTemplateNumber);
    else if (self->chemical_type == 2)
        *val = grib2_is_PDTN_ChemicalSourceSink(productDefinitionTemplateNumber);
    else
        *val = grib2_is_PDTN_Chemical(productDefinitionTemplateNumber);

    return GRIB_SUCCESS;
}

int grib_accessor_class_g2_chemical_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_g2_chemical_t* self         = (grib_accessor_g2_chemical_t*)a;
    grib_handle* hand                       = grib_handle_of_accessor(a);
    long productDefinitionTemplateNumber    = -1;
    long productDefinitionTemplateNumberNew = -1;
    /*long type=-1;
    long stream=-1;*/
    long eps          = -1;
    char stepType[15] = {0,};
    size_t slen = 15;
    /*int chemical = *val;*/
    int isInstant = 0;
    /*long derivedForecast=-1;*/
    int ret = 0;

    if (grib_get_long(hand, self->productDefinitionTemplateNumber, &productDefinitionTemplateNumber) != GRIB_SUCCESS)
        return GRIB_SUCCESS;

    /*
     grib_get_long(hand, self->type,&type);
     grib_get_long(hand, self->stream,&stream);
     */
    ret = grib_get_string(hand, self->stepType, stepType, &slen);
    Assert(ret == GRIB_SUCCESS);

    eps = grib_is_defined(hand, "perturbationNumber");
    //eps = grib2_is_PDTN_EPS(productDefinitionTemplateNumber);

    if (!strcmp(stepType, "instant"))
        isInstant = 1;

    /*
     * 0 = atmospheric chemical constituents
     * 1 = atmospheric chemical constituents based on a distribution function
     * 2 = atmospheric chemical constituents with source or sink
     */
    Assert(self->chemical_type == 0 || self->chemical_type == 1 || self->chemical_type == 2);

    if (eps == 1) {
        if (isInstant) {
            if      (self->chemical_type == 0) productDefinitionTemplateNumberNew=41;
            else if (self->chemical_type == 1) productDefinitionTemplateNumberNew=58;
            else if (self->chemical_type == 2) productDefinitionTemplateNumberNew=77;
        }
        else {
            if      (self->chemical_type == 0) productDefinitionTemplateNumberNew=43;
            else if (self->chemical_type == 1) productDefinitionTemplateNumberNew=68;
            else if (self->chemical_type == 2) productDefinitionTemplateNumberNew=79;
        }
    }
    else {
        /* deterministic */
        if (isInstant) {
            if      (self->chemical_type == 0) productDefinitionTemplateNumberNew=40;
            else if (self->chemical_type == 1) productDefinitionTemplateNumberNew=57;
            else if (self->chemical_type == 2) productDefinitionTemplateNumberNew=76;
        }
        else {
            if      (self->chemical_type == 0) productDefinitionTemplateNumberNew=42;
            else if (self->chemical_type == 1) productDefinitionTemplateNumberNew=67;
            else if (self->chemical_type == 2) productDefinitionTemplateNumberNew=78;
        }
    }

    if (productDefinitionTemplateNumber != productDefinitionTemplateNumberNew) {
        grib_set_long(hand, self->productDefinitionTemplateNumber, productDefinitionTemplateNumberNew);
        /*if (derivedForecast>=0) grib_set_long(hand, self->derivedForecast,derivedForecast);*/
    }

    return 0;
}

int grib_accessor_class_g2_chemical_t::value_count(grib_accessor* a, long* count){
    *count = 1;
    return 0;
}
