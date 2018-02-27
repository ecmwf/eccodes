/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/* cmake config header */
#ifdef HAVE_ECCODES_CONFIG_H
#include "eccodes_config.h"
#endif

/* autoconf config header */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <grib_api.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>

#define CHECK(a)  check(#a,a)
#define WARN(a)   warn(#a,a)
#define NUMBER(a) (sizeof(a)/sizeof(a[0]))

typedef struct pair {
    const char* key;
    int         key_type;
    long        value_long;
    char*       value_string;
} pair;

typedef struct parameter parameter;

typedef void (*check_proc)(grib_handle*,const parameter*,double min,double max);

struct parameter {
    const char* name;
    double min1;
    double min2;
    double max1;
    double max2;
    pair   pairs[15];
    check_proc checks[5];
};

static void point_in_time(grib_handle*,const parameter*,double,double);
static void statistical_process(grib_handle*,const parameter*,double,double);
static void six_hourly(grib_handle*,const parameter*,double,double);
static void since_prev_pp(grib_handle*,const parameter*,double,double);
static void three_hourly(grib_handle* h,const parameter* p,double min,double max);
static void from_start(grib_handle*,const parameter*,double,double);
static void daily_average(grib_handle*,const parameter*,double,double);
static void given_level(grib_handle*,const parameter*,double,double);
static void predefined_level(grib_handle*,const parameter*,double,double);
static void predefined_thickness(grib_handle*,const parameter*,double,double);
static void given_thickness(grib_handle*,const parameter*,double,double);
static void has_bitmap(grib_handle*,const parameter*,double,double);
static void has_soil_level(grib_handle*,const parameter*,double,double);
static void has_soil_layer(grib_handle*,const parameter*,double,double);

static void height_level(grib_handle*,const parameter*,double,double);
static void pressure_level(grib_handle*,const parameter*,double,double);
static void potential_temperature_level(grib_handle*,const parameter*,double,double);
static void potential_vorticity_level(grib_handle*,const parameter*,double,double);

/* 
TODO: 
- Shape of the earth
- Levels
 */

#include "tigge_check.h"


int error = 0;
const char* file = 0;
int field = 0;
int warning = 0;
int valueflg = 0;
const char* param = "unknown";
int warnflg = 0;
int zeroflg = 0;
int is_lam = 0;
int is_s2s = 0;
int is_s2s_refcst = 0;
int is_uerra = 0;

const char* good = NULL;
const char* bad = NULL;

FILE* fgood = NULL;
FILE* fbad  = NULL;

static void check(const char* name,int a)
{
    if(!a) {
        printf("%s, field %d [%s]: %s failed\n",file,field,param,name);
        error++;
    }
}

/*
static void warn(const char* name,int a)
{
    if(!a) {
        printf("%s, field %d [%s]: %s failed\n",file,field,param,name);
        warning++;
    }
}
 */

static void save(grib_handle* h, const char *name,FILE* f)
{
    size_t size;
    const void *buffer;
    int e;

    if(!f) return;

    if((e = grib_get_message(h,&buffer,&size)) != GRIB_SUCCESS)
    {
        printf("%s, field %d [%s]: cannot get message: %s\n",file,field,param,grib_get_error_message(e));
        exit(1);
    }

    if(fwrite(buffer,1,size,f) != size)
    {
        perror(name);
        exit(1);
    }
}

static long get(grib_handle *h,const char* what)
{
    int e; long val;
    if((e = grib_get_long(h,what,&val)) != GRIB_SUCCESS)
    {
        printf("%s, field %d [%s]: cannot get %s: %s\n",file,field,param,what,grib_get_error_message(e));
        error++;
        val = -1;
    }
    return val;
}

static double dget(grib_handle *h,const char* what)
{
    int e; double val;
    if((e = grib_get_double(h,what,&val)) != GRIB_SUCCESS)
    {
        printf("%s, field %d [%s]: cannot get %s: %s\n",file,field,param,what,grib_get_error_message(e));
        error++;
        val = -1;
    }
    return val;
}

static int missing(grib_handle *h,const char* what)
{
    int err=0;
    return grib_is_missing(h,what,&err);
}

static int eq(grib_handle *h,const char* what,long value)
{
    return get(h,what) == value;
}

static int ne(grib_handle *h,const char* what,long value)
{
    return get(h,what) != value;
}

static int ge(grib_handle *h,const char* what,long value)
{
    return get(h,what) >= value;
}

static int le(grib_handle *h,const char* what,long value)
{
    return get(h,what) <= value;
}

static int DBL_EQUAL(double d1, double d2, double tolerance)
{
    return fabs(d1-d2) <= tolerance;
}

