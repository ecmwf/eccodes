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

#define NUMBER(x) (sizeof(x) / sizeof(x[0]))

int assertion_caught = 0;
int logging_caught = 0;

typedef enum
{
    IBM_FLOAT,
    IEEE_FLOAT
} FloatRep;

static void compare_doubles(const double d1, const double d2, const double epsilon)
{
    Assert(fabs(d1 - d2) < epsilon);
}

static void check_float_representation(const double val, const double expected, const FloatRep rep)
{
    double out             = 0;
    const double tolerance = 1e-9;
    if (rep == IBM_FLOAT)
        Assert(grib_nearest_smaller_ibm_float(val, &out) == GRIB_SUCCESS);
    else
        Assert(grib_nearest_smaller_ieee_float(val, &out) == GRIB_SUCCESS);

    /*printf("%s: d1=%10.20f, out=%10.20f\n", (rep==IBM_FLOAT)?"ibm":"ieee", val, out);*/

    compare_doubles(out, expected, tolerance);
}

static void test_get_git_sha1()
{
    printf("Running %s ...\n", __func__);

    const char* sha1 = grib_get_git_sha1();
    Assert(sha1 != NULL);
}

static void test_get_build_date()
{
    printf("Running %s ...\n", __func__);

    const char* bdate = codes_get_build_date();
    Assert(bdate != NULL);
    /* Should be of the format YYYY.MM.DD or empty (not implemented) */
    Assert( strlen(bdate) == 0 || isdigit(bdate[0]) );
}

static void test_grib_nearest_smaller_ibmfloat()
{
    printf("Running %s ...\n", __func__);

    check_float_representation(-1.0, -1.0, IBM_FLOAT);
    check_float_representation(0.0, 0.0, IBM_FLOAT);
    check_float_representation(1.0, 1.0, IBM_FLOAT);
    check_float_representation(1.1, 1.0999994277954, IBM_FLOAT);
    check_float_representation(10.6, 10.599999427795, IBM_FLOAT);
    check_float_representation(7.85, 7.8499994277954, IBM_FLOAT);
}

static void test_grib_nearest_smaller_ieeefloat()
{
    printf("Running %s ...\n", __func__);

    check_float_representation(-1.0, -1.0, IEEE_FLOAT);
    check_float_representation(0.0, 0.0, IEEE_FLOAT);
    check_float_representation(1.0, 1.0, IEEE_FLOAT);
    check_float_representation(1.1, 1.0999999046325, IEEE_FLOAT);
    check_float_representation(10.6, 10.599999427795, IEEE_FLOAT);
    check_float_representation(7.85, 7.8499999046325, IEEE_FLOAT);
}

static void test_gaussian_latitudes(int order)
{
    printf("Running %s ...\n", __func__);

    int ret       = 0;
    const int num = 2 * order;
    double lat1 = 0, lat2 = 0;
    double* lats = (double*)malloc(sizeof(double) * num);

    ret = grib_get_gaussian_latitudes(order, lats);
    Assert(ret == GRIB_SUCCESS);

    lat1 = lats[0];
    lat2 = lats[num - 1];
    /* Check first and last latitudes are the same with opposite sign */
    compare_doubles(lat1, -lat2, 1.0e-6);

    free(lats);
}

