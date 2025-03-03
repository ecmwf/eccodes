/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "eccodes.h"
#include "action/Concept.h"

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
    ECCODES_ASSERT(fabs(d1 - d2) < epsilon);
}

static void check_float_representation(const double val, const double expected, const FloatRep rep)
{
    double out             = 0;
    const double tolerance = 1e-9;
    if (rep == IBM_FLOAT)
        ECCODES_ASSERT(grib_nearest_smaller_ibm_float(val, &out) == GRIB_SUCCESS);
    else
        ECCODES_ASSERT(grib_nearest_smaller_ieee_float(val, &out) == GRIB_SUCCESS);

    /*printf("%s: d1=%10.20f, out=%10.20f\n", (rep==IBM_FLOAT)?"ibm":"ieee", val, out);*/

    compare_doubles(out, expected, tolerance);
}

static void test_get_package_name()
{
    printf("Running %s ...\n", __func__);

    const char* pn = codes_get_package_name();
    ECCODES_ASSERT(pn != NULL);
    ECCODES_ASSERT(strlen(pn) > 1);
    printf("Package name = %s\n", pn);
}

static void test_get_git_sha1()
{
    printf("Running %s ...\n", __func__);

    const char* sha1 = codes_get_git_sha1();
    ECCODES_ASSERT(sha1 != NULL);
    printf("Git SHA1 = %s\n", sha1);
}

static void test_get_build_date()
{
    printf("Running %s ...\n", __func__);

    const char* bdate = codes_get_build_date();
    ECCODES_ASSERT(bdate != NULL);
    /* Should be of the format YYYY.MM.DD or empty (not implemented) */
    ECCODES_ASSERT( strlen(bdate) == 0 || isdigit(bdate[0]) );
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

    ret = codes_get_gaussian_latitudes(order, lats);
    ECCODES_ASSERT(ret == GRIB_SUCCESS);

    lat1 = lats[0];
    lat2 = lats[num - 1];
    /* Check first and last latitudes are the same with opposite sign */
    compare_doubles(lat1, -lat2, 1.0e-6);

    free(lats);
}

static void test_grib_get_reduced_row_legacy()
{
    printf("Running %s ...\n", __func__);
    long npoints=0, ilon_first=0, ilon_last=0;

    grib_get_reduced_row_legacy(25, 0.0, 100.0, &npoints, &ilon_first, &ilon_last);
    // printf("Result: npoints=%ld, ilon_first=%ld, ilon_last=%ld\n", npoints, ilon_first, ilon_last);

    grib_get_reduced_row_legacy(25, 90.0, 100.0, &npoints, &ilon_first, &ilon_last);
    grib_get_reduced_row_legacy(25, 295.0, 300.0, &npoints, &ilon_first, &ilon_last);
    grib_get_reduced_row_legacy(25, -20.0, 30.0, &npoints, &ilon_first, &ilon_last);
    grib_get_reduced_row_legacy(25, 301, 300.0, &npoints, &ilon_first, &ilon_last);
    grib_get_reduced_row_legacy(200, 0.0, 359.0, &npoints, &ilon_first, &ilon_last);
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
    ret                    = codes_get_gaussian_latitudes(order, lats);
    ECCODES_ASSERT(ret == GRIB_SUCCESS);

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
    if (!list) { ECCODES_ASSERT(!"List is NULL"); return; }
    for (i = 0; list[i] != NULL; ++i) {} /* count how many tokens */
    ECCODES_ASSERT(i == 4);
    if (!list[0] || !STR_EQUAL(list[0], "Born")) ECCODES_ASSERT(0);
    if (!list[1] || !STR_EQUAL(list[1], "To"))   ECCODES_ASSERT(0);
    if (!list[2] || !STR_EQUAL(list[2], "Be"))   ECCODES_ASSERT(0);
    if (!list[3] || !STR_EQUAL(list[3], "Wild")) ECCODES_ASSERT(0);
    ECCODES_ASSERT(list[4] == NULL);
    for (i = 0; list[i] != NULL; ++i) free(list[i]);
    free(list);

    strcpy(input, "12345|a gap|");
    list = string_split(input, "|");
    if (!list) { ECCODES_ASSERT(0); return; }
    for (i = 0; list[i] != NULL; ++i) {} /* count how many tokens */
    ECCODES_ASSERT(i == 2);
    if (!list[0] || !STR_EQUAL(list[0], "12345")) ECCODES_ASSERT(0);
    if (!list[1] || !STR_EQUAL(list[1], "a gap")) ECCODES_ASSERT(0);
    ECCODES_ASSERT(list[2] == NULL);
    for (i = 0; list[i] != NULL; ++i) free(list[i]);
    free(list);

    strcpy(input, "Steppenwolf");
    list = string_split(input, ",");
    if (!list) { ECCODES_ASSERT(0); return; }
    for (i = 0; list[i] != NULL; ++i) {} /* count how many tokens */
    ECCODES_ASSERT(i == 1);
    if (!list[0] || !STR_EQUAL(list[0], "Steppenwolf")) ECCODES_ASSERT(0);
    ECCODES_ASSERT(list[1] == NULL);
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
    char** list  = NULL;
    ECCODES_ASSERT(assertion_caught == 0);
    codes_set_codes_assertion_failed_proc(&my_assertion_proc);

    /* Do something illegal */
    list = string_split(empty, " ");

    ECCODES_ASSERT(assertion_caught == 1);
    ECCODES_ASSERT(list == NULL);

    /* Restore everything */
    codes_set_codes_assertion_failed_proc(NULL);
    assertion_caught = 0;

    // for (i = 0; list[i] != NULL; ++i)
    //     free(list[i]);
    free(list);
}