static void gaussian_grid(grib_handle* h)
{
    const double tolerance = 1.0/1000000.0; /* angular tolerance for grib2: micro degrees */
    long n = get(h,"numberOfParallelsBetweenAPoleAndTheEquator"); /* This is the key N */
    static double* values = NULL;
    static long last_n = 0;
    double north = dget(h,"latitudeOfFirstGridPointInDegrees");
    double south = dget(h,"latitudeOfLastGridPointInDegrees");

    double west = dget(h,"longitudeOfFirstGridPointInDegrees");
    double east = dget(h,"longitudeOfLastGridPointInDegrees");

    int e;

    if(n != last_n)
    {
        if(values) free(values);

        values = (double*)malloc(2*sizeof(double)*n);
        if(!values)
        {
            printf("%s, field %d [%s]: failed to allocate %ld bytes\n",file,field,param,2*(long)sizeof(double)*(n));
            error++;
            return;
        }

        if((e = grib_get_gaussian_latitudes(n,values)) != GRIB_SUCCESS)
        {
            printf("%s, field %d [%s]: cannot get gaussian latitudes for N%ld: %s\n",file,field,param,n,grib_get_error_message(e));
            error++;
            free(values);
            last_n = 0;
            return;
        }

        last_n = n;
    }

    values[0] = rint(values[0]*1e6)/1e6;

    if ( !DBL_EQUAL(north, values[0], tolerance) || !DBL_EQUAL(south, -values[0], tolerance) )
        printf("N=%ld north=%f south=%f v(=gauss_lat[0])=%f north-v=%0.30f south-v=%0.30f\n",
                n,north,south,values[0],north-values[0],south+values[0]);

    CHECK(DBL_EQUAL(north, values[0], tolerance));
    CHECK(DBL_EQUAL(south, -values[0], tolerance));

    if(missing(h,"numberOfPointsAlongAParallel"))   /* same as key Ni */
    {
        /* If missing, this is a REDUCED gaussian grid */
        const long MAXIMUM_RESOLUTION = 640;
        CHECK(get(h,"PLPresent"));
        CHECK(DBL_EQUAL(west, 0.0, tolerance));
        if (n > MAXIMUM_RESOLUTION) {
            printf("Gaussian number N (=%ld) cannot exceed %ld\n", n, MAXIMUM_RESOLUTION);
            CHECK(n <= MAXIMUM_RESOLUTION);
        }
    }
    else
    {
        /* REGULAR gaussian grid */
        long l_west = get(h,"longitudeOfFirstGridPoint");
        long l_east = get(h,"longitudeOfLastGridPoint");
        long parallel = get(h,"numberOfPointsAlongAParallel");
        long we = get(h,"iDirectionIncrement");
        double dwest  = dget(h,"longitudeOfFirstGridPointInDegrees");
        double deast  = dget(h,"longitudeOfLastGridPointInDegrees");
        double dwe = dget(h,"iDirectionIncrementInDegrees");
        /*printf("parallel=%ld east=%ld west=%ld we=%ld\n",parallel,east,west,we);*/

        CHECK(parallel == (l_east-l_west)/we + 1);
        CHECK(fabs((deast-dwest)/dwe + 1 - parallel) < 1e-10);
        CHECK(!get(h,"PLPresent"));
    }

    CHECK(ne(h,"Nj",0));

    if(get(h,"PLPresent")) {
        size_t count, i, nPl;
        int err_code = grib_get_size(h,"pl",&count);
        double *pl;
        double expected_lon2 = 0;
        long total, max_pl = 0;
        long numberOfValues = get(h,"numberOfValues");
        long numberOfDataPoints = get(h,"numberOfDataPoints");

        if(err_code) {
            printf("%s, field %d [%s]: cannot number of pl: %s\n",file,field,param,grib_get_error_message(err_code));
            error++;
            return;
        }

        pl = (double*)malloc(sizeof(double)*(count));
        CHECK(pl != NULL);

        nPl = count;
        if((err_code =  grib_get_double_array(h,"pl",pl,&count)))
        {
            printf("%s, field %d [%s]: cannot get pl: %s\n",file,field,param,grib_get_error_message(err_code));
            free(pl);
            error++;
            return;
        }
        if(nPl != count)
            printf("nPl=%ld count=%ld\n",(long)nPl,(long)count);

        CHECK(nPl == count);
        CHECK(nPl == (size_t)2*n);

        total = 0;
        max_pl = pl[0]; /* max elem of pl array = num points at equator */
        for(i = 0 ; i < count; i++) {
            total += pl[i];
            if (pl[i] > max_pl) max_pl = pl[i];
        }

        free(pl);

        /* Do not assume maximum of pl array is 4N! not true for octahedral */
        expected_lon2 = 360.0 - 360.0/max_pl;
        if (!DBL_EQUAL(expected_lon2, east, tolerance))
            printf("east actual=%g expected=%g diff=%g\n",east, expected_lon2, expected_lon2-east);
        CHECK(DBL_EQUAL(expected_lon2, east, tolerance));

        if(numberOfDataPoints != total)
            printf("GAUSS numberOfValues=%ld numberOfDataPoints=%ld sum(pl)=%ld\n",
                    numberOfValues,
                    numberOfDataPoints,
                    total);

        CHECK(numberOfDataPoints == total);

        CHECK(missing(h,"iDirectionIncrement"));
        CHECK(missing(h,"iDirectionIncrementInDegrees"));

        CHECK(eq(h,"iDirectionIncrementGiven",0));
        CHECK(eq(h,"jDirectionIncrementGiven",1));
    }

    CHECK(eq(h,"resolutionAndComponentFlags1",0));
    CHECK(eq(h,"resolutionAndComponentFlags2",0));
    CHECK(eq(h,"resolutionAndComponentFlags6",0));
    CHECK(eq(h,"resolutionAndComponentFlags7",0));
    CHECK(eq(h,"resolutionAndComponentFlags8",0));
}

static void check_validity_datetime(grib_handle* h)
{
    /* If we just set the stepRange (for non-instantaneous fields) to its
     * current value, then this causes the validity date and validity time
     * keys to be correctly computed.
     * Then we can compare the previous (possibly wrongly coded) value with
     * the newly computed one
     */
    char stepType[15]={0,};
    int err = 0;
    size_t str_len = 100;

    err = grib_get_string(h, "stepType", stepType, &str_len);
    if (err) return;
    if (strcmp(stepType, "instant")!=0) { /* not instantaneous */
        char stepRange[100]={0,};
        long saved_validityDate, saved_validityTime;
        long validityDate, validityTime;

        /* Check only applies to accumulated, max etc. */
        err = grib_get_string(h, "stepRange", stepRange, &str_len);
        if (err) return;

        saved_validityDate = get(h, "validityDate");
        saved_validityTime = get(h, "validityTime");

        err = grib_set_string(h, "stepRange", stepRange, &str_len);
        if (err) return;
        validityDate = get(h, "validityDate");
        validityTime = get(h, "validityTime");

        if (validityDate!=saved_validityDate || validityTime!=saved_validityTime) {
            printf("warning: %s, field %d [%s]: invalid validity Date/Time (Should be %ld and %ld)\n",
                    file,field,param, validityDate, validityTime);
            warning++;
        }
    }
}

