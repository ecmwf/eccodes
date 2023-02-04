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

#define EPSILON 1e-12
#define DBL_EQUAL(a, b) (fabs((a) - (b)) <= (EPSILON)*fabs((a)))

#define BOOL int
#define TRUE 1
#define FALSE 0

static BOOL Check(
    const long year1, const long month1, const long day1, const long hour1, const long min1, const long sec1,
    const long year2, const long month2, const long day2, const long hour2, const long min2, const long sec2)
{
    if (year1 != year2 || month1 != month2 || day1 != day2 || hour1 != hour2 || min1 != min2 || sec1 != sec2) {
        return FALSE;
    }
    return TRUE;
}

static void TestDateTime(const long year, const long month, const long day, const long hour, const long min, const long sec)
{
    double jd = 0.0;
    long year1, month1, day1, hour1, min1, sec1; /*generated ones*/
    long jdlong1, jdlong2, date;

    /* Convert the input values to a double */
    grib_datetime_to_julian(year, month, day, hour, min, sec, &jd);

    /* Convert the double back to the input values and compare */
    grib_julian_to_datetime(jd, &year1, &month1, &day1, &hour1, &min1, &sec1);

    if (!Check(year, month, day, hour, min, sec, year1, month1, day1, hour1, min1, sec1)) {
        fprintf(stderr,
                "Failed julian<->datetime\nInput:  year=%ld mon=%ld day=%ld time=%ld:%ld:%ld\n"
                "Output: year=%ld mon=%ld day=%ld time=%ld:%ld:%ld\n",
                year, month, day, hour, min, sec,
                year1, month1, day1, hour1, min1, sec1);
        exit(1);
    }

    /* Now test the conversion from long -> long */
    jdlong1 = (long)(jd + 0.5); /* round up */
    date    = grib_julian_to_date(jdlong1);
    jdlong2 = grib_date_to_julian(date);
    if (jdlong1 != jdlong2) {
        fprintf(stderr, "Failed julian<->date\nInput: year=%ld mon=%ld day=%ld time=%ld:%ld:%ld\n",
                year, month, day, hour, min, sec);
        exit(1);
    }
}

static void Test0()
{
    long year, month, day, hour, min, sec;
    double jd;

    printf("Test0...\n");
    /* Test a specific date */
    year  = 1801;
    month = 1;
    day   = 30;
    hour  = 18;
    min   = 26;
    sec   = 24;

    grib_datetime_to_julian(year, month, day, hour, min, sec, &jd);
    Assert(DBL_EQUAL(jd, 2378891.268333));
    printf("%ld %ld %ld %ld:%ld:%ld -> %f\n", year, month, day, hour, min, sec, jd);

    grib_julian_to_datetime(jd, &year, &month, &day, &hour, &min, &sec);
    Assert(year == 1801);
    Assert(month == 1);
    Assert(day == 30);
    Assert(hour == 18);
    Assert(min == 26);
    Assert(sec == 24);
    printf("%ld %ld %ld %ld:%ld:%ld -> %f\n", year, month, day, hour, min, sec, jd);
}

static void Test1()
{
    long year, month, day, hour, min, sec;
    double jd;

    /* Test a specific date */
    printf("Test1...\n");
    year  = 1957;
    month = 10;
    day   = 4;
    hour  = 19;
    min   = 26;
    sec   = 24;

    grib_datetime_to_julian(year, month, day, hour, min, sec, &jd);
    Assert(DBL_EQUAL(jd, 2436116.31));
    printf("%ld %ld %ld %ld:%ld:%ld -> %f\n", year, month, day, hour, min, sec, jd);

    grib_julian_to_datetime(jd, &year, &month, &day, &hour, &min, &sec);
    Assert(year == 1957);
    Assert(month == 10);
    Assert(day == 4);
    Assert(hour == 19);
    Assert(min == 26);
    Assert(sec == 24);
    printf("%ld %ld %ld %ld:%ld:%ld -> %f\n", year, month, day, hour, min, sec, jd);
}

