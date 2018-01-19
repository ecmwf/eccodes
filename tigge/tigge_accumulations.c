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
#include <string.h>
#include <assert.h>

#include "grib_api.h"

#define NUMBER(x) (sizeof(x)/sizeof(x[0]))

const char* checks[] = {
    /* Section 1 */
    "identificationOfOriginatingGeneratingCentre",
    "identificationOfOriginatingGeneratingSubCentre",
    "gribMasterTablesVersionNumber",
    "versionNumberOfGribLocalTables",
    "significanceOfReferenceTime",
    "year",
    "month",
    "day",
    "hour",
    "minute",
    "second",
    "productionStatusOfProcessedData",
    "typeOfProcessedData",

    /*Section3*/
    "sourceOfGridDefinition",
    "numberOfDataPoints",
    "numberOfOctetsForOptionalListOfNumbersDefiningNumberOfPoints",
    "interpretationOfListOfNumbersDefiningNumberOfPoints",
    "gridDefinitionTemplateNumber",
    "shapeOfTheEarth",
    "scaleFactorOfRadiusOfSphericalEarth",
    "scaledValueOfRadiusOfSphericalEarth",
    "scaleFactorOfMajorAxisOfOblateSpheroidEarth",
    "scaledValueOfMajorAxisOfOblateSpheroidEarth",
    "scaleFactorOfMinorAxisOfOblateSpheroidEarth",
    "scaledValueOfMinorAxisOfOblateSpheroidEarth",
    "numberOfPointsAlongAParallel",
    "numberOfPointsAlongAMeridian",
    "basicAngleOfTheInitialProductionDomain",
    "subdivisionsOfBasicAngle",
    "latitudeOfFirstGridPoint",
    "longitudeOfFirstGridPoint",
    "resolutionAndComponentFlags",
    "latitudeOfLastGridPoint",
    "longitudeOfLastGridPoint",
    "iDirectionIncrement",
    "jDirectionIncrement",
    "scanningMode",

    /*Section4*/

    "numberOfSection",
    "numberOfCoordinatesValues",
    "productDefinitionTemplateNumber",
    "parameterCategory",
    "parameterNumber",
    "typeOfGeneratingProcess",
    "backgroundGeneratingProcessIdentifier",
    "generatingProcessIdentifier",
    "hoursAfterReferenceTimeOfDataCutoff",
    "minutesAfterReferenceTimeOfDataCutoff",
/*    "indicatorOfUnitOfTimeRange",*/

    /* "forecastTime", */

    "typeOfFirstFixedSurface",
    "scaleFactorOfFirstFixedSurface",
    "scaledValueOfFirstFixedSurface",
    "typeOfSecondFixedSurface",
    "scaleFactorOfSecondFixedSurface",
    "scaledValueOfSecondFixedSurface",
    "typeOfEnsembleForecast",
    "perturbationNumber",
    "numberOfForecastsInEnsemble",

    /*
       "yearOfEndOfOverallTimeInterval",
       "monthOfEndOfOverallTimeInterval",
       "dayOfEndOfOverallTimeInterval",
       "hourOfEndOfOverallTimeInterval",
       "minuteOfEndOfOverallTimeInterval",
       "secondOfEndOfOverallTimeInterval",
     */

    "numberOfTimeRangeSpecificationsDescribingTheTimeIntervalsUsedToCalculateTheStatisticallyProcessedField",
    "totalNumberOfDataValuesMissingInStatisticalProcess",
    "typeOfStatisticalProcessing",
    "typeOfTimeIncrementBetweenSuccessiveFieldsUsedInTheStatisticalProcessing",
    /* "indicatorOfUnitOfTimeForTimeRangeOverWhichStatisticalProcessingIsDone", */
    /*
       "lengthOfTheTimeRangeOverWhichStatisticalProcessingIsDone",
     */
    "indicatorOfUnitOfTimeForTheIncrementBetweenTheSuccessiveFieldsUsed",
    "timeIncrementBetweenSuccessiveFields",
};

long check_values[NUMBER(checks)];