static void check_range(grib_handle* h,const parameter* p,double min,double max)
{
    double missing = 0;
    if(!valueflg)
        return;

    missing = dget(h,"missingValue");

    /* See ECC-437 */
    if(!(get(h,"bitMapIndicator") == 0 && min == missing && max == missing)){

        if(min < p->min1 || min > p->min2)
        {
            printf("warning: %s, field %d [%s]: %s minimum value %g is not in [%g,%g]\n",file,field,param,
                    p->name,
                    min,p->min1,p->min2);
            printf("  => [%g,%g]\n",min < p->min1 ? min : p->min1, min > p->min2 ? min : p->min2);

            warning++;
        }

        if(max < p->max1 || max > p->max2 )
        {
            printf("warning: %s, field %d [%s]: %s maximum value %g is not in [%g,%g]\n",file,field,param,
                    p->name,
                    max,p->max1,p->max2);
            printf("  => [%g,%g]\n",max < p->max1 ? max : p->max1, max > p->max2 ? max : p->max2);
            warning++;
        }

    }
}

static void point_in_time(grib_handle* h,const parameter* p,double min,double max)
{
    switch(get(h,"typeOfProcessedData"))
    {
    case 0: /* Analysis */
        if (is_uerra)
            CHECK(eq(h,"productDefinitionTemplateNumber",0)||eq(h,"productDefinitionTemplateNumber",1));
        if (get(h,"productDefinitionTemplateNumber") == 1){
            CHECK(ne(h,"numberOfForecastsInEnsemble",0));
            CHECK(le(h,"perturbationNumber",get(h,"numberOfForecastsInEnsemble")));
        }
        break;

    case 1: /* Forecast */
        if (is_uerra)
            CHECK(eq(h,"productDefinitionTemplateNumber",0)||eq(h,"productDefinitionTemplateNumber",1));
        if (get(h,"productDefinitionTemplateNumber") == 1){
            CHECK(ne(h,"numberOfForecastsInEnsemble",0));
            CHECK(le(h,"perturbationNumber",get(h,"numberOfForecastsInEnsemble")));
        }
        break;

    case 2: /* Analysis and forecast products */
        CHECK(eq(h,"productDefinitionTemplateNumber",0));
        break;

    case 3: /* Control forecast products */
        CHECK(eq(h,"perturbationNumber",0));
        CHECK(ne(h,"numberOfForecastsInEnsemble",0));
        if (is_s2s_refcst)
            CHECK(eq(h,"productDefinitionTemplateNumber",60));
        else if (is_s2s)
            /*CHECK(eq(h,"productDefinitionTemplateNumber",60)||eq(h,"productDefinitionTemplateNumber",11)||eq(h,"productDefinitionTemplateNumber",1));*/
            CHECK(eq(h,"productDefinitionTemplateNumber",1));
        else
            CHECK(eq(h,"productDefinitionTemplateNumber",1));
        break;

    case 4: /* Perturbed forecast products */
        CHECK(ne(h,"perturbationNumber",0));
        CHECK(ne(h,"numberOfForecastsInEnsemble",0));
        if (is_s2s_refcst)
            CHECK(eq(h,"productDefinitionTemplateNumber",60));
        else if (is_s2s)
            /*CHECK(eq(h,"productDefinitionTemplateNumber",60)||eq(h,"productDefinitionTemplateNumber",11)||eq(h,"productDefinitionTemplateNumber",1));*/
            CHECK(eq(h,"productDefinitionTemplateNumber",1));
        else
            CHECK(eq(h,"productDefinitionTemplateNumber",1));
        if (is_lam) {
            CHECK(le(h,"perturbationNumber",get(h,"numberOfForecastsInEnsemble")));
        } else {
            /* Is there always cf in tigge global datasets?? */
            CHECK(le(h,"perturbationNumber",get(h,"numberOfForecastsInEnsemble")-1));
        }
        break;

    default:
        printf("Unsupported typeOfProcessedData %ld\n",get(h,"typeOfProcessedData"));
        CHECK(0);
        break;
    }

    if (is_lam) {
        if(get(h,"indicatorOfUnitOfTimeRange") == 10 ) /*  three hours */
        {
            /* Three hourly is OK */
            ;
        }
        else
        {
            CHECK(eq(h,"indicatorOfUnitOfTimeRange",1));/* Hours */
            CHECK((get(h,"forecastTime") % 3) == 0);  /* Every three hours */
        }
    } 
    else if (is_uerra) {
        if(get(h,"indicatorOfUnitOfTimeRange") == 1) /*  hourly */
        {
            CHECK((eq(h,"forecastTime",1)||eq(h,"forecastTime",2)||eq(h,"forecastTime",4)||eq(h,"forecastTime",5))||(get(h,"forecastTime") % 3) == 0);
        }
    }
    else {
        if(get(h,"indicatorOfUnitOfTimeRange") == 11) /*  six hours */
        {
            /* Six hourly is OK */
            ;
        }
        else
        {
            CHECK(eq(h,"indicatorOfUnitOfTimeRange",1));/* Hours */
            CHECK((get(h,"forecastTime") % 6) == 0);  /* Every six hours */
        }
    }

    check_range(h,p,min,max);
}

static void height_level(grib_handle* h,const parameter* p,double min,double max)
{
    long level = get(h,"level");

    if (is_uerra){
        switch(level)
        {
        case   15:
        case   30:
        case   50:
        case   75:
        case  100:
        case  150:
        case  200:
        case  250:
        case  300:
        case  400:
        case  500:
            break;
        default:
            printf("%s, field %d [%s]: invalid height level %ld\n",file,field,param,level);
            error++;
            break;
        }
    }
}

