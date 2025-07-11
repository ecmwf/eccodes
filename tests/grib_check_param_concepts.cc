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

#include "expression/Long.h"
#include "expression/Functor.h"
#include "expression/String.h"
#include "grib_api_internal.h"

static int type_of_surface_missing(const char* name, const char* value)
{
    /* Surface Type is Code Table 4.5 in which 255 is the same as missing */
    if (strcmp(value, "255") == 0) {
        return 1;
    }
    /* Beware of problems where we put 'missing()' for a code table key! */
    if (strncmp(value, "missing", 7) == 0) {
        fprintf(stderr, "Invalid value for %s Code Table entry: '%s'\n", name, value);
        ECCODES_ASSERT(0);
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

#define STAT_PROC_MAX_VAL 103 // increase this with new Code Table 4.10 entries

/*
 * key      = paramId or shortName
 * filename = paramId.def or shortName.def
*/
static int grib_check_param_concepts(const char* key, const char* filename)
{
    int isLocal = 0;
    int count = 0;
    grib_concept_value* concept_value = grib_parse_concept_file(NULL, filename);
    if (!concept_value)
        return GRIB_IO_PROBLEM;

    if (strstr(filename, "/localConcepts/")) {
        isLocal = 1;
    }
    while (concept_value) {
        count++;
        if (strlen(concept_value->name) == 0) {
            fprintf(stderr, "%s %s: Empty concept value (count=%d)\n", key, concept_value->name, count);
            ECCODES_ASSERT(0);
        }
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
        if (strcmp(key, "cfVarName")==0) {
            ECCODES_ASSERT( strlen(concept_value->name) > 0 );
            if ( isdigit(concept_value->name[0]) || strcmp(concept_value->name, "~")==0 ) {
                fprintf(stderr, "%s %s: Invalid cfVarName in file %s\n",
                        key, concept_value->name, filename);
                return GRIB_INVALID_KEY_VALUE;
            }
        }
        while (concept_condition) {
            char condition_value[512] = {0, };
            eccodes::Expression* expression = concept_condition->expression;
            if(expression) {
                const char* condition_name  = concept_condition->name;
                /* printf("%s\n", concept_value->name); */
                /* condition_name is discipline, parameterCategory etc. */
                if (strcmp(expression->class_name(), "long") == 0) {
                    eccodes::expression::Long* el = dynamic_cast<eccodes::expression::Long*>(expression);
                    long value;
                    el->evaluate_long(NULL, &value);
                    snprintf(condition_value, sizeof(condition_value), "%ld", value);
                }
                else if (strcmp(expression->class_name(), "functor") == 0) {
                    eccodes::expression::Functor* ef = dynamic_cast<eccodes::expression::Functor*>(expression);
                    snprintf(condition_value, sizeof(condition_value), "%s", ef->name());
                }
                else if (strcmp(expression->class_name(), "string") == 0) {
                    eccodes::expression::String* es = dynamic_cast<eccodes::expression::String*>(expression);
                    int error;
                    const char* value = es->evaluate_string(NULL, NULL, NULL, &error);
                    snprintf(condition_value, sizeof(condition_value), "%s", value);
                }
                else {
                    fprintf(stderr, "%s %s: Unknown class name: '%s'\n",
                            key, concept_value->name, expression->class_name());
                    ECCODES_ASSERT(0);
                }
                if (!isLocal && strcmp(condition_name, "localTablesVersion") == 0) {
                    fprintf(stderr, "%s %s: Cannot have localTablesVersion key in WMO file %s!\n",
                            key, concept_value->name, filename);
                    return GRIB_INVALID_KEY_VALUE;
                }
                if (strcmp(condition_name, "typeOfStatisticalProcessing") == 0) {
                    long lValue = atol(condition_value);
                    if (lValue > STAT_PROC_MAX_VAL || lValue < 0) {
                        fprintf(stderr, "Bad value for %s in %s=%s in file %s\n",
                                condition_name, key, concept_value->name, filename);
                        return GRIB_INVALID_KEY_VALUE;
                    }
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
            }
            concept_condition = concept_condition->next;
        }
        /* Now check the scale factor/value pairs */
        if (type1Missing == 1 && (scaleFactor1Missing == 0 || scaledValue1Missing == 0)) err = 1;
        if (type2Missing == 1 && (scaleFactor2Missing == 0 || scaledValue2Missing == 0)) err = 1;
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

    ECCODES_ASSERT(argc == 3);
    err = grib_check_param_concepts(concepts_key, concepts_filename);
    if (err) return err;

    printf("ALL OK\n");

    return 0;
}
