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
 * C Implementation: bufr_read_tempf
 *
 * Description: read and print radiosonde data from TEMP BUFR messages.
 * If available this version also lists the position information from the WMO list
 * (now OSCAR/Surface) appended to the reports by ECMWF
 *
 * Author: Bruce Ingleby
 */

/*
 * Please note that TEMP reports can be encoded in various ways in BUFR. Therefore the code
 * below might not work directly for other types of TEMP messages than the one used in the
 * example. It is advised to use bufr_dump to understand the structure of the messages.
 */

#include "eccodes.h"

/* Returns 1 if the bit at 'pos' in 'var' is set. The counting starts at 0 */
#define BTEST(var, pos) ((var) & (1 << (pos)))

/* Helper function to fill a double array with values of 'key'. Client must free memory */
static int get_double_array(codes_handle* h, const char* key, double** arr, size_t* size)
{
    CODES_CHECK(codes_get_size(h, key, size), 0);
    *arr = (double*)malloc(*size * sizeof(double));
    return codes_get_double_array(h, key, *arr, size);
}

/* Helper function to fill an integer (=long) array with values of 'key'. Client must free memory */
static int get_long_array(codes_handle* h, const char* key, long** arr, size_t* size)
{
    CODES_CHECK(codes_get_size(h, key, size), 0);
    *arr = (long*)malloc(*size * sizeof(long));
    return codes_get_long_array(h, key, *arr, size);
}

/* Reset dimension of input array to 'newsize' and fill with 'fillValue' */
static void realloc_and_fill(double** arr, size_t newsize, double fillValue)
{
    size_t i;
    free(*arr);
    *arr = (double*)malloc(newsize * sizeof(double));
    for(i=0; i<newsize; ++i) *arr[i] = fillValue;
}