static void pressure_level(grib_handle* h,const parameter* p,double min,double max)
{
    long level = get(h,"level");

    if (is_uerra){
        switch(level)
        {
        case 1000:
        case  975:
        case  950:
        case  925:
        case  900:
        case  875:
        case  850:
        case  825:
        case  800:
        case  750:
        case  700:
        case  600:
        case  500:
        case  400:
        case  300:
        case  250:
        case  200:
        case  150:
        case  100:
        case   70:
        case  50:
        case   30:
        case   20:
        case   10:
            break;
        default:
            printf("%s, field %d [%s]: invalid pressure level %ld\n",file,field,param,level);
            error++;
            break;
        }
    }
    else if (is_s2s){
        switch(level)
        {
        case 1000:
        case  925:
        case  850:
        case  700:
        case  500:
        case  300:
        case  200:
        case  100:
        case  50:
        case  10:
            break;
        default:
            printf("%s, field %d [%s]: invalid pressure level %ld\n",file,field,param,level);
            error++;
            break;
        }
    } else {
        switch(level)
        {
        case 1000:
        case  200:
        case  250:
        case  300:
        case  500:
        case  700:
        case  850:
        case  925:
        case  50:
            break;
        default:
            printf("%s, field %d [%s]: invalid pressure level %ld\n",file,field,param,level);
            error++;
            break;
        }
    }
}

static void potential_vorticity_level(grib_handle* h,const parameter* p,double min,double max)
{
    long level = get(h,"level");

    switch(level)
    {
    case 2:
        break;
    default:
        printf("%s, field %d [%s]: invalid potential vorticity level %ld\n",file,field,param,level);
        error++;
        break;
    }
}

static void potential_temperature_level(grib_handle* h,const parameter* p,double min,double max)
{
    long level = get(h,"level");

    switch(level)
    {
    case 320:
        break;
    default:
        printf("%s, field %d [%s]: invalid potential temperature level %ld\n",file,field,param,level);
        error++;
        break;
    }
}

static void statistical_process(grib_handle* h,const parameter* p,double min,double max)
{
    switch(get(h,"typeOfProcessedData"))
    {
    case 0: /* Analysis */
        if (is_uerra)
            CHECK(eq(h,"productDefinitionTemplateNumber",8)||eq(h,"productDefinitionTemplateNumber",11));
        break;

    case 1: /* Forecast */
        if (is_uerra)
            CHECK(eq(h,"productDefinitionTemplateNumber",8)||eq(h,"productDefinitionTemplateNumber",11));
        break;

    case 2: /* Analysis and forecast products */
        CHECK(eq(h,"productDefinitionTemplateNumber",8));
        break;

    case 3: /* Control forecast products */
        if (!is_s2s_refcst)
            CHECK(eq(h,"productDefinitionTemplateNumber",11));
        else
            CHECK(eq(h,"productDefinitionTemplateNumber",61));
        break;

    case 4: /* Perturbed forecast products */
        if (!is_s2s_refcst)
            CHECK(eq(h,"productDefinitionTemplateNumber",11));
        else
            CHECK(eq(h,"productDefinitionTemplateNumber",61));
        break;

    default:
        printf("Unsupported typeOfProcessedData %ld\n",get(h,"typeOfProcessedData"));
        error++;
        return;
        break;
    }

    if (is_lam) {
        if(get(h,"indicatorOfUnitOfTimeRange") == 10 ) /*  three hours */
        {
            /* Three hourly is OK */
            ;
        }
        else
        {
            CHECK(eq(h,"indicatorOfUnitOfTimeRange",1));/* Hours */
            CHECK((get(h,"forecastTime") % 3) == 0);  /* Every three hours */
        }
    } 
    else if (is_uerra) 
    {
/*  forecastTime for uerra might be all steps decreased by 1 i.e 0,1,2,3,4,5,8,11...29 too many... */
        if(get(h,"indicatorOfUnitOfTimeRange") == 1)
        {
            CHECK(le(h,"forecastTime",30));
        }
    }
    else 
    {
        if(get(h,"indicatorOfUnitOfTimeRange") == 11) /*  six hours */
        {
            /* Six hourly is OK */
            ;
        }
        else
        {
            CHECK(eq(h,"indicatorOfUnitOfTimeRange",1));/* Hours */
            CHECK((get(h,"forecastTime") % 6) == 0);  /* Every six hours */
        }
    }

    CHECK(eq(h,"numberOfTimeRange",1));
    CHECK(eq(h,"numberOfMissingInStatisticalProcess",0));
    CHECK(eq(h,"typeOfTimeIncrement",2));
    /*CHECK(eq(h,"indicatorOfUnitOfTimeForTheIncrementBetweenTheSuccessiveFieldsUsed",255));*/

    if (is_s2s)
        if(get(h,"typeOfStatisticalProcessing") == 0)
            CHECK(eq(h,"timeIncrementBetweenSuccessiveFields",1)||eq(h,"timeIncrementBetweenSuccessiveFields",4));
        else
            CHECK(eq(h,"timeIncrementBetweenSuccessiveFields",0));
    else
        CHECK(eq(h,"timeIncrementBetweenSuccessiveFields",0));


    CHECK(eq(h,"minuteOfEndOfOverallTimeInterval",0));
    CHECK(eq(h,"secondOfEndOfOverallTimeInterval",0));

    if (is_uerra)
    {
        CHECK((eq(h,"endStep",1)||eq(h,"endStep",2)||eq(h,"endStep",4)||eq(h,"endStep",5))||(get(h,"endStep") % 3) == 0);
    }
    else if (is_lam)
    {
        CHECK((get(h,"endStep") % 3) == 0);  /* Every three hours */
    }
    else
    {
        CHECK((get(h,"endStep") % 6) == 0); /* Every six hours */
    }
    

    if(get(h,"indicatorOfUnitForTimeRange") == 11)
    {
        /* Six hourly is OK */
        CHECK(get(h,"lengthOfTimeRange")*6 + get(h,"startStep") == get(h,"endStep"));
    }
    else if(get(h,"indicatorOfUnitForTimeRange") == 10)
    {
        /* Three hourly is OK */
        CHECK(get(h,"lengthOfTimeRange")*3 + get(h,"startStep") == get(h,"endStep"));
    }
    else
    {
        CHECK(eq(h,"indicatorOfUnitForTimeRange",1)); /* Hours */
        CHECK(get(h,"lengthOfTimeRange") + get(h,"startStep") == get(h,"endStep"));
    }
}

