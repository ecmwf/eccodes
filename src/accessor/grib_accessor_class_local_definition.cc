
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
#include "grib_accessor_class_local_definition.h"

grib_accessor_class_local_definition_t _grib_accessor_class_local_definition{"local_definition"};
grib_accessor_class* grib_accessor_class_local_definition = &_grib_accessor_class_local_definition;


void grib_accessor_class_local_definition_t::init(grib_accessor* a, const long l, grib_arguments* c){
    grib_accessor_local_definition_t* self = (grib_accessor_local_definition_t*)a;
    grib_handle* hand                    = grib_handle_of_accessor(a);
    int n                                = 0;

    self->grib2LocalSectionNumber                 = grib_arguments_get_name(hand, c, n++);
    self->productDefinitionTemplateNumber         = grib_arguments_get_name(hand, c, n++);
    self->productDefinitionTemplateNumberInternal = grib_arguments_get_name(hand, c, n++);
    self->type                                    = grib_arguments_get_name(hand, c, n++);
    self->stream                                  = grib_arguments_get_name(hand, c, n++);
    self->the_class                               = grib_arguments_get_name(hand, c, n++);
    self->eps                                     = grib_arguments_get_name(hand, c, n++);
    self->stepType                                = grib_arguments_get_name(hand, c, n++);
    self->derivedForecast                         = grib_arguments_get_name(hand, c, n++);
}

int grib_accessor_class_local_definition_t::unpack_long(grib_accessor* a, long* val, size_t* len){
    grib_accessor_local_definition_t* self = (grib_accessor_local_definition_t*)a;

    return grib_get_long(grib_handle_of_accessor(a), self->grib2LocalSectionNumber, val);
}