static void test_gaussian_latitude_640()
{
    printf("Running %s ...\n", __func__);

    /* Test latitudes for one specific Gaussian number */
    const int order        = 640;
    const int num          = 2 * order;
    int ret                = 0;
    const double tolerance = 1e-6;
    double* lats           = (double*)malloc(sizeof(double) * num);
    ret                    = grib_get_gaussian_latitudes(order, lats);
    Assert(ret == GRIB_SUCCESS);

    compare_doubles(lats[0], 89.892396, tolerance);
    compare_doubles(lats[1], 89.753005, tolerance);
    compare_doubles(lats[2], 89.612790, tolerance);
    compare_doubles(lats[3], 89.472390, tolerance);
    compare_doubles(lats[4], 89.331918, tolerance);
    compare_doubles(lats[5], 89.191413, tolerance);
    compare_doubles(lats[6], 89.050889, tolerance);
    compare_doubles(lats[7], 88.910352, tolerance);
    compare_doubles(lats[8], 88.769808, tolerance);
    compare_doubles(lats[9], 88.629259, tolerance);
    compare_doubles(lats[10], 88.488706, tolerance);
    compare_doubles(lats[11], 88.348150, tolerance);
    compare_doubles(lats[12], 88.207592, tolerance);
    compare_doubles(lats[13], 88.067032, tolerance);
    compare_doubles(lats[14], 87.926471, tolerance);
    compare_doubles(lats[15], 87.785908, tolerance);

    compare_doubles(lats[1262], -87.504781, tolerance);
    compare_doubles(lats[1263], -87.645345, tolerance);
    compare_doubles(lats[1264], -87.785908, tolerance);
    compare_doubles(lats[1265], -87.926471, tolerance);
    compare_doubles(lats[1266], -88.067032, tolerance);
    compare_doubles(lats[1267], -88.207592, tolerance);
    compare_doubles(lats[1268], -88.348150, tolerance);
    compare_doubles(lats[1269], -88.488706, tolerance);
    compare_doubles(lats[1270], -88.629259, tolerance);
    compare_doubles(lats[1271], -88.769808, tolerance);
    compare_doubles(lats[1272], -88.910352, tolerance);
    compare_doubles(lats[1273], -89.050889, tolerance);
    compare_doubles(lats[1274], -89.191413, tolerance);
    compare_doubles(lats[1275], -89.331918, tolerance);
    compare_doubles(lats[1276], -89.472390, tolerance);
    compare_doubles(lats[1277], -89.612790, tolerance);
    compare_doubles(lats[1278], -89.753005, tolerance);
    compare_doubles(lats[1279], -89.892396, tolerance);

    free(lats);
}

static void test_string_splitting()
{
    printf("Running %s ...\n", __func__);

    int i          = 0;
    char input[80] = "Born|To|Be|Wild";
    char** list    = 0;

    list           = string_split(input, "|");
    if (!list) { Assert(!"List is NULL"); return; }
    for (i = 0; list[i] != NULL; ++i) {} /* count how many tokens */
    Assert(i == 4);
    if (!list[0] || !STR_EQUAL(list[0], "Born")) Assert(0);
    if (!list[1] || !STR_EQUAL(list[1], "To"))   Assert(0);
    if (!list[2] || !STR_EQUAL(list[2], "Be"))   Assert(0);
    if (!list[3] || !STR_EQUAL(list[3], "Wild")) Assert(0);
    Assert(list[4] == NULL);
    for (i = 0; list[i] != NULL; ++i) free(list[i]);
    free(list);

    strcpy(input, "12345|a gap|");
    list = string_split(input, "|");
    if (!list) { Assert(0); return; }
    for (i = 0; list[i] != NULL; ++i) {} /* count how many tokens */
    Assert(i == 2);
    if (!list[0] || !STR_EQUAL(list[0], "12345")) Assert(0);
    if (!list[1] || !STR_EQUAL(list[1], "a gap")) Assert(0);
    Assert(list[2] == NULL);
    for (i = 0; list[i] != NULL; ++i) free(list[i]);
    free(list);

    strcpy(input, "Steppenwolf");
    list = string_split(input, ",");
    if (!list) { Assert(0); return; }
    for (i = 0; list[i] != NULL; ++i) {} /* count how many tokens */
    Assert(i == 1);
    if (!list[0] || !STR_EQUAL(list[0], "Steppenwolf")) Assert(0);
    Assert(list[1] == NULL);
    for (i = 0; list[i] != NULL; ++i) free(list[i]);
    free(list);

    /* Note: currently cannot cope with */
    /*  input being NULL */
    /*  input being empty */
    /*  input having several adjacent delimiters e.g. 'A||B|||C' */
}

static void my_assertion_proc(const char* message)
{
    printf("It's OK. I caught the assertion: %s\n", message);
    assertion_caught = 1;
}

static void test_assertion_catching()
{
    printf("Running %s ...\n", __func__);

    char empty[] = "";
    char** list  = 0;
    int i        = 0;
    Assert(assertion_caught == 0);
    codes_set_codes_assertion_failed_proc(&my_assertion_proc);

    /* Do something illegal */
    list = string_split(empty, " ");

    Assert(assertion_caught == 1);

    /* Restore everything */
    codes_set_codes_assertion_failed_proc(NULL);
    assertion_caught = 0;

    for (i = 0; list[i] != NULL; ++i)
        free(list[i]);
    free(list);
}