static void has_bitmap(grib_handle* h,const parameter* p,double min,double max)
{
    /* printf("bitMapIndicator %ld\n",get(h,"bitMapIndicator")); */
    CHECK(eq(h,"bitMapIndicator",0));
}

static void has_soil_level(grib_handle* h,const parameter* p,double min,double max)
{
    CHECK(get(h,"topLevel") == get(h,"bottomLevel"));
    CHECK(le(h,"level",14)); /* max in UERRA */
}

static void has_soil_layer(grib_handle* h,const parameter* p,double min,double max)
{
    CHECK(get(h,"topLevel") == get(h,"bottomLevel") - 1);
    CHECK(le(h,"level",14)); /* max in UERRA */
}

static void six_hourly(grib_handle* h,const parameter* p,double min,double max)
{
    statistical_process(h,p,min,max);

    if(get(h,"indicatorOfUnitForTimeRange") == 11)
        CHECK(eq(h,"lengthOfTimeRange",1));
    else
        CHECK(eq(h,"lengthOfTimeRange",6));

    CHECK(get(h,"endStep") == get(h,"startStep") + 6);
    check_range(h,p,min,max);
}

static void since_prev_pp(grib_handle* h,const parameter* p,double min,double max)
{
    statistical_process(h,p,min,max);

    CHECK(eq(h,"indicatorOfUnitForTimeRange",1));
    CHECK(get(h,"endStep") == get(h,"startStep") + get(h,"lengthOfTimeRange"));
    check_range(h,p,min,max);
}

static void three_hourly(grib_handle* h,const parameter* p,double min,double max)
{
    statistical_process(h,p,min,max);

    if(get(h,"indicatorOfUnitForTimeRange") == 11)
        CHECK(eq(h,"lengthOfTimeRange",1));
    else
        CHECK(eq(h,"lengthOfTimeRange",3));

    CHECK(get(h,"endStep") == get(h,"startStep") + 3);
    check_range(h,p,min,max);
}

static void from_start(grib_handle* h,const parameter* p,double min,double max)
{
    long step = get(h,"endStep");
    statistical_process(h,p,min,max);
    CHECK(eq(h,"startStep",0));

    if(step == 0){
        if(!is_uerra){
            CHECK(min == 0 && max == 0); /* ??? xxx */
        }
    }
    else
    {
        check_range(h,p,min/step,max/step);
    }
}

static void daily_average(grib_handle* h,const parameter* p,double min,double max)
{
    long step = get(h,"endStep");
    CHECK(get(h,"startStep") == get(h,"endStep") - 24);
    statistical_process(h,p,min,max);

    if(step == 0)
        CHECK(min == 0 && max == 0);
    else
    {
        check_range(h,p,min,max);
    }
}

static void given_level(grib_handle* h,const parameter* p,double min,double max)
{
    CHECK(ne(h,"typeOfFirstFixedSurface",255));
    CHECK(!missing(h,"scaleFactorOfFirstFixedSurface"));
    CHECK(!missing(h,"scaledValueOfFirstFixedSurface"));

    CHECK(eq(h,"typeOfSecondFixedSurface",255));
    CHECK(missing(h,"scaleFactorOfSecondFixedSurface"));
    CHECK(missing(h,"scaledValueOfSecondFixedSurface"));
}

static void predefined_level(grib_handle* h,const parameter* p,double min,double max)
{
    CHECK(ne(h,"typeOfFirstFixedSurface",255));
    CHECK(missing(h,"scaleFactorOfFirstFixedSurface"));
    CHECK(missing(h,"scaledValueOfFirstFixedSurface"));

    CHECK(eq(h,"typeOfSecondFixedSurface",255));
    CHECK(missing(h,"scaleFactorOfSecondFixedSurface"));
    CHECK(missing(h,"scaledValueOfSecondFixedSurface"));
}

static void predefined_thickness(grib_handle* h,const parameter* p,double min,double max)
{
    CHECK(ne(h,"typeOfFirstFixedSurface",255));
    CHECK(missing(h,"scaleFactorOfFirstFixedSurface"));
    CHECK(missing(h,"scaledValueOfFirstFixedSurface"));

    CHECK(ne(h,"typeOfSecondFixedSurface",255));
    CHECK(missing(h,"scaleFactorOfSecondFixedSurface"));
    CHECK(missing(h,"scaledValueOfSecondFixedSurface"));
}

static void given_thickness(grib_handle* h,const parameter* p,double min,double max)
{
    CHECK(ne(h,"typeOfFirstFixedSurface",255));
    CHECK(!missing(h,"scaleFactorOfFirstFixedSurface"));
    CHECK(!missing(h,"scaledValueOfFirstFixedSurface"));

    CHECK(ne(h,"typeOfSecondFixedSurface",255));
    CHECK(!missing(h,"scaleFactorOfSecondFixedSurface"));
    CHECK(!missing(h,"scaledValueOfSecondFixedSurface"));
}