static void my_logging_proc(const grib_context* c, int level, const char* mesg)
{
    logging_caught = 1;
}
static void test_logging_proc()
{
    grib_context* context = grib_context_get_default();
    ECCODES_ASSERT(logging_caught == 0);

    /* Override default behaviour */
    grib_context_set_logging_proc(context, my_logging_proc);
    grib_context_log(context, GRIB_LOG_ERROR, "test_logging_proc: This error will be handled by me");
    ECCODES_ASSERT(logging_caught == 1);

    /* Restore the logging proc */
    logging_caught = 0;
    grib_context_set_logging_proc(context, NULL);
    grib_context_log(context, GRIB_LOG_ERROR, "test_logging_proc: This error will come out as normal");
    ECCODES_ASSERT(logging_caught == 0);
}

static void my_print_proc(const grib_context* c, void* descriptor, const char* mesg)
{
}
static void test_print_proc()
{
    grib_context* context = grib_context_get_default();
    grib_context_set_print_proc(context, my_print_proc);
    grib_context_set_print_proc(context, NULL);
}

static void test_concept_condition_strings()
{
    printf("Running %s ...\n", __func__);

    int err           = 0;
    char result[1024] = {0,};
    grib_context* context = NULL;
    grib_handle* h = grib_handle_new_from_samples(context, "GRIB2");
    if (!h) return;

    err = get_concept_condition_string(h, "paramId", NULL, result);
    ECCODES_ASSERT(!err);
    ECCODES_ASSERT(strcmp(result, "discipline=0,parameterCategory=0,parameterNumber=0") == 0);

    err = get_concept_condition_string(h, "gridType", NULL, result);
    ECCODES_ASSERT(!err);
    ECCODES_ASSERT(strcmp(result, "gridDefinitionTemplateNumber=0,PLPresent=0") == 0);

    err = get_concept_condition_string(h, "stepType", NULL, result);
    ECCODES_ASSERT(!err);
    ECCODES_ASSERT(strcmp(result, "selectStepTemplateInstant=1,stepTypeInternal=instant") == 0);

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
    ECCODES_ASSERT( strcmp(pA, " Standing")==0 );

    string_lrtrim(&pB, 1, 0); /*left only*/
    ECCODES_ASSERT( strcmp(pB, "Weeping ")==0 );

    string_lrtrim(&pC, 1, 1); /*both ends*/
    ECCODES_ASSERT( strcmp(pC, "Silhouette")==0 );

    string_lrtrim(&pD, 1, 1); /*make sure other spaces are not removed*/
    ECCODES_ASSERT( strcmp(pD, "The Forest Of October")==0 );

    string_lrtrim(&pE, 1, 1); /* Other chars */
    ECCODES_ASSERT( strcmp(pE, "Apostle In Triumph")==0 );
}

static void test_string_ends_with()
{
    printf("Running %s ...\n", __func__);

    ECCODES_ASSERT( string_ends_with("GRIB2.tmpl", "tmpl") == 1 );
    ECCODES_ASSERT( string_ends_with("GRIB2.tmpl", ".tmpl") == 1 );
    ECCODES_ASSERT( string_ends_with("", "") == 1 );
    ECCODES_ASSERT( string_ends_with(".", ".") == 1 );
    ECCODES_ASSERT( string_ends_with("Bam", "") == 1 );

    ECCODES_ASSERT( string_ends_with("GRIB2.tmpl", "tmp") == 0 );
    ECCODES_ASSERT( string_ends_with("GRIB2.tmpl", "tmpl0") == 0 );
    ECCODES_ASSERT( string_ends_with("GRIB2.tmpl", "1.tmpl") == 0 );
    ECCODES_ASSERT( string_ends_with("GRIB2.tmpl", " ") == 0 );
}

