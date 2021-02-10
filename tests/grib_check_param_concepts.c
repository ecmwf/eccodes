/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <assert.h>
#include "grib_api_internal.h"

typedef struct grib_expression_long
{
    grib_expression base;
    /* Members defined in long */
    long value;
} grib_expression_long;

typedef struct grib_expression_functor
{
    grib_expression base;
    /* Members defined in functor */
    char* name;
    grib_arguments* args;
} grib_expression_functor;

static int grib_check_param_concepts(const char* key, const char* filename)
{
    grib_concept_value* concept_value = grib_parse_concept_file(NULL, filename);
    while (concept_value) {
        grib_concept_condition* concept_condition = concept_value->conditions;
        int scaleFactor1Missing = 0, scaleFactor2Missing = 0;
        int scaledValue1Missing = 0, scaledValue2Missing = 0;
        int err = 0;
        /* printf("\nconcept_value->name=%s\n", concept_value->name); // A given paramId e.g. 151163 */
        while (concept_condition) {
            char condition_value[512] = {0,};
            grib_expression* expression = concept_condition->expression;
            const char* condition_name  = concept_condition->name;
            /* printf(" condition_name->name=%s", condition_name); // e.g. discipline, parameterCategory */
            if(strcmp(expression->cclass->name,"long")==0) {
                grib_expression_long* e = (grib_expression_long*)expression;
                sprintf(condition_value, "%ld", e->value);
            } else if(strcmp(expression->cclass->name,"functor")==0) {
                grib_expression_functor* e = (grib_expression_functor*)expression;
                sprintf(condition_value, "%s", e->name);
            } else {
                Assert(0);
            }
            if (strcmp(condition_name, "scaleFactorOfFirstFixedSurface")==0 && strcmp(condition_value,"missing")==0)
                scaleFactor1Missing = 1;
            if (strcmp(condition_name, "scaleFactorOfSecondFixedSurface")==0 && strcmp(condition_value,"missing")==0)
                scaleFactor2Missing = 1;
            if (strcmp(condition_name, "scaledValueOfFirstFixedSurface")==0 && strcmp(condition_value,"missing")==0)
                scaledValue1Missing = 1;
            if (strcmp(condition_name, "scaledValueOfSecondFixedSurface")==0 && strcmp(condition_value,"missing")==0)
                scaledValue2Missing = 1;

            concept_condition = concept_condition->next;
        }
        /* Now check the scale factor/value pairs */
        if (scaleFactor1Missing && !scaledValue1Missing) err = 1;
        if (!scaleFactor1Missing && scaledValue1Missing) err = 1;
        if (scaleFactor2Missing && !scaledValue2Missing) err = 1;
        if (!scaleFactor2Missing && scaledValue2Missing) err = 1;
        if (err) {
            fprintf(stderr, "Error: mismatched scale factor, scaled value keys for %s=%s\n", key, concept_value->name);
            return 1;
        }
        concept_value = concept_value->next;
    }
    return 0; /* ALL OK */
}

int main(int argc, char** argv)
{
    int err = 0;
    const char* concepts_key = argv[1];
    const char* concepts_filename = argv[2];
    
    assert(argc == 3);
    err = grib_check_param_concepts(concepts_key, concepts_filename);
    if (err) return err;

    printf("ALL OK\n");

    return 0;
}