static void Test2()
{
    long year, month, day, hour, minute, second;
    int i, last;
    double jd;
    long jdl, date;
    /* Test specific julian values */
    double jds[] = {
        2451545.0,
        2446822.5,
        2446966.0,
        2447187.5,
        2447332.0,
        2415020.5,
        2305447.5,
        2305812.5,
        2205512.5,
        2026871.8,
        0.0
    };

    last = sizeof(jds) / sizeof(double);

    printf("\n");
    printf("Test2...\n");
    for (i = 0; i < last; i++) {
        jd = jds[i];
        grib_julian_to_datetime(jd, &year, &month, &day, &hour, &minute, &second);
        printf("+ %ld %ld %ld %ld:%ld:%ld -> %f\n", year, month, day, hour, minute, second, jd);
        grib_datetime_to_julian(year, month, day, hour, minute, second, &jd);
        printf("- %ld %ld %ld %ld:%ld:%ld -> %f\n", year, month, day, hour, minute, second, jd);

        if (!DBL_EQUAL(jd, jds[i])) {
            fprintf(stderr, "i=%d:  Got: %f, expected: %f\n", i, jd, jds[i]);
            Assert(0);
        }

        jdl  = (long)(jd + 0.5);
        date = grib_julian_to_date(jdl);
        printf("+ %ld -> %ld\n", date, jdl);
        jdl = grib_date_to_julian(date);
        printf("- %ld -> %ld\n", date, jdl);
        printf("\n");
    }
}

static void Test3()
{
    long iyear, imnth, iday, ihr, imin, isec;
    printf("Test3...\n");
    for (iyear = 1950; iyear <= 2015; iyear += 5) {
        for (imnth = 1; imnth <= 12; imnth += 1) {
            for (iday = 1; iday <= 28; iday += 1) {
                for (ihr = 1; ihr <= 23; ihr += 2) {
                    for (imin = 1; imin <= 59; imin += 3) {
                        for (isec = 1; isec <= 59; isec += 5) {
                            TestDateTime(iyear, imnth, iday, ihr, imin, isec);
                        }
                    }
                }
            }
        }
    }
}

static void Test4()
{
    const long iyear = 1582;

    TestDateTime(iyear, 9, 1, 1, 0, 0);
    TestDateTime(iyear, 9, 2, 1, 0, 0);
    TestDateTime(iyear, 9, 3, 1, 0, 0);
    TestDateTime(iyear, 9, 4, 1, 0, 0);
    TestDateTime(iyear, 9, 4, 16, 0, 0);

    /* TODO */
    /* TestDateTime(iyear, 10, 5, 1, 0, 0); */
    TestDateTime(iyear, 10, 1, 1, 0, 0);
    TestDateTime(iyear, 10, 2, 1, 0, 0);
    TestDateTime(iyear, 10, 3, 1, 0, 0);
    TestDateTime(iyear, 10, 4, 1, 0, 0);
    TestDateTime(iyear, 10, 15, 1, 0, 0);

    TestDateTime(iyear, 11, 1, 1, 0, 0);
    TestDateTime(iyear, 11, 2, 1, 0, 0);
    TestDateTime(iyear, 11, 3, 1, 0, 0);
    TestDateTime(iyear, 11, 4, 1, 0, 0);
    TestDateTime(iyear, 11, 5, 1, 0, 0);
    TestDateTime(iyear, 11, 5, 15, 0, 0);

    /* TODO
     * for (imnth = 1; imnth <= 12; imnth += 1) {
     *   for (iday = 1; iday <= 28; iday += 1) {
     *     TestDateTime(iyear, imnth, iday, 1, 0, 0);
     *   }
     * }
     */
}

int main(int argc, char* argv[])
{
    Test0();
    Test1();
    Test2();
    Test3();
    Test4();
    printf("All OK\n");
    return 0;
}