static void test_string_to_long()
{
    printf("Running %s ...\n", __func__);

    long lVal = 0;
    ECCODES_ASSERT( string_to_long("0", &lVal, 1) == GRIB_SUCCESS);
    ECCODES_ASSERT( lVal == 0 );

    ECCODES_ASSERT( string_to_long("42", &lVal, 1) == GRIB_SUCCESS);
    ECCODES_ASSERT( lVal == 42 );

    ECCODES_ASSERT( string_to_long("-1", &lVal, 1) == GRIB_SUCCESS);
    ECCODES_ASSERT( lVal == -1 );
    ECCODES_ASSERT( string_to_long("+999", &lVal, 1) == GRIB_SUCCESS);
    ECCODES_ASSERT( lVal == 999 );

    ECCODES_ASSERT( string_to_long("15MB", &lVal, 0) == GRIB_SUCCESS);
    ECCODES_ASSERT( lVal == 15 );

    // illegal cases
    ECCODES_ASSERT( string_to_long("4000000000000000000000", &lVal, 1) == GRIB_INVALID_ARGUMENT);
    ECCODES_ASSERT( string_to_long("XY", &lVal, 1) == GRIB_INVALID_ARGUMENT);
    ECCODES_ASSERT( string_to_long("A6", &lVal, 1) == GRIB_INVALID_ARGUMENT);
    ECCODES_ASSERT( string_to_long("5K", &lVal, 1) == GRIB_INVALID_ARGUMENT);
}

static void test_string_replace_char()
{
    printf("Running %s ...\n", __func__);

    char input[32] = {0,};
    strncpy(input, "Mask Of Zoro", sizeof(input));
    string_replace_char(input, ' ', '-');
    ECCODES_ASSERT(STR_EQUAL(input, "Mask-Of-Zoro"));
}

static void test_string_remove_char()
{
    printf("Running %s ...\n", __func__);
    char input[64] = "a:b:c";
    string_remove_char(input, ':');
    ECCODES_ASSERT(STR_EQUAL(input, "abc"));
}

static void test_gribex_mode()
{
    grib_context* c = grib_context_get_default();
    printf("Running %s ...\n", __func__);

    ECCODES_ASSERT( grib_get_gribex_mode(c) == 0 ); /* default is OFF */
    grib_gribex_mode_on(c);
    codes_gribex_mode_on(c);
    ECCODES_ASSERT( grib_get_gribex_mode(c) == 1 );
    grib_gribex_mode_off(c);
    codes_gribex_mode_off(c);
    ECCODES_ASSERT( grib_get_gribex_mode(c) == 0 );
    ECCODES_ASSERT( codes_get_gribex_mode(c) == 0 );
}

static void test_gts_header_mode()
{
    grib_context* c = grib_context_get_default();
    printf("Running %s ...\n", __func__);

    grib_gts_header_on(c);
    codes_gts_header_on(c);
    ECCODES_ASSERT(c->gts_header_on == 1);
    grib_gts_header_off(c);
    codes_gts_header_off(c);
    ECCODES_ASSERT(c->gts_header_on == 0);
}

static void test_data_quality_checks()
{
    grib_context* c = grib_context_get_default();
    printf("Running %s ...\n", __func__);

    grib_context_set_data_quality_checks(c, 1);//warning
    grib_context_set_data_quality_checks(c, 2);//error
    grib_context_set_data_quality_checks(c, 0);//no checks
}