static void my_logging_proc(const grib_context* c, int level, const char* mesg)
{
    logging_caught = 1;
}
static void test_logging_proc()
{
    grib_context* context = grib_context_get_default();
    Assert(logging_caught == 0);

    /* Override default behaviour */
    grib_context_set_logging_proc(context, my_logging_proc);
    grib_context_log(context, GRIB_LOG_ERROR, "test_logging_proc: This error will be handled by me");
    Assert(logging_caught == 1);

    /* Restore the logging proc */
    logging_caught = 0;
    grib_context_set_logging_proc(context, NULL);
    grib_context_log(context, GRIB_LOG_ERROR, "test_logging_proc: This error will come out as normal");
    Assert(logging_caught == 0);
}

static void test_concept_condition_strings()
{
    printf("Running %s ...\n", __func__);

    int err           = 0;
    char result[1024] = {0,};
    grib_context* context = NULL;
    grib_handle* h = grib_handle_new_from_samples(context, "GRIB2");

    err = get_concept_condition_string(h, "typeOfLevel", NULL, result);
    Assert(!err);
    Assert(strcmp(result, "typeOfFirstFixedSurface=1,typeOfSecondFixedSurface=255") == 0);

    err = get_concept_condition_string(h, "paramId", NULL, result);
    Assert(!err);
    Assert(strcmp(result, "discipline=0,parameterCategory=0,parameterNumber=0") == 0);

    err = get_concept_condition_string(h, "gridType", NULL, result);
    Assert(!err);
    Assert(strcmp(result, "gridDefinitionTemplateNumber=0,PLPresent=0") == 0);

    err = get_concept_condition_string(h, "stepType", NULL, result);
    Assert(!err);
    Assert(strcmp(result, "selectStepTemplateInstant=1,stepTypeInternal=instant") == 0);

    grib_handle_delete(h);
}

static void test_string_trimming()
{
    printf("Running %s ...\n", __func__);

    char a[] = " Standing  ";
    char b[] = "  Weeping ";
    char c[] = "  Silhouette ";
    char d[] = " The Forest Of October  ";
    char e[] = "\t\n Apostle In Triumph \r ";
    char* pA = a;
    char* pB = b;
    char* pC = c;
    char* pD = d;
    char* pE = e;

    string_lrtrim(&pA, 0, 1); /*right only*/
    Assert( strcmp(pA, " Standing")==0 );

    string_lrtrim(&pB, 1, 0); /*left only*/
    Assert( strcmp(pB, "Weeping ")==0 );

    string_lrtrim(&pC, 1, 1); /*both ends*/
    Assert( strcmp(pC, "Silhouette")==0 );

    string_lrtrim(&pD, 1, 1); /*make sure other spaces are not removed*/
    Assert( strcmp(pD, "The Forest Of October")==0 );

    string_lrtrim(&pE, 1, 1); /* Other chars */
    Assert( strcmp(pE, "Apostle In Triumph")==0 );
}

static void test_string_ends_with()
{
    printf("Running %s ...\n", __func__);

    Assert( string_ends_with("GRIB2.tmpl", "tmpl") == 1 );
    Assert( string_ends_with("GRIB2.tmpl", ".tmpl") == 1 );
    Assert( string_ends_with("", "") == 1 );
    Assert( string_ends_with(".", ".") == 1 );
    Assert( string_ends_with("Bam", "") == 1 );

    Assert( string_ends_with("GRIB2.tmpl", "tmp") == 0 );
    Assert( string_ends_with("GRIB2.tmpl", "tmpl0") == 0 );
    Assert( string_ends_with("GRIB2.tmpl", "1.tmpl") == 0 );
    Assert( string_ends_with("GRIB2.tmpl", " ") == 0 );
}

static void test_string_to_long()
{
    printf("Running %s ...\n", __func__);

    long lVal = 0;
    Assert( string_to_long("0", &lVal, 1) == GRIB_SUCCESS);
    Assert( lVal == 0 );

    Assert( string_to_long("42", &lVal, 1) == GRIB_SUCCESS);
    Assert( lVal == 42 );

    Assert( string_to_long("-1", &lVal, 1) == GRIB_SUCCESS);
    Assert( lVal == -1 );
    Assert( string_to_long("+999", &lVal, 1) == GRIB_SUCCESS);
    Assert( lVal == 999 );

    Assert( string_to_long("15MB", &lVal, 0) == GRIB_SUCCESS);
    Assert( lVal == 15 );

    // illegal cases
    Assert( string_to_long("4000000000000000000000", &lVal, 1) == GRIB_INVALID_ARGUMENT);
    Assert( string_to_long("XY", &lVal, 1) == GRIB_INVALID_ARGUMENT);
    Assert( string_to_long("A6", &lVal, 1) == GRIB_INVALID_ARGUMENT);
    Assert( string_to_long("5K", &lVal, 1) == GRIB_INVALID_ARGUMENT);
}