static void usage(const char *prog)
{
    fprintf(stderr,"%s:  in1 [in2 ... ]  out\n",prog);
    exit(1);
}

static void init_checks(grib_handle* h)
{
    size_t i;
    for(i = 0; i < NUMBER(checks); i++) {
        GRIB_CHECK(grib_get_long(h,checks[i],&check_values[i]),checks[i]);
    }
}

static void do_checks(grib_handle* h)
{
    size_t i;
    long val;
    for(i = 0; i < NUMBER(checks); i++)
    {
        GRIB_CHECK(grib_get_long(h,checks[i],&val),checks[i]);
        if(val != check_values[i])
        {
            printf("Value mismatch for %s: %ld and %ld\n",checks[i],check_values[i],val);
            exit(1);
        }
    }
}

static void output_field(grib_handle* h,FILE* f,long bits,double* values,size_t size,const char* out)
{
    const void *buffer; 

    GRIB_CHECK(grib_set_long(h,"numberOfBitsContainingEachPackedValue",bits),"numberOfBitsContainingEachPackedValue");
    GRIB_CHECK(grib_set_double_array(h,"values",values,size),NULL);
    GRIB_CHECK(grib_get_message(h,&buffer,&size),NULL);

    if(fwrite(buffer,1,size,f) != size)
    {
        perror(out);
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    int i;
    FILE *in,*out;
    int e;
    grib_handle *result = NULL,*h;
    double* values = NULL;
    double *tmp = NULL;
    size_t size=0,count,j;

    long step      = 0;
    long startStep = 0;
    long endStep   = 0;
    long bits      = 0;

    if(argc < 3) usage(argv[0]);

    out = fopen(argv[argc-1],"w");
    if(!out) {
        perror(argv[argc-1]);
        exit(1);
    }

    for(i = 1; i < argc-1; i++)
    {
        in = fopen(argv[i],"r");
        if(!in) {
            perror(argv[i]);
            exit(1);
        }

        while( (h = grib_handle_new_from_file(NULL,in,&e)) != NULL )
        {
            long b = 0;

            GRIB_CHECK(grib_get_long(h,"startStep",&startStep),"startStep");
            GRIB_CHECK(grib_get_long(h,"endStep",&endStep),"endStep");
            GRIB_CHECK(grib_get_long(h,"numberOfBitsContainingEachPackedValue",&b),"numberOfBitsContainingEachPackedValue");

            if(b > bits) bits = b;

            if(result == NULL)
            {
                grib_handle *g = grib_handle_clone(h);
                result = h;

                assert(g);

                init_checks(g);

                GRIB_CHECK(grib_get_size(g,"values",&size),argv[i]);

                values = (double*)calloc(size,sizeof(double));
                assert(values);

                tmp    = (double*)calloc(size,sizeof(double));
                assert(tmp);

                assert(startStep == 0);

                GRIB_CHECK(grib_set_long(g,"startStep",0),"startStep");
                GRIB_CHECK(grib_set_long(g,"endStep",0),"endStep");

                output_field(g,out,0,values,size,argv[argc-1]);

                grib_handle_delete(g);

            }
            else
                do_checks(h);

            GRIB_CHECK(grib_get_size(h,"values",&count),argv[i]);
            assert(count == size);

            GRIB_CHECK(grib_get_double_array(h,"values",tmp,&count),argv[i]);
            assert(count == size);

            for(j = 0; j < count; j++)
                values[j] += tmp[j];


            if(startStep != step)
            {
                printf("Fields not in step order: step=%ld previous=%ld\n",startStep,step);
                exit(1);
            }

            assert(endStep - startStep == 6);
            step = endStep;

            GRIB_CHECK(grib_set_long(result,"startStep",0), "startStep");
            GRIB_CHECK(grib_set_long(result,"endStep",step),"endStep");

            output_field(result,out,bits,values,size,argv[argc-1]);

            if(h != result)
                grib_handle_delete(h);
        }

        GRIB_CHECK(e,argv[argc-2]);
    }

    if(result)
        grib_handle_delete(result);

    if(fclose(out))
    {
        perror(argv[argc-1]);
        exit(1);
    }

    return 0;
}