static void test_bufr_multi_element_constant_arrays()
{
    grib_context* c = grib_context_get_default();
    printf("Running %s ...\n", __func__);

    codes_bufr_multi_element_constant_arrays_on(c);
    codes_bufr_multi_element_constant_arrays_off(c);
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
    ECCODES_ASSERT(idx_lower == 1 && idx_upper == 2);
    grib_binary_search(array_asc, idx_asc_max, 56.1, &idx_upper, &idx_lower);
    ECCODES_ASSERT(idx_lower == 2 && idx_upper == 3);
    grib_binary_search(array_asc, idx_asc_max, -0.1, &idx_upper, &idx_lower);
    ECCODES_ASSERT(idx_lower == 0 && idx_upper == 1);

    grib_binary_search(array_desc, idx_desc_max, 88, &idx_upper, &idx_lower);
    ECCODES_ASSERT(idx_lower == 0 && idx_upper == 1);
    grib_binary_search(array_desc, idx_desc_max, -88, &idx_upper, &idx_lower);
    ECCODES_ASSERT(idx_lower == 2 && idx_upper == 3);
    grib_binary_search(array_desc, idx_desc_max, 1, &idx_upper, &idx_lower);
    ECCODES_ASSERT(idx_lower == 1 && idx_upper == 2);
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
    ECCODES_ASSERT( !err );
    ECCODES_ASSERT( count == 2 );
    grib_print_values("print values test: values1", values1, stdout, count);

    ECCODES_ASSERT( strcmp(values1[0].name, "key1")==0 );
    ECCODES_ASSERT( strcmp(values1[0].string_value, "value1")==0 );
    ECCODES_ASSERT( values1[0].equal == 1 );
    ECCODES_ASSERT( strcmp(values1[1].name, "key2")==0 );
    ECCODES_ASSERT( strcmp(values1[1].string_value, "value2")==0 );
    ECCODES_ASSERT( values1[1].equal == 0 );
    /* Note how the input is modified by the tokenizer (thanks to strtok_r) */
    ECCODES_ASSERT( strcmp(input1, "key1=value1")==0 );
    free( (void*)values1[0].name );
    free( (void*)values1[1].name );
    free( (void*)values1[0].string_value );
    free( (void*)values1[1].string_value );

    count = max_count;
    err = parse_keyval_string(NULL, input2,
                              values_required, GRIB_TYPE_LONG, values2, &count);
    ECCODES_ASSERT( !err );
    ECCODES_ASSERT( count == 1 );
    grib_print_values("print values test: values2", values2, stdout, count);
    ECCODES_ASSERT( strcmp(values2[0].name, "x")==0 );
    ECCODES_ASSERT( values2[0].long_value == 14 );
    ECCODES_ASSERT( values2[0].equal == 1 );
    free( (void*)values2[0].name );

    count = max_count;
    err = parse_keyval_string(NULL, input3,
                              values_required, GRIB_TYPE_DOUBLE, values3, &count);
    ECCODES_ASSERT( !err );
    ECCODES_ASSERT( count == 1 );
    grib_print_values("print values test: values3", values3, stdout, count);
    ECCODES_ASSERT( strcmp(values3[0].name, "mars.level")==0 );
    free( (void*)values3[0].name );
}

static void test_time_conversions()
{
    printf("Running %s ...\n", __func__);
    long result = convert_to_minutes(120, 13); // 120s = 2mins
    ECCODES_ASSERT(result == 2);
}

static void test_dates()
{
    printf("Running %s ...\n", __func__);

    ECCODES_ASSERT( is_date_valid(1979,12, 1, 0,0,0) );
    ECCODES_ASSERT( is_date_valid(1900, 1, 1, 0,0,0) );
    ECCODES_ASSERT( is_date_valid(1964, 4, 6, 0,0,0) );
    ECCODES_ASSERT( is_date_valid(2023, 3, 4, 0,0,0) );
    ECCODES_ASSERT( is_date_valid(2023, 3, 4, 12,0,0) );
    ECCODES_ASSERT( is_date_valid(2023, 3, 4, 0,10,0) );
    ECCODES_ASSERT( is_date_valid(2023, 3, 4, 0,0,59) );
    ECCODES_ASSERT( is_date_valid(0000, 3, 4, 0,0,0) );
    ECCODES_ASSERT( is_date_valid(2020, 2, 29, 0,0,0) );//leap year

    ECCODES_ASSERT( !is_date_valid(  10, -1, 1, 0,0,0) );// bad months
    ECCODES_ASSERT( !is_date_valid(1900, 0,  1, 0,0,0) );
    ECCODES_ASSERT( !is_date_valid(1900, 13, 1, 0,0,0) );

    ECCODES_ASSERT( !is_date_valid(1900, 5,  0, 0,0,0) ); // bad days
    ECCODES_ASSERT( !is_date_valid(2000, 5, 32, 0,0,0) );
    ECCODES_ASSERT( !is_date_valid(2000, 5, -7, 0,0,0) );

    ECCODES_ASSERT( !is_date_valid(2000, 5, 8, 99,0,0) );//bad hours
    ECCODES_ASSERT( !is_date_valid(2000, 5, 9, -1,0,0) );

    ECCODES_ASSERT( !is_date_valid(2000, 5, 8, 0, 61,0) );//bad mins
    ECCODES_ASSERT( !is_date_valid(2000, 5, 9, 0,-1, 0) );

    ECCODES_ASSERT( !is_date_valid(2000, 5, 8, 0, 1, -1) );//bad secs
    ECCODES_ASSERT( !is_date_valid(2000, 5, 9, 0, 1, 60) );

    ECCODES_ASSERT( !is_date_valid(2023, 2, 29, 0,0,0) );//Feb
}