static void test_string_replace_char()
{
    printf("Running %s ...\n", __func__);

    char input[32] = {0,};
    strncpy(input, "Mask Of Zoro", sizeof(input));
    string_replace_char(input, ' ', '-');
    Assert(STR_EQUAL(input, "Mask-Of-Zoro"));
}

static void test_string_remove_char()
{
    printf("Running %s ...\n", __func__);
    char input[64] = "a:b:c";
    string_remove_char(input, ':');
    Assert(STR_EQUAL(input, "abc"));
}

static void test_gribex_mode()
{
    grib_context* c = grib_context_get_default();
    printf("Running %s ...\n", __func__);

    Assert( grib_get_gribex_mode(c) == 0 ); /* default is OFF */
    grib_gribex_mode_on(c);
    Assert( grib_get_gribex_mode(c) == 1 );
    grib_gribex_mode_off(c);
    Assert( grib_get_gribex_mode(c) == 0 );
}

static void test_grib_binary_search()
{
    printf("Running %s ...\n", __func__);

    double array_asc[] = {-0.1, 33.4, 56.1, 101.8};
    double array_desc[] = {88, 78, 0, -88};
    const size_t idx_asc_max = NUMBER(array_asc) - 1;
    const size_t idx_desc_max = NUMBER(array_desc) - 1;
    size_t idx_upper=0, idx_lower = 0;

    grib_binary_search(array_asc, idx_asc_max, 56.0, &idx_upper, &idx_lower);
    Assert(idx_lower == 1 && idx_upper == 2);
    grib_binary_search(array_asc, idx_asc_max, 56.1, &idx_upper, &idx_lower);
    Assert(idx_lower == 2 && idx_upper == 3);
    grib_binary_search(array_asc, idx_asc_max, -0.1, &idx_upper, &idx_lower);
    Assert(idx_lower == 0 && idx_upper == 1);

    grib_binary_search(array_desc, idx_desc_max, 88, &idx_upper, &idx_lower);
    Assert(idx_lower == 0 && idx_upper == 1);
    grib_binary_search(array_desc, idx_desc_max, -88, &idx_upper, &idx_lower);
    Assert(idx_lower == 2 && idx_upper == 3);
    grib_binary_search(array_desc, idx_desc_max, 1, &idx_upper, &idx_lower);
    Assert(idx_lower == 1 && idx_upper == 2);
}

static void test_parse_keyval_string()
{
    printf("Running %s ...\n", __func__);

    int err = 0;
    int values_required = 1;
    int count = 0;
    grib_values values1[128] = {{0},};
    grib_values values2[128] = {{0},};
    grib_values values3[128] = {{0},};
    const int max_count = 128;
    char input1[] = "key1=value1,key2!=value2";
    char input2[] = "x=14";
    char input3[] = "mars.level=0.978";

    count = max_count;
    err = parse_keyval_string(NULL, input1,
                              values_required, GRIB_TYPE_UNDEFINED, values1, &count);
    Assert( !err );
    Assert( count == 2 );
    grib_print_values("print values test: values1", values1);

    Assert( strcmp(values1[0].name, "key1")==0 );
    Assert( strcmp(values1[0].string_value, "value1")==0 );
    Assert( values1[0].equal == 1 );
    Assert( strcmp(values1[1].name, "key2")==0 );
    Assert( strcmp(values1[1].string_value, "value2")==0 );
    Assert( values1[1].equal == 0 );
    /* Note how the input is modified by the tokenizer (thanks to strtok_r) */
    Assert( strcmp(input1, "key1=value1")==0 );
    free( (void*)values1[0].name );
    free( (void*)values1[1].name );
    free( (void*)values1[0].string_value );
    free( (void*)values1[1].string_value );

    count = max_count;
    err = parse_keyval_string(NULL, input2,
                              values_required, GRIB_TYPE_LONG, values2, &count);
    Assert( !err );
    Assert( count == 1 );
    grib_print_values("print values test: values2", values2);
    Assert( strcmp(values2[0].name, "x")==0 );
    Assert( values2[0].long_value == 14 );
    Assert( values2[0].equal == 1 );
    free( (void*)values2[0].name );

    count = max_count;
    err = parse_keyval_string(NULL, input3,
                              values_required, GRIB_TYPE_DOUBLE, values3, &count);
    Assert( !err );
    Assert( count == 1 );
    grib_print_values("print values test: values3", values3);
    Assert( strcmp(values3[0].name, "mars.level")==0 );
    free( (void*)values3[0].name );
}