static void latlon_grid(grib_handle* h)
{
    const double tolerance = 1.0/1000000.0; /* angular tolerance for grib2: micro degrees */
    long data_points = get(h,"numberOfDataPoints");
    long meridian    = get(h,"numberOfPointsAlongAMeridian");
    long parallel    = get(h,"numberOfPointsAlongAParallel");

    long north = get(h,"latitudeOfFirstGridPoint");
    long south = get(h,"latitudeOfLastGridPoint");
    long west  = get(h,"longitudeOfFirstGridPoint");
    long east  = get(h,"longitudeOfLastGridPoint");

    long ns = get(h,"jDirectionIncrement");
    long we = get(h,"iDirectionIncrement");

    double dnorth = dget(h,"latitudeOfFirstGridPointInDegrees");
    double dsouth = dget(h,"latitudeOfLastGridPointInDegrees");
    double dwest  = dget(h,"longitudeOfFirstGridPointInDegrees");
    double deast  = dget(h,"longitudeOfLastGridPointInDegrees");

    double dns = dget(h,"jDirectionIncrementInDegrees");
    double dwe = dget(h,"iDirectionIncrementInDegrees");

    if(eq(h,"basicAngleOfTheInitialProductionDomain",0))
    {
        CHECK(missing(h,"subdivisionsOfBasicAngle"));
    }
    else
    {
        /* long basic    = get(h,"basicAngleOfTheInitialProductionDomain"); */
        /* long division = get(h,"subdivisionsOfBasicAngle"); */
        CHECK(!missing(h,"subdivisionsOfBasicAngle"));
        CHECK(!eq(h,"subdivisionsOfBasicAngle",0));
    }

    if(missing(h,"subdivisionsOfBasicAngle"))
    {
        CHECK(eq(h,"basicAngleOfTheInitialProductionDomain",0));
    }

    CHECK(meridian*parallel == data_points);

    CHECK(eq(h,"resolutionAndComponentFlags1",0));
    CHECK(eq(h,"resolutionAndComponentFlags2",0));
    CHECK(eq(h,"resolutionAndComponentFlags6",0));
    CHECK(eq(h,"resolutionAndComponentFlags7",0));
    CHECK(eq(h,"resolutionAndComponentFlags8",0));

    CHECK(eq(h,"iDirectionIncrementGiven",1));
    CHECK(eq(h,"jDirectionIncrementGiven",1));

    CHECK(eq(h,"numberOfOctectsForNumberOfPoints",0));
    CHECK(eq(h,"interpretationOfNumberOfPoints",0));

    if(get(h,"iScansNegatively"))
    {
        long tmp    = east;
        double dtmp = deast;

        east = west;
        west = tmp;

        deast = dwest;
        dwest = dtmp;
    }

    if(get(h,"jScansPositively"))
    {
        long   tmp  = north;
        double dtmp = dnorth;

        north = south;
        south = tmp;

        dnorth = dsouth;
        dsouth = dtmp;
    }

    if (!(is_lam || is_uerra))
    {
        double area, globe;
        CHECK(north > south);
        CHECK(east  > west);

        /* Check that the grid is symmetrical */
        CHECK(north == -south);
        CHECK( DBL_EQUAL(dnorth, -dsouth, tolerance) );
        CHECK(parallel == (east-west)/we + 1);
        CHECK(fabs((deast-dwest)/dwe + 1 - parallel) < 1e-10);
        CHECK(meridian == (north-south)/ns + 1);
        CHECK(fabs((dnorth-dsouth)/dns + 1 - meridian) < 1e-10 );

        /* Check that the field is global */
        area  = (dnorth-dsouth) * (deast-dwest);
        globe = 360.0*180.0;
        CHECK(area <= globe);
        CHECK(area >= globe*0.95);
    }

    /* GRIB2 requires longitudes are always positive */
    CHECK(east >= 0);
    CHECK(west >= 0);

    /*
      printf("meridian=%ld north=%ld south=%ld ns=%ld \n",meridian,north,south,ns);
      printf("meridian=%ld north=%f south=%f ns=%f \n",meridian,dnorth,dsouth,dns);
      printf("parallel=%ld east=%ld west=%ld we=%ld \n",parallel,east,west,we);
      printf("parallel=%ld east=%f west=%f we=%f \n",parallel,deast,dwest,dwe);
     */

}

#define X(x) printf("%s=%ld ",#x,get(h,#x))

static void check_parameter(grib_handle* h,double min,double max)
{
    int err;
    int best = -1;
    int match = -1;
    size_t i = 0;

    for(i = 0; i < NUMBER(parameters); i++)
    {
        int j = 0;
        int matches = 0;
        while(parameters[i].pairs[j].key != NULL)
        {
            long val = -1;
            const int ktype = parameters[i].pairs[j].key_type;
            if (ktype == GRIB_TYPE_LONG) {
                if(grib_get_long(h,parameters[i].pairs[j].key,&val) == GRIB_SUCCESS) {
                    if(parameters[i].pairs[j].value_long == val) {
                        matches++;
                    }
                }
            }
            else if (ktype == GRIB_TYPE_STRING) {
                char strval[256]={0,};
                size_t len = 256;
                if (is_uerra && strcasecmp(parameters[i].pairs[j].key,"model")==0) {
                    /* printf("Skipping model keyword for UERRA class\n"); */
                    matches++; /*xxx hack to pretend that model key was matched.. */
                } else {
                    if (strcasecmp(parameters[i].pairs[j].value_string,"MISSING")==0) {
                        int is_miss = grib_is_missing(h, parameters[i].pairs[j].key, &err);
                        if (err == GRIB_SUCCESS && is_miss) {
                            matches++;
                        }
                    }
                    else if(grib_get_string(h,parameters[i].pairs[j].key,strval,&len) == GRIB_SUCCESS) {
                        if(strcmp(parameters[i].pairs[j].value_string, strval) == 0) {
                            matches++;
                        }
                    }
                }
            }
            else {
                assert(!"Unknown key type");
            }
            j++;
#if 0
            printf("%s %s %ld val -> %d %d %d\n",
                    parameters[i].pairs[j].key,
                    parameters[i].pairs[j].value_string,
                    val,
                    matches,
                    j,
                    best);
#endif
        }

        if(matches == j && matches > best)
        {
            best = matches;
            match = i;
        }
    }

    if(match >= 0)
    {

        /*int j = 0;*/
        param = parameters[match].name;
        i = 0;
        while(parameters[match].checks[i])
            (*parameters[match].checks[i++])(h,&parameters[match],min,max);
        /*
                printf("=========================\n");
                printf("%s -> %d %d\n",param, match, best);
                while(parameters[match].pairs[j].key != NULL)
                {
                     printf("%s val -> %ld %d\n",parameters[match].pairs[j].key,parameters[match].pairs[j].value,j);
                     j++;
                }
                printf("matched parameter: %s\n", param);
         */
    }
    else
    {
        printf("%s, field %d [%s]: cannot match parameter\n",file,field,param);
        X(origin);
        X(discipline);
        X(parameterCategory);
        X(parameterNumber);
        X(typeOfFirstFixedSurface);
        X(scaleFactorOfFirstFixedSurface);
        X(scaledValueOfFirstFixedSurface);
        X(typeOfSecondFixedSurface);
        X(scaleFactorOfSecondFixedSurface);
        X(scaledValueOfSecondFixedSurface);
        printf("\n");
        error++;
    }
}