void test_scale_factor_scaled_values()
{
    printf("Running %s ...\n", __func__);

    int err =0;
    int64_t value, factor;
    const int64_t scaled_value_max = 4294967295; // usually 4 octets
    const int64_t scale_factor_max = 255; // usually 1 octet

    err = compute_scaled_value_and_scale_factor(0, scaled_value_max, scale_factor_max, &value, &factor);
    ECCODES_ASSERT(!err);
    ECCODES_ASSERT(value == 0);
    ECCODES_ASSERT(factor == 0);

    err = compute_scaled_value_and_scale_factor(1, scaled_value_max, scale_factor_max, &value, &factor);
    ECCODES_ASSERT(!err);
    ECCODES_ASSERT(value == 1);
    ECCODES_ASSERT(factor == 0);

    err = compute_scaled_value_and_scale_factor(1.5, scaled_value_max, scale_factor_max, &value, &factor);
    ECCODES_ASSERT(!err);
    ECCODES_ASSERT(value == 15);
    ECCODES_ASSERT(factor == 1);

    err = compute_scaled_value_and_scale_factor(4.56, scaled_value_max, scale_factor_max, &value, &factor);
    ECCODES_ASSERT(!err);
    ECCODES_ASSERT(value == 456);
    ECCODES_ASSERT(factor == 2);

    err = compute_scaled_value_and_scale_factor(-0.003, scaled_value_max, scale_factor_max, &value, &factor);
    ECCODES_ASSERT(!err);
    ECCODES_ASSERT(value == -3);
    ECCODES_ASSERT(factor == 3);

    err = compute_scaled_value_and_scale_factor(145.889, scaled_value_max, scale_factor_max, &value, &factor);
    ECCODES_ASSERT(!err);
    ECCODES_ASSERT(value == 145889);
    ECCODES_ASSERT(factor == 3);

    err = compute_scaled_value_and_scale_factor(1111.00009, scaled_value_max, scale_factor_max, &value, &factor);
    ECCODES_ASSERT(!err);
    ECCODES_ASSERT(value == 111100009);
    ECCODES_ASSERT(factor == 5);
}

void test_iarray()
{
    printf("Running %s ...\n", __func__);

    grib_iarray* a = grib_iarray_new(10, 10);
    grib_iarray_push(a, 42);
    grib_iarray_push(a, 10000);
    grib_iarray_print("iarray", a);

    grib_iarray* b = grib_iarray_new(1, 1);
    grib_iarray_push(b, 0);
    grib_iarray_push(b, -1);
    grib_iarray_push(b, +1);

    grib_viarray* va = grib_viarray_new(1, 1);
    grib_viarray_push(va, a);
    grib_viarray_push(va, b);
    grib_viarray_print("viarray", va);

    grib_iarray_delete(a);
    grib_iarray_delete(b);
    grib_viarray_delete(va);
}

void test_darray()
{
    printf("Running %s ...\n", __func__);

    grib_darray* a = grib_darray_new(10, 10);
    grib_darray_push(a, 42.009);
    grib_darray_push(a, -1.11);
    grib_darray_push(a, 5099);
    grib_darray_print("darray", a);

    grib_darray* b = grib_darray_new(5, 1);
    grib_darray_push(b, 8);
    grib_darray_push(b, 12);

    grib_vdarray* va = grib_vdarray_new(1, 1);
    grib_vdarray_push(va, a);
    grib_vdarray_push(va, b);
    grib_vdarray_print("vdarray", va);

    grib_darray_delete(a);
    grib_darray_delete(b);
    grib_vdarray_delete(va);
}

void test_sarray()
{
    printf("Running %s ...\n", __func__);

    grib_sarray* a = grib_sarray_new(10, 10);

    char ants_s[] = "ants";
    char bugs_s[] = "bugs";
    grib_sarray_push(a, ants_s);
    grib_sarray_push(a, bugs_s);
    grib_sarray_print("sarray", a);

    grib_vsarray* va = grib_vsarray_new(1, 1);
    grib_vsarray_push(va, a);
    grib_vsarray_print("vsarray", va);

    grib_sarray_delete(a);
    grib_vsarray_delete(va);
}

void test_codes_get_product_name()
{
    printf("Running %s ...\n", __func__);
    ECCODES_ASSERT( STR_EQUAL("ANY",   codes_get_product_name(PRODUCT_ANY)) );
    ECCODES_ASSERT( STR_EQUAL("GRIB",  codes_get_product_name(PRODUCT_GRIB)) );
    ECCODES_ASSERT( STR_EQUAL("BUFR",  codes_get_product_name(PRODUCT_BUFR)) );
    ECCODES_ASSERT( STR_EQUAL("GTS",   codes_get_product_name(PRODUCT_GTS)) );
    ECCODES_ASSERT( STR_EQUAL("METAR", codes_get_product_name(PRODUCT_METAR)) );
    ECCODES_ASSERT( STR_EQUAL("TAF",   codes_get_product_name(PRODUCT_TAF)) );
}