int grib_accessor_class_local_definition_t::pack_long(grib_accessor* a, const long* val, size_t* len){
    grib_accessor_local_definition_t* self         = (grib_accessor_local_definition_t*)a;
    grib_handle* hand                            = grib_handle_of_accessor(a);
    long productDefinitionTemplateNumber         = -1;
    long productDefinitionTemplateNumberInternal = -1;
    long productDefinitionTemplateNumberNew      = -1;
    long grib2LocalSectionNumber                 = -1;
    long type                                    = -1;
    long stream                                  = -1;
    long the_class                               = -1;
    long eps                                     = -1;
    long chemical                                = -1;
    long aerosol                                 = -1;
    long chemical_distfn                         = -1;
    long chemical_srcsink                        = -1;
    long aerosol_optical                         = -1;
    char stepType[15]                            = {0,};
    size_t slen               = 15;
    int localDefinitionNumber = *val;
    int isInstant             = 0;
    int tooEarly              = 0;
    long derivedForecast      = -1;
    long editionNumber        = 0;

    if (grib_get_long(hand, "editionNumber", &editionNumber) == GRIB_SUCCESS) {
        Assert(editionNumber != 1);
    }

    if (grib_get_long(hand, self->productDefinitionTemplateNumber, &productDefinitionTemplateNumber) != GRIB_SUCCESS)
        tooEarly = 1;
    grib_get_long(hand, self->productDefinitionTemplateNumberInternal, &productDefinitionTemplateNumberInternal);
    grib_get_long(hand, self->type, &type);
    grib_get_long(hand, self->stream, &stream);
    grib_get_long(hand, self->the_class, &the_class);
    grib_get_long(hand, self->eps, &eps);
    grib_get_string(hand, self->stepType, stepType, &slen);
    if (!strcmp(stepType, "instant"))
        isInstant = 1;
    grib_get_long(hand, self->grib2LocalSectionNumber, &grib2LocalSectionNumber);
    grib_get_long(hand, "is_chemical", &chemical);
    grib_get_long(hand, "is_chemical_distfn", &chemical_distfn);
    grib_get_long(hand, "is_chemical_srcsink", &chemical_srcsink);
    grib_get_long(hand, "is_aerosol", &aerosol);
    grib_get_long(hand, "is_aerosol_optical", &aerosol_optical);
    if (chemical == 1 && aerosol == 1) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "Parameter cannot be both chemical and aerosol!");
        return GRIB_ENCODING_ERROR;
    }

    if (grib_is_defined(hand, "perturbationNumber")) {
        eps = 1;
    }
    //if (grib2_is_PDTN_EPS(productDefinitionTemplateNumber))
    //    eps = 1;

    switch (localDefinitionNumber) {
        case 0:
            productDefinitionTemplateNumberNew = productDefinitionTemplateNumber;
            break;

        case 300:
            grib_context_log(a->context, GRIB_LOG_ERROR,
                             "Invalid localDefinitionNumber %d. This local definition has been deprecated.",
                             localDefinitionNumber);
            return GRIB_ENCODING_ERROR;
        case 500:
            productDefinitionTemplateNumberNew = 0;
            break;

        case 1:  // MARS labelling
        case 36: // MARS labelling for long window 4Dvar system
        case 40: // MARS labeling with domain and model (for LAM)
        case 42: // LC-WFV: Wave forecast verification
            if (isInstant) {
                // type=em || type=es
                if (type == 17) {
                    productDefinitionTemplateNumberNew = 2;
                    derivedForecast                    = 0;
                }
                else if (type == 18) {
                    productDefinitionTemplateNumberNew = 2;
                    derivedForecast                    = 4;
                    // eps or enda or elda or ewla
                }
                else if (eps == 1 || stream == 1030 || stream == 1249 || stream == 1250) {
                    productDefinitionTemplateNumberNew = 1;
                }
                else {
                    productDefinitionTemplateNumberNew = 0;
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
                    // eps or enda or elda or ewla
                }
                else if (eps == 1 || stream == 1030 || stream == 1249 || stream == 1250) {
                    productDefinitionTemplateNumberNew = 11;
                }
                else {
                    productDefinitionTemplateNumberNew = 8;
                }
            }
            break;
        case 41: // EFAS: uses post-processing templates
            if (isInstant) {
                if (eps == 1)
                    productDefinitionTemplateNumberNew = 71;
                else
                    productDefinitionTemplateNumberNew = 70;
            }
            else {
                // non-instantaneous: accum etc
                if (eps == 1)
                    productDefinitionTemplateNumberNew = 73;
                else
                    productDefinitionTemplateNumberNew = 72;
            }
            break;

        case 15: // Seasonal forecast data
        case 16: // Seasonal forecast monthly mean data
        case 12: // Seasonal forecast monthly mean data for lagged systems
        case 18: // Multianalysis ensemble data
        case 26: // MARS labelling or ensemble forecast data
        case 30: // Forecasting Systems with Variable Resolution
            if (isInstant) {
                productDefinitionTemplateNumberNew = 1;
            }
            else {
                productDefinitionTemplateNumberNew = 11;
            }
            break;

        case 5:  // Forecast probability data
        case 7:  // Sensitivity data
        case 9:  // Singular vectors and ensemble perturbations
        case 11: // Supplementary data used by the analysis
        case 14: // Brightness temperature
        case 20: // 4D variational increments
        case 21: // Sensitive area predictions
        case 23: // Coupled atmospheric, wave and ocean means
        case 24: // Satellite Channel Number Data
        case 25:
        case 28:  // COSMO local area EPS
        case 38:  // 4D variational increments for long window 4Dvar system
        case 39:  // 4DVar model errors for long window 4Dvar system
        case 60:  // Ocean data analysis
        case 192: // Multiple ECMWF local definitions
            if (isInstant) {
                productDefinitionTemplateNumberNew = 0;
            }
            else {
                productDefinitionTemplateNumberNew = 8;
            }
            break;

        default:
#ifdef DEBUG
            // In test & development mode, fail so we remember to adjust PDTN
            grib_context_log(a->context, GRIB_LOG_ERROR,
                             "grib_accessor_local_definition_t: Invalid localDefinitionNumber %d", localDefinitionNumber);
            return GRIB_ENCODING_ERROR;
#endif
            // ECC-1253: Do not fail in operations. Leave PDTN as is
            productDefinitionTemplateNumberNew = productDefinitionTemplateNumber;
            break;
    }

    // Adjust for atmospheric chemical constituents
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
    // Adjust for atmospheric chemical constituents based on a distribution function
    if (chemical_distfn == 1) {
        if (eps == 1) {
            if (isInstant) {
                productDefinitionTemplateNumberNew = 58;
            }
            else {
                productDefinitionTemplateNumberNew = 68;
            }
        }
        else {
            if (isInstant) {
                productDefinitionTemplateNumberNew = 57;
            }
            else {
                productDefinitionTemplateNumberNew = 67;
            }
        }
    }

    // Adjust for atmospheric chemical constituents with source or sink
    if (chemical_srcsink == 1) {
        if (eps == 1) {
            if (isInstant) {
                productDefinitionTemplateNumberNew = 77;
            }
            else {
                productDefinitionTemplateNumberNew = 79;
            }
        }
        else {
            if (isInstant) {
                productDefinitionTemplateNumberNew = 76;
            }
            else {
                productDefinitionTemplateNumberNew = 78;
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
                //productDefinitionTemplateNumberNew = 47;  This PDT is deprecated
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
    // Adjust for optical properties of aerosol
    if (aerosol_optical == 1) {
        if (eps == 1) {
            if (isInstant) {
                productDefinitionTemplateNumberNew = 49;
            }
        }
        else {
            if (isInstant) {
                productDefinitionTemplateNumberNew = 48;
            }
        }
    }

    if (productDefinitionTemplateNumber != productDefinitionTemplateNumberNew) {
        if (a->context->debug) {
            fprintf(stderr, "ECCODES DEBUG grib_accessor_local_definition_t: ldNumber=%d, newPDTN=%ld\n",
                   localDefinitionNumber, productDefinitionTemplateNumberNew);
        }
        if (tooEarly)
            grib_set_long(hand, self->productDefinitionTemplateNumberInternal, productDefinitionTemplateNumberNew);
        else
            grib_set_long(hand, self->productDefinitionTemplateNumber, productDefinitionTemplateNumberNew);
    }
    if (derivedForecast >= 0)
        grib_set_long(hand, self->derivedForecast, derivedForecast);

    grib_set_long(hand, self->grib2LocalSectionNumber, *val);

    return 0;
}

int grib_accessor_class_local_definition_t::value_count(grib_accessor* a, long* count){
    *count = 1;
    return 0;
}