static void test_dates()
{
    printf("Running %s ...\n", __func__);

    Assert( is_date_valid(1979,12, 1, 0,0,0) );
    Assert( is_date_valid(1900, 1, 1, 0,0,0) );
    Assert( is_date_valid(1964, 4, 6, 0,0,0) );
    Assert( is_date_valid(2023, 3, 4, 0,0,0) );
    Assert( is_date_valid(2023, 3, 4, 12,0,0) );
    Assert( is_date_valid(2023, 3, 4, 0,10,0) );
    Assert( is_date_valid(2023, 3, 4, 0,0,59) );
    Assert( is_date_valid(0000, 3, 4, 0,0,0) );
    Assert( is_date_valid(2020, 2, 29, 0,0,0) );//leap year

    Assert( !is_date_valid(  10, -1, 1, 0,0,0) );// bad months
    Assert( !is_date_valid(1900, 0,  1, 0,0,0) );
    Assert( !is_date_valid(1900, 13, 1, 0,0,0) );

    Assert( !is_date_valid(1900, 5,  0, 0,0,0) ); // bad days
    Assert( !is_date_valid(2000, 5, 32, 0,0,0) );
    Assert( !is_date_valid(2000, 5, -7, 0,0,0) );

    Assert( !is_date_valid(2000, 5, 8, 99,0,0) );//bad hours
    Assert( !is_date_valid(2000, 5, 9, -1,0,0) );

    Assert( !is_date_valid(2000, 5, 8, 0, 61,0) );//bad mins
    Assert( !is_date_valid(2000, 5, 9, 0,-1, 0) );

    Assert( !is_date_valid(2000, 5, 8, 0, 1, -1) );//bad secs
    Assert( !is_date_valid(2000, 5, 9, 0, 1, 60) );

    Assert( !is_date_valid(2023, 2, 29, 0,0,0) );//Feb
}

void test_scale_factor_scaled_values()
{
    printf("Running %s ...\n", __func__);

    int err =0;
    int64_t value, factor;
    const int64_t scaled_value_max = 4294967295; // usually 4 octets
    const int64_t scale_factor_max = 255; // usually 1 octet

    err = compute_scaled_value_and_scale_factor(0, scaled_value_max, scale_factor_max, &value, &factor);
    Assert(!err);
    Assert(value == 0);
    Assert(factor == 0);

    err = compute_scaled_value_and_scale_factor(1, scaled_value_max, scale_factor_max, &value, &factor);
    Assert(!err);
    Assert(value == 1);
    Assert(factor == 0);

    err = compute_scaled_value_and_scale_factor(1.5, scaled_value_max, scale_factor_max, &value, &factor);
    Assert(!err);
    Assert(value == 15);
    Assert(factor == 1);

    err = compute_scaled_value_and_scale_factor(4.56, scaled_value_max, scale_factor_max, &value, &factor);
    Assert(!err);
    Assert(value == 456);
    Assert(factor == 2);

    err = compute_scaled_value_and_scale_factor(-0.003, scaled_value_max, scale_factor_max, &value, &factor);
    Assert(!err);
    Assert(value == -3);
    Assert(factor == 3);

    err = compute_scaled_value_and_scale_factor(145.889, scaled_value_max, scale_factor_max, &value, &factor);
    Assert(!err);
    Assert(value == 145889);
    Assert(factor == 3);

    err = compute_scaled_value_and_scale_factor(1111.00009, scaled_value_max, scale_factor_max, &value, &factor);
    Assert(!err);
    Assert(value == 111100009);
    Assert(factor == 5);
}