void test_codes_get_type_name()
{
    printf("Running %s ...\n", __func__);
    ECCODES_ASSERT( STR_EQUAL("long",    codes_get_type_name(GRIB_TYPE_LONG)) );
    ECCODES_ASSERT( STR_EQUAL("string",  grib_get_type_name(GRIB_TYPE_STRING)) );
    ECCODES_ASSERT( STR_EQUAL("double",  grib_get_type_name(GRIB_TYPE_DOUBLE)) );
    ECCODES_ASSERT( STR_EQUAL("bytes",   grib_get_type_name(GRIB_TYPE_BYTES)) );
    ECCODES_ASSERT( STR_EQUAL("label",   grib_get_type_name(GRIB_TYPE_LABEL)) );
    ECCODES_ASSERT( STR_EQUAL("section", grib_get_type_name(GRIB_TYPE_SECTION)) );
}

void test_grib_surface_type_requires_value()
{
    printf("Running %s ...\n", __func__);
    int err = 0;
    int edition = 2;
    ECCODES_ASSERT( codes_grib_surface_type_requires_value(edition, 103, &err) == 1 && !err );
    ECCODES_ASSERT( codes_grib_surface_type_requires_value(edition, 160, &err) == 1 && !err );
    ECCODES_ASSERT( codes_grib_surface_type_requires_value(edition, 10,  &err) == 0 && !err );

    ECCODES_ASSERT( codes_grib_surface_type_requires_value(edition, -1, &err) == 0 );
    ECCODES_ASSERT( err == GRIB_INVALID_ARGUMENT);
    ECCODES_ASSERT( codes_grib_surface_type_requires_value(edition, 300, &err) == 0 );
    ECCODES_ASSERT( err == GRIB_INVALID_ARGUMENT);

    // not implemented for edition 1
    edition = 1;
    ECCODES_ASSERT( codes_grib_surface_type_requires_value(edition, 1, &err) == 0 );
    ECCODES_ASSERT( err == GRIB_NOT_IMPLEMENTED );
}

void test_grib2_choose_PDTN()
{
    printf("Running %s ...\n", __func__);
    int det = true;
    int instant = true;

    ECCODES_ASSERT( 0  == grib2_choose_PDTN(0,  det,  instant) );
    ECCODES_ASSERT( 8  == grib2_choose_PDTN(0,  det, !instant) );
    ECCODES_ASSERT( 1  == grib2_choose_PDTN(0, !det,  instant) );
    ECCODES_ASSERT( 11 == grib2_choose_PDTN(0, !det, !instant) );
}