int main(int argc, char* argv[])
{
    FILE* in = NULL;

    /* Message handle. Required in all the ecCodes calls acting on a message.*/
    codes_handle* h = NULL;

    double *lat = NULL, *lon = NULL, *presVal = NULL, *zVal = NULL;
    double *dlatVal = NULL, *dlonVal = NULL;
    double *tVal = NULL, *tdVal = NULL, *wspVal = NULL, *wdirVal = NULL;
    double htg, htp, htec = 0, balloonwt;
    int err         = 0;
    int status_rsno = 0, status_ht = 0, status_airt = 0, status_dewt = 0, status_p = 0;
    int count     = 0;
    int llskip    = 0;
    int llstdonly = 1; /* Set True to list standard levels only */
    size_t i = 0, size = 0, sizews = 0, sizelats = 0;
    long blockNumber, stationNumber;
    long year, month, day, hour, minute, second, ymd, hms, sondeType;
    long *timeVal = NULL, *vssVal = NULL;
    const char* infile = "../../data/bufr/PraticaTemp.bufr";
    char statid[128]   = {0,};
    char dropid[128]   = {0,};
    char rsnumber[16] = {0,};
    char rssoftware[16] = {0,};

    in = fopen(infile, "rb");
    if (!in) {
        fprintf(stderr, "Error: unable to open file %s\n", infile);
        return 1;
    }

    /* Loop over the messages in the BUFR file */
    while ((h = codes_handle_new_from_file(NULL, in, PRODUCT_BUFR, &err)) != NULL || err != CODES_SUCCESS) {
        count++;
        if (h == NULL) {
            fprintf(stderr, "Error: unable to create handle for message %d\n", count);
            continue;
        }

        /* We need to instruct ecCodes to expand the descriptors i.e., unpack the data values */
        CODES_CHECK(codes_set_long(h, "unpack", 1), 0);

        /* In our BUFR message verticalSoundingSignificance is always followed by
         *    geopotential, airTemperature, dewpointTemperature,
         *    windDirection, windSpeed and pressure */
        llskip = 0;

        size = 1024;
        err  = codes_get_string(h, "aircraftRegistrationNumberOrOtherIdentification", dropid, &size);
        if (err) strcpy(dropid, "UNKNOWN");
        size = 1024;
        err  = codes_get_string(h, "shipOrMobileLandStationIdentifier", statid, &size);
        if (err) strcpy(statid, dropid);

        CODES_CHECK(codes_get_long(h, "blockNumber", &blockNumber), 0);
        CODES_CHECK(codes_get_long(h, "stationNumber", &stationNumber), 0);
        if (blockNumber < 99 && stationNumber < 1000)
            snprintf(statid, sizeof(statid), "%ld%ld", blockNumber, stationNumber);
        CODES_CHECK(codes_get_long(h, "year", &year), 0);
        CODES_CHECK(codes_get_long(h, "month", &month), 0);
        CODES_CHECK(codes_get_long(h, "day", &day), 0);
        CODES_CHECK(codes_get_long(h, "hour", &hour), 0);
        CODES_CHECK(codes_get_long(h, "minute", &minute), 0);
        err = codes_get_long(h, "second", &second);
        if (err) second = 0;

        err = get_double_array(h, "latitude", &lat, &sizelats);

        err = get_double_array(h, "longitude", &lon, &size);

        status_ht = codes_get_double(h, "heightOfStationGroundAboveMeanSeaLevel", &htg);
        if (status_ht) htg = -999.0;
        status_ht = codes_get_double(h, "heightOfBarometerAboveMeanSeaLevel", &htp);
        if (status_ht) htp = -999.0;

        CODES_CHECK(codes_get_long(h, "radiosondeType", &sondeType), 0);
        err = codes_get_double(h, "heightOfStation", &htec); /* Height from WMO list (BUFR) */
        if (!err && htg == -999.0) htg = htec;

        ymd = year * 10000 + month * 100 + day;
        hms = hour * 10000 + minute * 100 + second;

        size        = 16;
        status_rsno = codes_get_string(h, "radiosondeSerialNumber", rsnumber, &size);
        size        = 16;
        codes_get_string(h, "softwareVersionNumber", rssoftware, &size);

        err = codes_get_double(h, "weightOfBalloon", &balloonwt);
        if (err) balloonwt = 0;

        /* Ascent (skip reports without dtime array for now) */
        err     = get_long_array(h, "timePeriod", &timeVal, &size);
        if (err) {
            printf("Ob: %d %s %ld %ld %g %g %g %g %ld\n",
                   count, statid, ymd, hms, lat[0], lon[0], htg, htp, sondeType);
            printf("Missing times - skip\n");
            llskip = 1;
        }
        status_p = get_double_array(h, "pressure", &presVal, &size);
        status_ht = get_double_array(h, "nonCoordinateGeopotentialHeight", &zVal, &size);

        if (!llskip) {
            err = get_double_array(h, "latitudeDisplacement", &dlatVal, &size);
            err = get_double_array(h, "longitudeDisplacement", &dlonVal, &size);
            err = get_long_array(h, "extendedVerticalSoundingSignificance", &vssVal, &size);
            status_airt = get_double_array(h, "airTemperature", &tVal, &size);
            status_dewt = get_double_array(h, "dewpointTemperature", &tdVal, &size);
            err = get_double_array(h, "windDirection", &wdirVal, &size);
            err = get_double_array(h, "windSpeed", &wspVal, &sizews);

            if (status_p != CODES_SUCCESS) {
                realloc_and_fill(&presVal, sizews, -999999999.0);
            }
            if (status_ht != CODES_SUCCESS) {
                realloc_and_fill(&zVal, sizews, -999999999.0);
            }
            if (status_airt != CODES_SUCCESS) {
                realloc_and_fill(&tVal, sizews, -999999999.0);
            }
            if (status_dewt != CODES_SUCCESS) {
                realloc_and_fill(&tdVal, sizews, -999999999.0);
            }
            /* Print the values */
            printf("Ob: %7d %s %ld %ld %7.3f %7.3f %7.1f %7.1f %4ld %5zu\n",
                   count, statid, ymd, hms, lat[0], lon[0], htg, htp, sondeType, sizews);
            if (status_rsno == CODES_SUCCESS) {
                printf("RS number/software/balloonwt: %s %s %7.3f\n", rsnumber, rssoftware, balloonwt);
            }
            if (status_ht == CODES_SUCCESS && sizelats > 1) {
                printf("WMO list lat, lon, ht: %s %g %g %g\n", statid, lat[1], lon[1], htec);
            }
            printf("level  dtime    dlat   dlon   pressure  geopotH  airTemp   dewPtT  windDir   windSp   signif\n");
            for (i = 0; i < sizews; ++i) {
                long iflag = vssVal[i];
                if (!llstdonly || BTEST(iflag, 16)) {
                    printf("%5zu %6ld %7.3f %7.3f %9.1f %8.1f %8.2f %8.2f %8.2f %8.2f %8ld\n",
                           i + 1, timeVal[i],
                           dlatVal[i], dlonVal[i],
                           presVal[i], zVal[i], tVal[i], tdVal[i],
                           wdirVal[i], wspVal[i], vssVal[i]);
                }
            }
        }

        /* Release memory */
        free(lat);
        free(lon);
        free(timeVal);
        free(dlatVal);
        free(dlonVal);
        free(presVal);
        free(zVal);
        free(tVal);
        free(tdVal);
        free(wdirVal);
        free(wspVal);
        free(vssVal);
        codes_handle_delete(h);
    }

    fclose(in);
    printf("Finishing normally. Number of BUFR records read: %d\n", count);
    return 0;
}