static void verify(grib_handle* h)
{
    double min = 0,max = 0;

    CHECK(eq(h,"editionNumber",2));
    CHECK(missing(h,"reserved") || eq(h,"reserved",0));

    if (valueflg)
    {
        size_t count, n;
        int e = grib_get_size(h,"values",&count);
        double *values;
        size_t i;
        int bitmap = !eq(h,"bitMapIndicator",255);

        if(e) {
            printf("%s, field %d [%s]: cannot number of values: %s\n",file,field,param,grib_get_error_message(e));
            error++;
            return;
        }

        values = (double*)malloc(sizeof(double)*(count));
        if(!values)
        {
            printf("%s, field %d [%s]: failed to allocate %ld bytes\n",file,field,param,(long)(sizeof(double)*count));
            error++;
            return;
        }

        CHECK(eq(h,"numberOfDataPoints",count));

        n = count;
        if((e =  grib_get_double_array(h,"values",values,&count)))
        {
            printf("%s, field %d [%s]: cannot get values: %s\n",file,field,param,grib_get_error_message(e));
            free(values);
            error++;
            return;
        }

        if(n != count)
        {
            printf("%s, field %d [%s]: value count changed %ld -> %ld\n",file,field,param,(long)count,(long)n);
            free(values);
            error++;
            return;
        }

        if(bitmap)
        {
            double missing = dget(h,"missingValue");

            min = max = missing;

            for(i = 0; i < count ; i++)
            {
                if((min == missing) || ((values[i] != missing) && (min>values[i]))) min = values[i];
                if((max == missing) || ((values[i] != missing) && (max<values[i]))) max = values[i];
            }
        }
        else
        {
            min = max = values[0];
            for(i = 1; i < count ; i++)
            {
                if(min>values[i]) min = values[i];
                if(max<values[i]) max = values[i];
            }
        }
        free(values);
    }

    check_parameter(h,min,max);

    /* Section 1 */

    CHECK(ge(h,"gribMasterTablesVersionNumber",4));
    CHECK(eq(h,"versionNumberOfGribLocalTables",0)); /* Local tables not used */

    CHECK(eq(h,"significanceOfReferenceTime",1)); /* Start of forecast */

    if (!is_s2s){
        /* todo check for how many years back the reforecast is done? Is it coded in the grib??? */
        /* Check if the date is OK */
        {
            long date = get(h,"date");
            /* CHECK(date > 20060101); */

            CHECK( (date / 10000)         == get(h,"year"));
            CHECK( ((date % 10000) / 100) == get(h,"month"));
            CHECK( ((date % 100))         == get(h,"day"));
        }
    }

    if (is_uerra){
        CHECK(le(h,"hour",24));
    }
    else if (is_lam){
        CHECK(eq(h,"hour",0) || eq(h,"hour",3) || eq(h,"hour",6) || eq(h,"hour",9) || eq(h,"hour",12) || eq(h,"hour",15) || eq(h,"hour",18) || eq(h,"hour",21));
    }
    else
    {
    /* Only 00, 06 12 and 18 Cycle OK */
        CHECK(eq(h,"hour",0) || eq(h,"hour",6) || eq(h,"hour",12) || eq(h,"hour",18));
    }
    CHECK(eq(h,"minute",0));
    CHECK(eq(h,"second",0));
    CHECK(ge(h,"startStep",0));

    if (is_s2s){
        CHECK(eq(h,"productionStatusOfProcessedData",6)||eq(h,"productionStatusOfProcessedData",7)); /*  S2S prod||test */
        CHECK(le(h,"endStep",100*24));
    }
    else if (!is_uerra)
    {
        CHECK(eq(h,"productionStatusOfProcessedData",4)||eq(h,"productionStatusOfProcessedData",5)); /*  TIGGE prod||test */
        CHECK(le(h,"endStep",30*24));
    }

    if (is_uerra){
        CHECK((eq(h,"step",1)||eq(h,"step",2)||eq(h,"step",4)||eq(h,"step",5))||(get(h,"step") % 3) == 0);
    }
    else if (is_lam) {
        CHECK((get(h,"step") % 3) == 0);
    }
    else
    {
        CHECK((get(h,"step") % 6) == 0);
    }

    if (is_uerra){
        CHECK(eq(h,"productionStatusOfProcessedData",8)||eq(h,"productionStatusOfProcessedData",9)); /*  UERRA prod||test */
        CHECK(le(h,"endStep",30));
        /* 0 = analysis , 1 = forecast */
        CHECK(eq(h,"typeOfProcessedData",0)||eq(h,"typeOfProcessedData",1));
        if (get(h,"typeOfProcessedData") == 0)
            CHECK(eq(h,"step",0));
        else
            CHECK((eq(h,"step",1)||eq(h,"step",2)||eq(h,"step",4)||eq(h,"step",5))||(get(h,"step") % 3) == 0);
    }
    else
    {
        /* 2 = analysis or forecast , 3 = control forecast, 4 = perturbed forecast */
        CHECK(eq(h,"typeOfProcessedData",2)||eq(h,"typeOfProcessedData",3)||eq(h,"typeOfProcessedData",4));
    }

    /* TODO: validate local usage. Empty for now xxx*/
    /* CHECK(eq(h,"section2.sectionLength",5)); */

    /* Section 3 */

    CHECK(eq(h,"sourceOfGridDefinition",0)); /* Specified in Code table 3.1 */

    switch(get(h,"gridDefinitionTemplateNumber"))
    {
    case 0:
    case 1: /*rotated latlon*/
        latlon_grid(h);
        break;

    case 30: /*Lambert conformal*/
        /*lambert_grid(h); # TODO xxx
        printf("warning: Lambert grid - geometry checking not implemented yet!\n"); */
       /*CHECK(eq(h,"scanningMode",64));*/ /* M-F data used to have it wrong.. but it might depends on other projection set up as well!*/
        break;

    case 40: /* gaussian grid (regular or reduced) */
        gaussian_grid(h);
        break;

    default:
        printf("%s, field %d [%s]: Unsupported gridDefinitionTemplateNumber %ld\n",
                file,field,param,
                get(h,"gridDefinitionTemplateNumber"));
        error++;
        return;
        break;
    }

    /* If there is no bitmap, this should be true */
    /* CHECK(eq(h,"bitMapIndicator",255));*/

    if(eq(h,"bitMapIndicator",255))
        CHECK(get(h,"numberOfValues") == get(h,"numberOfDataPoints"));
    else
        CHECK(get(h,"numberOfValues") <= get(h,"numberOfDataPoints"));

    /* Check values */
    CHECK(eq(h,"typeOfOriginalFieldValues",0)); /* Floating point */

    check_validity_datetime(h);

    /* do not store empty values e.g. fluxes at step 0
        todo ?? now it's allowed in the code here!
        if(!missing(h,"typeOfStatisticalProcessing"))
          CHECK(ne(h,"stepRange",0));*/


}