void test_grib2_select_PDTN()
{
    printf("Running %s ...\n", __func__);
    int eps = 1;
    int instant = 1;
    int chemical = 1;
    int chemical_srcsink = 1;
    int chemical_distfn = 1;
    int aerosol = 1;
    int aerosol_optical = 1;

    // arguments = eps instant chemical chemical_srcsink chemical_distfn aerosol aerosol_optical

    // Chemicals
    ECCODES_ASSERT( 40 == grib2_select_PDTN(!eps, instant,  chemical, 0, 0, 0, 0) );
    ECCODES_ASSERT( 41 == grib2_select_PDTN(eps,  instant,  chemical, 0, 0, 0, 0) );
    ECCODES_ASSERT( 42 == grib2_select_PDTN(!eps, !instant, chemical, 0, 0, 0, 0) );
    ECCODES_ASSERT( 43 == grib2_select_PDTN(eps,  !instant, chemical, 0, 0, 0, 0) );

    // Chemical source/sink
    ECCODES_ASSERT( 76 == grib2_select_PDTN(!eps, instant,  !chemical, chemical_srcsink, 0,0,0) );
    ECCODES_ASSERT( 77 == grib2_select_PDTN(eps,  instant,  !chemical, chemical_srcsink, 0,0,0) );
    ECCODES_ASSERT( 78 == grib2_select_PDTN(!eps, !instant, !chemical, chemical_srcsink, 0,0,0) );
    ECCODES_ASSERT( 79 == grib2_select_PDTN(eps,  !instant, !chemical, chemical_srcsink, 0,0,0) );

    // Chemical distrib. function
    ECCODES_ASSERT( 58 == grib2_select_PDTN(eps, instant,  0, 0, chemical_distfn, 0, 0) );
    ECCODES_ASSERT( 68 == grib2_select_PDTN(eps, !instant,  0, 0, chemical_distfn, 0, 0) );
    ECCODES_ASSERT( 57 == grib2_select_PDTN(!eps, instant,  0, 0, chemical_distfn, 0, 0) );
    ECCODES_ASSERT( 67 == grib2_select_PDTN(!eps, !instant,  0, 0, chemical_distfn, 0, 0) );

    // Aerosols
    ECCODES_ASSERT( 50 == grib2_select_PDTN(!eps, instant,  0, 0, 0, aerosol, 0) );
    ECCODES_ASSERT( 46 == grib2_select_PDTN(!eps, !instant, 0, 0, 0, aerosol, 0) );
    ECCODES_ASSERT( 45 == grib2_select_PDTN(eps, instant,   0, 0, 0, aerosol, 0) );
    ECCODES_ASSERT( 85 == grib2_select_PDTN(eps, !instant,  0, 0, 0, aerosol, 0) );

    // Aerosol optical
    ECCODES_ASSERT( 49 == grib2_select_PDTN(eps, instant,  0, 0, 0, 0, aerosol_optical) );
    ECCODES_ASSERT( 48 == grib2_select_PDTN(!eps, instant, 0, 0, 0, 0, aerosol_optical) );

    // Plain vanilla
    ECCODES_ASSERT(  0 == grib2_select_PDTN(!eps, instant,  !chemical, !chemical_srcsink, !chemical_distfn, !aerosol, 0) );
    ECCODES_ASSERT(  1 == grib2_select_PDTN(1,1,0,0,0, !aerosol,0) );
    ECCODES_ASSERT(  8 == grib2_select_PDTN(0,0,0,0,0, !aerosol,0) );
    ECCODES_ASSERT( 11 == grib2_select_PDTN(1,0,0,0,0, !aerosol,0) );

    //printf("%d\n", grib2_select_PDTN(!eps, instant,  !chemical, !chemical_srcsink, !chemical_distfn, aerosol, 0) );
    ECCODES_ASSERT( 1 == grib2_is_PDTN_EPS(1) );
    ECCODES_ASSERT( 1 == grib2_is_PDTN_EPS(11) );
    ECCODES_ASSERT( 0 == grib2_is_PDTN_EPS(0) );
}

void test_codes_get_error_message()
{
    printf("Running %s ...\n", __func__);
    const char* errmsg = grib_get_error_message(6666);
    ECCODES_ASSERT( STR_EQUAL(errmsg, "Unknown error -6666"));
}

void test_codes_context_set_debug()
{
    printf("Running %s ...\n", __func__);
    grib_context* context = NULL;
    int err = 0;

    printf("\tEnable debugging...\n");
    grib_context_set_debug(context, -1);

    grib_handle* h = grib_handle_new_from_samples(context, "GRIB2");
    if (!h) return;
    err = grib_set_long(h, "paramId", 167);
    ECCODES_ASSERT(!err);

    printf("\tDisable debugging...\n");
    grib_context_set_debug(context, 0);

    err = grib_set_long(h, "edition", 1);
    ECCODES_ASSERT(!err);
    printf("\tEnable debugging again (verbose)...\n");
    grib_context_set_debug(context, 1);
    grib_handle_delete(h);

    grib_context_set_debug(context, 0);
}

void test_codes_is_feature_enabled()
{
    printf("Running %s ...\n", __func__);
    const char* features[] = {
        "AEC",
        "MEMFS",
        "JPG",
        "PNG",
        "ECCODES_THREADS",
        "ECCODES_OMP_THREADS",
        "NETCDF",
        "FORTRAN",
        "GEOGRAPHY",
        NULL};
    for (int i = 0; features[i]; ++i) {
        const char* f = features[i];
        printf("\tFeature %s enabled?\t%d\n", f, codes_is_feature_enabled(f));
    }
    ECCODES_ASSERT( 0 == codes_is_feature_enabled("total rubbish") );
}

void test_codes_get_features()
{
    printf("Running %s ...\n", __func__);

    size_t len = 512;
    char* features = (char*)calloc(len, sizeof(char));
    int err = codes_get_features(features, &len, CODES_FEATURES_ALL);
    ECCODES_ASSERT(!err);
    printf("\tFeatures are: '%s'\n", features);

    len = 512;
    err = codes_get_features(features, &len, CODES_FEATURES_ENABLED);
    printf("\tEnabled features are: '%s'\n", features);

    len = 512;
    err = codes_get_features(features, &len, CODES_FEATURES_DISABLED);
    printf("\tDisabled features are: '%s'\n", features);

    free(features);
}

