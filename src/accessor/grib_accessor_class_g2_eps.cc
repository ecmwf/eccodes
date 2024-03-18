
/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"
#include "grib_accessor_class_g2_eps.h"

grib_accessor_class_g2_eps_t _grib_accessor_class_g2_eps{"g2_eps"};
grib_accessor_class* grib_accessor_class_g2_eps = &_grib_accessor_class_g2_eps;


void grib_accessor_class_g2_eps_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_g2_eps_t* self = (grib_accessor_g2_eps_t*)a;
    int n                      = 0;

    self->productDefinitionTemplateNumber = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->type                            = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->stream                          = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->stepType                        = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
    self->derivedForecast                 = grib_arguments_get_name(grib_handle_of_accessor(a), c, n++);
}

int grib_accessor_class_g2_eps_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_g2_eps_t* self           = (grib_accessor_g2_eps_t*)a;
    long productDefinitionTemplateNumber = 0;
    int err = 0;
    grib_handle* hand = grib_handle_of_accessor(a);

    err = grib_get_long(hand, self->productDefinitionTemplateNumber, &productDefinitionTemplateNumber);
    if (err) return err;

    *val = 0;
    if (grib_is_defined(hand, "perturbationNumber")) {
        *val = 1;
    }
    //if (grib2_is_PDTN_EPS(productDefinitionTemplateNumber))
    //    *val = 1;

    return GRIB_SUCCESS;
}

int grib_accessor_class_g2_eps_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_g2_eps_t* self              = (grib_accessor_g2_eps_t*)a;
    grib_handle* hand                       = grib_handle_of_accessor(a);
    long productDefinitionTemplateNumber    = -1;
    long productDefinitionTemplateNumberNew = -1;
    long type                               = -1;
    long stream                             = -1;
    long chemical                           = -1;
    long aerosol                            = -1;
    char stepType[15]                       = {0,};
    size_t slen          = 15;
    int eps              = *val;
    int isInstant        = 0;
    long derivedForecast = -1;

    if (grib_get_long(hand, self->productDefinitionTemplateNumber, &productDefinitionTemplateNumber) != GRIB_SUCCESS)
        return GRIB_SUCCESS;

    grib_get_long(hand, self->type, &type);
    grib_get_long(hand, self->stream, &stream);
    grib_get_string(hand, self->stepType, stepType, &slen);
    if (!strcmp(stepType, "instant"))
        isInstant = 1;
    grib_get_long(hand, "is_chemical", &chemical);
    grib_get_long(hand, "is_aerosol", &aerosol);
    if (chemical == 1 && aerosol == 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Parameter cannot be both chemical and aerosol!");
        return GRIB_ENCODING_ERROR;
    }

    // eps or stream=(enda or elda or ewla)
    if (eps || stream == 1030 || stream == 1249 || stream == 1250) {
        if (isInstant) {
            // type=em || type=es
            if (type == 17) {
                productDefinitionTemplateNumberNew = 2;
                derivedForecast                    = 0;
            }
            else if (type == 18) {
                productDefinitionTemplateNumberNew = 2;
                derivedForecast                    = 4;
            }
            else {
                productDefinitionTemplateNumberNew = 1;
            }
        }
        else {
            // type=em || type=es
            if (type == 17) {
                productDefinitionTemplateNumberNew = 12;
                derivedForecast                    = 0;
            }
            else if (type == 18) {
                productDefinitionTemplateNumberNew = 12;
                derivedForecast                    = 4;
            }
            else {
                productDefinitionTemplateNumberNew = 11;
            }
        }
    }
    else {
        if (isInstant) {
            productDefinitionTemplateNumberNew = 0;
        }
        else {
            productDefinitionTemplateNumberNew = 8;
        }
    }

    // Adjust for chemical species
    if (chemical == 1) {
        if (eps == 1) {
            if (isInstant) {
                productDefinitionTemplateNumberNew = 41;
            }
            else {
                productDefinitionTemplateNumberNew = 43;
            }
        }
        else {
            if (isInstant) {
                productDefinitionTemplateNumberNew = 40;
            }
            else {
                productDefinitionTemplateNumberNew = 42;
            }
        }
    }

    // Adjust for aerosols
    if (aerosol == 1) {
        if (eps == 1) {
            if (isInstant) {
                productDefinitionTemplateNumberNew = 45;
            }
            else {
                //productDefinitionTemplateNumberNew = 47;   This PDT is deprecated
                productDefinitionTemplateNumberNew = 85;
            }
        }
        else {
            if (isInstant) {
                productDefinitionTemplateNumberNew = 48; //44 is deprecated*/
            }
            else {
                productDefinitionTemplateNumberNew = 46;
            }
        }
    }

    if (productDefinitionTemplateNumber != productDefinitionTemplateNumberNew) {
        grib_set_long(hand, self->productDefinitionTemplateNumber, productDefinitionTemplateNumberNew);
        if (derivedForecast >= 0)
            grib_set_long(hand, self->derivedForecast, derivedForecast);
    }

    return 0;
}

int grib_accessor_class_g2_eps_t::value_count(grib_accessor* a, long* count){
    *count = 1;
    return 0;
}