void test_iarray()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();
    grib_iarray* a = grib_iarray_new(c, 10, 10);
    grib_iarray_push(a, 42);
    grib_iarray_push(a, 10000);
    grib_iarray_print("iarray", a);

    grib_iarray* b = grib_iarray_new(c, 1, 1);
    grib_iarray_push(b, 0);
    grib_iarray_push(b, -1);
    grib_iarray_push(b, +1);

    grib_viarray* va = grib_viarray_new(c, 1, 1);
    grib_viarray_push(c, va, a);
    grib_viarray_push(c, va, b);
    grib_viarray_print("viarray", va);

    grib_iarray_delete(a);
    grib_iarray_delete(b);
    grib_viarray_delete(c, va);
}

void test_darray()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();
    grib_darray* a = grib_darray_new(c, 10, 10);
    grib_darray_push(c, a, 42.009);
    grib_darray_push(c, a, -1.11);
    grib_darray_push(c, a, 5099);
    grib_darray_print("darray", a);

    grib_darray* b = grib_darray_new(c, 5, 1);
    grib_darray_push(c, b, 8);
    grib_darray_push(c, b, 12);

    grib_vdarray* va = grib_vdarray_new(c, 1, 1);
    grib_vdarray_push(c, va, a);
    grib_vdarray_push(c, va, b);
    grib_vdarray_print("vdarray", va);

    grib_darray_delete(c, a);
    grib_darray_delete(c, b);
    grib_vdarray_delete(c, va);
}

void test_sarray()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();
    grib_sarray* a = grib_sarray_new(c, 10, 10);
    grib_sarray_push(c, a, "ants");
    grib_sarray_push(c, a, "bugs");
    grib_sarray_print("sarray", a);
    grib_sarray_delete(c, a);
}

void test_codes_get_product_name()
{
    printf("Running %s ...\n", __func__);
    Assert( STR_EQUAL("ANY",   codes_get_product_name(PRODUCT_ANY)) );
    Assert( STR_EQUAL("GRIB",  codes_get_product_name(PRODUCT_GRIB)) );
    Assert( STR_EQUAL("BUFR",  codes_get_product_name(PRODUCT_BUFR)) );
    Assert( STR_EQUAL("GTS",   codes_get_product_name(PRODUCT_GTS)) );
    Assert( STR_EQUAL("METAR", codes_get_product_name(PRODUCT_METAR)) );
}

void test_codes_get_type_name()
{
    printf("Running %s ...\n", __func__);
    Assert( STR_EQUAL("long",    grib_get_type_name(GRIB_TYPE_LONG)) );
    Assert( STR_EQUAL("string",  grib_get_type_name(GRIB_TYPE_STRING)) );
    Assert( STR_EQUAL("double",  grib_get_type_name(GRIB_TYPE_DOUBLE)) );
    Assert( STR_EQUAL("bytes",   grib_get_type_name(GRIB_TYPE_BYTES)) );
    Assert( STR_EQUAL("label",   grib_get_type_name(GRIB_TYPE_LABEL)) );
    Assert( STR_EQUAL("section", grib_get_type_name(GRIB_TYPE_SECTION)) );
}

int main(int argc, char** argv)
{
    printf("Doing unit tests. ecCodes version = %ld\n", grib_get_api_version());

    test_iarray();
    test_darray();
    test_sarray();

    test_codes_get_product_name();
    test_codes_get_type_name();

    test_scale_factor_scaled_values();
    test_dates();
    test_logging_proc();
    test_grib_binary_search();
    test_parse_keyval_string();

    test_get_git_sha1();
    test_get_build_date();
    test_gribex_mode();

    test_concept_condition_strings();

    test_assertion_catching();

    test_gaussian_latitude_640();

    test_gaussian_latitudes(32);
    test_gaussian_latitudes(48);
    test_gaussian_latitudes(80);
    test_gaussian_latitudes(128);
    test_gaussian_latitudes(160);
    test_gaussian_latitudes(200);
    test_gaussian_latitudes(256);
    test_gaussian_latitudes(320);
    test_gaussian_latitudes(400);
    test_gaussian_latitudes(512);
    test_gaussian_latitudes(640);
    test_gaussian_latitudes(1024);
    test_gaussian_latitudes(1280);
    test_gaussian_latitudes(2000);

    test_grib_nearest_smaller_ibmfloat();
    test_grib_nearest_smaller_ieeefloat();

    test_string_splitting();
    test_string_ends_with();
    test_string_to_long();
    test_string_trimming();
    test_string_replace_char();
    test_string_remove_char();

    return 0;
}