static void test_grib_get_binary_scale_fact()
{
    printf("Running %s ...\n", __func__);
    int err = 0;
    long result = grib_get_binary_scale_fact(INFINITY, 0, 0, &err);
    ECCODES_ASSERT(err == GRIB_OUT_OF_RANGE);
    ECCODES_ASSERT(result == 0);

    result = grib_get_binary_scale_fact(100, 0, 65, &err); // bpv too big
    ECCODES_ASSERT(err == GRIB_OUT_OF_RANGE);
    ECCODES_ASSERT(result == 0);

    result = grib_get_binary_scale_fact(100, 0, 0, &err); // bpv 0
    ECCODES_ASSERT(err == GRIB_ENCODING_ERROR);
    ECCODES_ASSERT(result == 0);
}

static void test_filepool()
{
    printf("Running %s ...\n", __func__);
    grib_file_pool_print("file_pool contents", stdout);
}

static void test_expressions()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();

    grib_expression* eTrue = new_true_expression(c);
    ECCODES_ASSERT(eTrue);
    const char* cname = eTrue->class_name();
    ECCODES_ASSERT( cname && strlen(cname) > 0 );

    grib_expression* eUnOp = new_unop_expression(c, 0, 0, 0);
    ECCODES_ASSERT(eUnOp);
    cname = eUnOp->class_name();
    ECCODES_ASSERT( cname && strlen(cname) > 0 );

    grib_expression* eBinop = new_binop_expression(c, 0, 0, 0, 0);
    ECCODES_ASSERT(eBinop);
    cname = eBinop->class_name();
    ECCODES_ASSERT( cname && strlen(cname) > 0 );

    grib_expression* eSubStr = new_sub_string_expression(c, "atest", 0, 1);
    ECCODES_ASSERT(eSubStr);
    cname = eSubStr->class_name();
    ECCODES_ASSERT( cname && strlen(cname) > 0 );

    grib_expression* eLen = new_length_expression(c, "abc");
    ECCODES_ASSERT(eLen);
    cname = eLen->class_name();
    ECCODES_ASSERT( cname && strlen(cname) > 0 );

    grib_expression* eIsInt = new_is_integer_expression(c, "42", 0, 1);
    ECCODES_ASSERT(eIsInt);
    cname = eIsInt->class_name();
    ECCODES_ASSERT( cname && strlen(cname) > 0 );

    grib_expression* eStrCmp = new_string_compare_expression(c, 0, 0, 0);
    ECCODES_ASSERT(eStrCmp);
    cname = eStrCmp->class_name();
    ECCODES_ASSERT( cname && strlen(cname) > 0 );

    grib_expression* eLogAnd = new_logical_and_expression(c, 0, 0);
    ECCODES_ASSERT(eLogAnd);
    cname = eLogAnd->class_name();
    ECCODES_ASSERT( cname && strlen(cname) > 0 );

    grib_expression* eLogOr = new_logical_or_expression(c, 0 ,0);
    ECCODES_ASSERT(eLogOr);
    cname = eLogOr->class_name();
    ECCODES_ASSERT( cname && strlen(cname) > 0 );

    grib_expression* eIsInDict = new_is_in_dict_expression(c, "a", "list");
    ECCODES_ASSERT(eIsInDict);
    cname = eIsInDict->class_name();
    ECCODES_ASSERT( cname && strlen(cname) > 0 );

    grib_expression* eIsInList = new_is_in_list_expression(c, "b", "list");
    ECCODES_ASSERT(eIsInList);
    cname = eIsInList->class_name();
    ECCODES_ASSERT( cname && strlen(cname) > 0 );

}

int main(int argc, char** argv)
{
    printf("Doing unit tests. ecCodes version = %ld\n", grib_get_api_version());
    printf("codes_print_api_version gives: ");
    codes_print_api_version(stdout);
    printf("\n");

    test_grib_get_reduced_row_legacy();

    test_codes_context_set_debug();
    test_codes_get_error_message();
    test_grib_get_binary_scale_fact();

    test_iarray();
    test_darray();
    test_sarray();

    test_codes_get_product_name();
    test_codes_get_type_name();

    test_scale_factor_scaled_values();
    test_dates();
    test_time_conversions();
    test_logging_proc();
    test_print_proc();
    test_grib_binary_search();
    test_parse_keyval_string();

    test_get_git_sha1();
    test_get_package_name();
    test_get_build_date();
    test_gribex_mode();
    test_gts_header_mode();
    test_bufr_multi_element_constant_arrays();
    test_data_quality_checks();

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

    test_grib_surface_type_requires_value();
    test_grib2_select_PDTN();
    test_grib2_choose_PDTN();
    test_codes_is_feature_enabled();
    test_codes_get_features();
    test_filepool();
    test_expressions();

    printf("\n\nProgram %s finished\n", argv[0]);
    return 0;
}