static void validate(const char* path)
{
    FILE *f = fopen(path,"r");
    grib_handle *h = 0;
    int err;
    int count = 0;

    file  = path;
    field = 0;

    if(!f) {
        printf("%s: %s\n",path,strerror(errno));
        error++;
        return;
    }

    while( (h= grib_handle_new_from_file(0,f,&err)) != NULL)
    {
        int last_error   = error;
        int last_warning = warning;

        ++field;
        verify(h);

        if((last_error != error) || (warnflg && (last_warning != warning)))
            save(h,bad,fbad);
        else
            save(h,good,fgood);


        grib_handle_delete(h);
        count++;
        param = "unknown";
    }
    fclose(f);

    if(err) {
        printf("%s: grib_handle_new_from_file: %s\n",path,grib_get_error_message(err));
        error++;
        return;
    }

    if(count == 0) {
        printf("%s does not contain any GRIBs\n",path);
        error++;
        return;
    }
}

static void scan(const char* name)
{
    DIR *dir;
    if((dir = opendir(name)) != NULL)
    {
        struct dirent* e;
        char tmp[1024];
        while( (e = readdir(dir)) != NULL)
        {
            if(e->d_name[0] == '.') continue;
            sprintf(tmp,"%s/%s",name,e->d_name);
            scan(tmp);
        }

        closedir(dir);
    }
    else
        validate(name);
}

static void usage()
{
    printf("tigge_check [options] grib_file grib_file ...\n");
    printf("   -l: check local area model fields\n");
    printf("   -v: check value ranges\n");
    printf("   -w: warnings are treated as errors\n");
    printf("   -g: write good gribs\n");
    printf("   -b: write bad  gribs\n");
    printf("   -z: return 0 to calling shell\n");
    printf("   -s: check s2s fields\n");
    printf("   -r: check s2s reforecast fields\n");
    printf("   -u: check uerra fields\n");
    exit(1);
}

int main(int argc, char** argv)
{
    int i;
    int err = 0;

    for(i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
            case 'w':
                warnflg++;
                break;

            case 'z':
                zeroflg++;
                break;

            case 'v':
                valueflg++;
                break;

            case 'g':
                if(++i == argc) usage();
                good = argv[i];
                fgood = fopen(good,"w");
                if(!fgood) { perror(good); exit(1); }
                break;

            case 'b':
                if(++i == argc) usage();
                bad = argv[i];
                fbad = fopen(bad,"w");
                if(!fbad) { perror(bad); exit(1); }
                break;

            case 'l':
                is_lam=1;
                break;

            case 's':
                is_s2s=1;
                break;

            case 'r':
                is_s2s_refcst=1;
                break;

            case 'u':
                is_uerra=1;
                break;

            default:
                usage();
                break;
            }
        }
        else
        {
            break;
        }
    }

    if(i == argc)
        usage();

    for(; i < argc; i++)
    {
        error = 0;
        scan(argv[i]);
        if(error) err = 1;
        if(warning && warnflg) err = 1;
    }

    if(fgood && fclose(fgood)) { perror(good); exit(1); }
    if(fbad  && fclose(fbad )) { perror(bad ); exit(1); }

    return zeroflg ? 0: err;
}
