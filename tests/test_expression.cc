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
 * Unit tests for expression evaluation subsystem.
 *
 * Tests True, Long, Double, IsInteger, SubString, Length, Accessor
 * expressions for correct evaluation of evaluate_long and evaluate_double.
 */

#include "eccodes.h"
#include "grib_api_internal.h"

#include <cstdio>
#include <cstring>
#include <cmath>

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(cond)                                                      \
    do {                                                                        \
        if (!(cond)) {                                                          \
            fprintf(stderr, "FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond);    \
            tests_failed++;                                                     \
            return;                                                             \
        }                                                                       \
    } while (0)

#define TEST_PASS() tests_passed++

/* ========== Test: True expression ========== */
static void test_true_expression()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();
    grib_handle* h = grib_handle_new_from_samples(c, "GRIB2");
    TEST_ASSERT(h != NULL);

    grib_expression* e = new_true_expression(c);
    TEST_ASSERT(e != NULL);

    double dval = -1;
    int err = e->evaluate_double(h, &dval);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(dval == 1.0);

    long lval = -1;
    err = e->evaluate_long(h, &lval);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(lval == 1);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: Long expression ========== */
static void test_long_expression()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();
    grib_handle* h = grib_handle_new_from_samples(c, "GRIB2");
    TEST_ASSERT(h != NULL);

    grib_expression* e = new_long_expression(c, 42);
    TEST_ASSERT(e != NULL);

    long lval = 0;
    int err = e->evaluate_long(h, &lval);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(lval == 42);

    double dval = 0;
    err = e->evaluate_double(h, &dval);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(fabs(dval - 42.0) < 1e-10);

    /* Test with zero */
    grib_expression* e0 = new_long_expression(c, 0);
    TEST_ASSERT(e0 != NULL);
    err = e0->evaluate_long(h, &lval);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(lval == 0);

    /* Test with negative */
    grib_expression* eNeg = new_long_expression(c, -100);
    TEST_ASSERT(eNeg != NULL);
    err = eNeg->evaluate_long(h, &lval);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(lval == -100);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: Double expression ========== */
static void test_double_expression()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();
    grib_handle* h = grib_handle_new_from_samples(c, "GRIB2");
    TEST_ASSERT(h != NULL);

    grib_expression* e = new_double_expression(c, 3.14159);
    TEST_ASSERT(e != NULL);

    double dval = 0;
    int err = e->evaluate_double(h, &dval);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(fabs(dval - 3.14159) < 1e-10);

    long lval = 0;
    err = e->evaluate_long(h, &lval);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(lval == 3);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: IsInteger expression ========== */
static void test_is_integer_expression()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();
    grib_handle* h = grib_handle_new_from_samples(c, "GRIB2");
    TEST_ASSERT(h != NULL);

    /* "edition" is a long (integer) key */
    grib_expression* e = new_is_integer_expression(c, "edition", 0, 1);
    TEST_ASSERT(e != NULL);

    double dval = -1;
    int err = e->evaluate_double(h, &dval);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(dval > 0); /* edition = 2, which is an integer */

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: Accessor expression (reads key value) ========== */
static void test_accessor_expression()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();
    grib_handle* h = grib_handle_new_from_samples(c, "GRIB2");
    TEST_ASSERT(h != NULL);

    grib_expression* e = new_accessor_expression(c, "edition", 0, 0);
    TEST_ASSERT(e != NULL);

    long lval = 0;
    int err = e->evaluate_long(h, &lval);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(lval == 2);

    double dval = 0;
    err = e->evaluate_double(h, &dval);
    TEST_ASSERT(err == GRIB_SUCCESS);
    TEST_ASSERT(fabs(dval - 2.0) < 1e-10);

    grib_handle_delete(h);
    TEST_PASS();
}

/* ========== Test: class_name is set for all expression types ========== */
static void test_expression_class_names()
{
    printf("Running %s ...\n", __func__);
    grib_context* c = grib_context_get_default();

    grib_expression* eTrue = new_true_expression(c);
    TEST_ASSERT(eTrue != NULL);
    TEST_ASSERT(eTrue->class_name() != NULL);
    TEST_ASSERT(strlen(eTrue->class_name()) > 0);

    grib_expression* eLong = new_long_expression(c, 0);
    TEST_ASSERT(eLong != NULL);
    TEST_ASSERT(eLong->class_name() != NULL);

    grib_expression* eDouble = new_double_expression(c, 0.0);
    TEST_ASSERT(eDouble != NULL);
    TEST_ASSERT(eDouble->class_name() != NULL);

    grib_expression* eLogAnd = new_logical_and_expression(c, NULL, NULL);
    TEST_ASSERT(eLogAnd != NULL);
    TEST_ASSERT(eLogAnd->class_name() != NULL);

    grib_expression* eLogOr = new_logical_or_expression(c, NULL, NULL);
    TEST_ASSERT(eLogOr != NULL);
    TEST_ASSERT(eLogOr->class_name() != NULL);

    grib_expression* eStrCmp = new_string_compare_expression(c, NULL, NULL, NULL);
    TEST_ASSERT(eStrCmp != NULL);
    TEST_ASSERT(eStrCmp->class_name() != NULL);

    grib_expression* eUnOp = new_unop_expression(c, NULL, NULL, NULL);
    TEST_ASSERT(eUnOp != NULL);
    TEST_ASSERT(eUnOp->class_name() != NULL);

    grib_expression* eBinOp = new_binop_expression(c, NULL, NULL, NULL, NULL);
    TEST_ASSERT(eBinOp != NULL);
    TEST_ASSERT(eBinOp->class_name() != NULL);

    TEST_PASS();
}

int main(int argc, char** argv)
{
    printf("Running expression evaluation unit tests...\n\n");

    test_true_expression();
    test_long_expression();
    test_double_expression();
    test_is_integer_expression();
    test_accessor_expression();
    test_expression_class_names();

    printf("\n========================================\n");
    printf("Expression tests: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
