/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * Check GRIB2 parameter concept file e.g. shortName.def, paramId.def
 */

#include "grib_api_internal.h"

typedef struct grib_expression_long {
    grib_expression base;
    long value;
} grib_expression_long;

typedef struct grib_expression_functor {
    grib_expression base;
    char* name;
    grib_arguments* args;
} grib_expression_functor;

typedef struct grib_expression_string {
    grib_expression base;
    char* value;
} grib_expression_string;

static int type_of_surface_missing(const char* name, const char* value)
{
    /* Surface Type is Code Table 4.5 in which 255 is the same as missing */
    if (strcmp(value, "255") == 0) {
        return 1;
    }
    /* Beware of problems where we put 'missing()' for a code table key! */
    if (strncmp(value, "missing", 7) == 0) {
        fprintf(stderr, "Invalid value for %s Code Table entry: '%s'\n", name, value);
        Assert(0);
    }
    return 0;
}
static int scale_factor_missing(const char* value)
{
    /* Scale factor is one octet so 255 is the same as missing */
    if (strcmp(value, "missing") == 0 || strcmp(value, "255") == 0) {
        return 1;
    }
    return 0;
}

/*
 * key      = paramId or shortName
 * filename = paramId.def or shortName.def
*/
static int grib_check_param_concepts(const char* key, const char* filename)
{
    int isLocal = 0;
    grib_concept_value* concept_value = grib_parse_concept_file(NULL, filename);
    if (!concept_value)
        return GRIB_IO_PROBLEM;

    if (strstr(filename, "/localConcepts/")) {
        isLocal = 1;
    }
    while (concept_value) {
        grib_concept_condition* concept_condition = concept_value->conditions;
        /* Convention:
         *  -1 key not present
         *  0  key present and not missing
         *  1  key present and missing
         */
        int scaleFactor1Missing = -1, scaleFactor2Missing = -1;
        int scaledValue1Missing = -1, scaledValue2Missing = -1;
        int type1Missing = -1, type2Missing = -1;
        int err = 0;
        /* concept_value->name is the value of the key (e.g. 151163 or sst) */
        while (concept_condition) {
            char condition_value[512] = {0,};
            grib_expression* expression = concept_condition->expression;
            const char* condition_name  = concept_condition->name;
            /* printf("%s\n", concept_value->name); */
            /* condition_name is discipline, parameterCategory etc. */
            if (strcmp(expression->cclass->name, "long") == 0) {
                grib_expression_long* el = (grib_expression_long*)expression;
                snprintf(condition_value, sizeof(condition_value), "%ld", el->value);
            }
            else if (strcmp(expression->cclass->name, "functor") == 0) {
                grib_expression_functor* ef = (grib_expression_functor*)expression;
                snprintf(condition_value, sizeof(condition_value), "%s", ef->name);
            }
            else if (strcmp(expression->cclass->name, "string") == 0) {
                grib_expression_string* es = (grib_expression_string*)expression;
                snprintf(condition_value, sizeof(condition_value), "%s", es->value);
            }
            else {
                fprintf(stderr, "%s %s: Unknown class name: '%s'\n",
                        key, concept_value->name, expression->cclass->name);
                Assert(0);
            }
            if (!isLocal && strcmp(condition_name, "localTablesVersion") == 0) {
                fprintf(stderr, "%s %s: Cannot have localTablesVersion key in WMO file %s!\n",
                        key, concept_value->name, filename);
                return GRIB_INVALID_KEY_VALUE;
            }
            if (strcmp(condition_name, "typeOfFirstFixedSurface") == 0) {
                type1Missing = type_of_surface_missing(condition_name, condition_value);
            }
            if (strcmp(condition_name, "typeOfSecondFixedSurface") == 0) {
                type2Missing = type_of_surface_missing(condition_name, condition_value);
            }
            if (strcmp(condition_name, "scaleFactorOfFirstFixedSurface") == 0) {
                scaleFactor1Missing = scale_factor_missing(condition_value);
            }
            if (strcmp(condition_name, "scaleFactorOfSecondFixedSurface") == 0) {
                scaleFactor2Missing = scale_factor_missing(condition_value);
            }
            if (strcmp(condition_name, "scaledValueOfFirstFixedSurface") == 0) {
                scaledValue1Missing = (strncmp(condition_value, "missing", 7) == 0);
            }
            if (strcmp(condition_name, "scaledValueOfSecondFixedSurface") == 0) {
                scaledValue2Missing = (strncmp(condition_value, "missing", 7) == 0);
            }

            concept_condition = concept_condition->next;
        }
        /* Now check the scale factor/value pairs */
        if (type1Missing == 1 && scaleFactor1Missing == 0 && scaledValue1Missing == 0) err = 1;
        if (type2Missing == 1 && scaleFactor2Missing == 0 && scaledValue2Missing == 0) err = 1;
        if (scaleFactor1Missing == 1 && scaledValue1Missing == 0) err = 1;
        if (scaleFactor1Missing == 0 && scaledValue1Missing == 1) err = 1;
        if (scaleFactor2Missing == 1 && scaledValue2Missing == 0) err = 1;
        if (scaleFactor2Missing == 0 && scaledValue2Missing == 1) err = 1;
        if (err) {
            fprintf(stderr,
                    "Error: Mismatched type of surface, scale factor, scaled value keys for %s='%s'.\n"
                    "       If the type of surface is missing so should its scaled keys\n"
                    "       If the scale factor is missing so should the scaled value and vice versa\n",
                    key, concept_value->name);
            return GRIB_INVALID_KEY_VALUE;
        }
        concept_value = concept_value->next;
    }
    return GRIB_SUCCESS;
}

int main(int argc, char** argv)
{
    int err                       = 0;
    const char* concepts_key      = argv[1];
    const char* concepts_filename = argv[2];

    Assert(argc == 3);
    err = grib_check_param_concepts(concepts_key, concepts_filename);
    if (err) return err;

    printf("ALL OK\n");

    return 0;
}
