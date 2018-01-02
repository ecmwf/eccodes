/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <eccodes.h>

/* This code was generated automatically */

int main(int argc, char** argv)
{
    codes_handle *h     = NULL;
    size_t size        = 0;
    double* vdouble    = NULL;
    FILE* f            = NULL;
    const void* buffer = NULL;

    if(argc != 2) {
       fprintf(stderr,"usage: %s out\n",argv[0]);
        exit(1);
    }

    h = codes_grib_handle_new_from_samples(NULL,"GRIB2");
    if(!h) {
        fprintf(stderr,"Cannot create grib handle\n");
        exit(1);
    }

    CODES_CHECK(codes_set_long(h,"parametersVersion",1),0);
    CODES_CHECK(codes_set_long(h,"truncateLaplacian",0),0);
    CODES_CHECK(codes_set_long(h,"truncateDegrees",0),0);
    CODES_CHECK(codes_set_long(h,"dummy",1),0);
    CODES_CHECK(codes_set_long(h,"changingPrecision",0),0);
    CODES_CHECK(codes_set_long(h,"unitsFactor",1),0);
    CODES_CHECK(codes_set_long(h,"unitsBias",0),0);
    CODES_CHECK(codes_set_long(h,"timeRangeIndicatorFromStepRange",-1),0);
    CODES_CHECK(codes_set_long(h,"missingValue",9999),0);

    /* 0 = Meteorological products (grib2/tables/4/0.0.table)  */
    CODES_CHECK(codes_set_long(h,"discipline",0),0);

    CODES_CHECK(codes_set_long(h,"editionNumber",2),0);

    /* 98 = European Center for Medium-Range Weather Forecasts */
    CODES_CHECK(codes_set_long(h,"centre",98),0);

    CODES_CHECK(codes_set_long(h,"subCentre",0),0);

    /* 4 = Version implemented on 7 November 2007 (grib2/tables/1.0.table)  */
    CODES_CHECK(codes_set_long(h,"tablesVersion",4),0);


    /* 0 = Local tables not used  (grib2/tables/4/1.1.table)  */
    CODES_CHECK(codes_set_long(h,"localTablesVersion",0),0);


    /* 1 = Start of forecast (grib2/tables/4/1.2.table)  */
    CODES_CHECK(codes_set_long(h,"significanceOfReferenceTime",1),0);

    CODES_CHECK(codes_set_long(h,"year",2007),0);
    CODES_CHECK(codes_set_long(h,"month",3),0);
    CODES_CHECK(codes_set_long(h,"day",23),0);
    CODES_CHECK(codes_set_long(h,"hour",12),0);
    CODES_CHECK(codes_set_long(h,"minute",0),0);
    CODES_CHECK(codes_set_long(h,"second",0),0);
    CODES_CHECK(codes_set_long(h,"dataDate",20070323),0);
    CODES_CHECK(codes_set_long(h,"dataTime",1200),0);

    /* 0 = Operational products (grib2/tables/4/1.3.table)  */
    CODES_CHECK(codes_set_long(h,"productionStatusOfProcessedData",0),0);

    /* 2 = Analysis and forecast products (grib2/tables/4/1.4.table)  */
    CODES_CHECK(codes_set_long(h,"typeOfProcessedData",2),0);

    CODES_CHECK(codes_set_long(h,"selectStepTemplateInterval",1),0);
    CODES_CHECK(codes_set_long(h,"selectStepTemplateInstant",1),0);
    CODES_CHECK(codes_set_long(h,"grib2LocalSectionPresent",0),0);

    /* 0 = Specified in Code table 3.1 (grib2/tables/4/3.0.table)  */
    CODES_CHECK(codes_set_long(h,"sourceOfGridDefinition",0),0);

    CODES_CHECK(codes_set_long(h,"numberOfDataPoints",496),0);
    CODES_CHECK(codes_set_long(h,"numberOfOctectsForNumberOfPoints",0),0);

    /* 0 = There is no appended list (grib2/tables/4/3.11.table)  */
    CODES_CHECK(codes_set_long(h,"interpretationOfNumberOfPoints",0),0);

    CODES_CHECK(codes_set_long(h,"PLPresent",0),0);

    /* 0 = Latitude/longitude. Also called equidistant cylindrical, or Plate Carree (grib2/tables/4/3.1.table)  */
    CODES_CHECK(codes_set_long(h,"gridDefinitionTemplateNumber",0),0);


    /* 0 = Earth assumed spherical with radius = 6,367,470.0 m (grib2/tables/4/3.2.table)  */
    CODES_CHECK(codes_set_long(h,"shapeOfTheEarth",0),0);

    CODES_CHECK(codes_set_missing(h,"scaleFactorOfRadiusOfSphericalEarth"),0);
    CODES_CHECK(codes_set_missing(h,"scaledValueOfRadiusOfSphericalEarth"),0);
    CODES_CHECK(codes_set_missing(h,"scaleFactorOfEarthMajorAxis"),0);
    CODES_CHECK(codes_set_missing(h,"scaledValueOfEarthMajorAxis"),0);
    CODES_CHECK(codes_set_missing(h,"scaleFactorOfEarthMinorAxis"),0);
    CODES_CHECK(codes_set_missing(h,"scaledValueOfEarthMinorAxis"),0);
    CODES_CHECK(codes_set_long(h,"radius",6367470),0);
    CODES_CHECK(codes_set_long(h,"Ni",16),0);
    CODES_CHECK(codes_set_long(h,"Nj",31),0);
    CODES_CHECK(codes_set_long(h,"basicAngleOfTheInitialProductionDomain",0),0);
    CODES_CHECK(codes_set_long(h,"mBasicAngle",0),0);
    CODES_CHECK(codes_set_long(h,"angleMultiplier",1),0);
    CODES_CHECK(codes_set_long(h,"mAngleMultiplier",1000000),0);
    CODES_CHECK(codes_set_missing(h,"subdivisionsOfBasicAngle"),0);
    CODES_CHECK(codes_set_long(h,"angleDivisor",1000000),0);
    CODES_CHECK(codes_set_long(h,"latitudeOfFirstGridPoint",60000000),0);
    CODES_CHECK(codes_set_long(h,"longitudeOfFirstGridPoint",0),0);

    /* 48 = 00110000
    (3=1)  i direction increments given
    (4=1)  j direction increments given
    (5=0)  Resolved u- and v- components of vector quantities relative to easterly and northerly directions
    See grib2/tables/[tablesVersion]/3.3.table */
    CODES_CHECK(codes_set_long(h,"resolutionAndComponentFlags",48),0);

    CODES_CHECK(codes_set_long(h,"iDirectionIncrementGiven",1),0);
    CODES_CHECK(codes_set_long(h,"jDirectionIncrementGiven",1),0);
    CODES_CHECK(codes_set_long(h,"uvRelativeToGrid",0),0);
    CODES_CHECK(codes_set_long(h,"latitudeOfLastGridPoint",0),0);
    CODES_CHECK(codes_set_long(h,"longitudeOfLastGridPoint",30000000),0);
    CODES_CHECK(codes_set_long(h,"iDirectionIncrement",2000000),0);
    CODES_CHECK(codes_set_long(h,"jDirectionIncrement",2000000),0);

    /* 0 = 00000000
    (1=0)  Points of first row or column scan in the +i (+x) direction
    (2=0)  Points of first row or column scan in the -j (-y) direction
    (3=0)  Adjacent points in i (x) direction are consecutive
    (4=0)  All rows scan in the same direction
    See grib2/tables/[tablesVersion]/3.4.table */
    CODES_CHECK(codes_set_long(h,"scanningMode",0),0);

    CODES_CHECK(codes_set_long(h,"iScansNegatively",0),0);
    CODES_CHECK(codes_set_long(h,"jScansPositively",0),0);
    CODES_CHECK(codes_set_long(h,"jPointsAreConsecutive",0),0);
    CODES_CHECK(codes_set_long(h,"alternativeRowScanning",0),0);
    CODES_CHECK(codes_set_long(h,"iScansPositively",1),0);

    /* ITERATOR */


    /* NEAREST */

    CODES_CHECK(codes_set_long(h,"timeRangeIndicator",0),0);
    CODES_CHECK(codes_set_long(h,"NV",0),0);
    CODES_CHECK(codes_set_long(h,"neitherPresent",0),0);

    /* 0 = Analysis or forecast at a horizontal level or in a horizontal layer at a point in time (grib2/tables/4/4.0.table)  */
    CODES_CHECK(codes_set_long(h,"productDefinitionTemplateNumber",0),0);


    /* Parameter information */


    /* 0 = Temperature (grib2/tables/4/4.1.0.table)  */
    CODES_CHECK(codes_set_long(h,"parameterCategory",0),0);


    /* 0 = Temperature  (K)  (grib2/tables/4/4.2.0.0.table)  */
    CODES_CHECK(codes_set_long(h,"parameterNumber",0),0);


    /* 0 = Analysis (grib2/tables/4/4.3.table)  */
    CODES_CHECK(codes_set_long(h,"typeOfGeneratingProcess",0),0);

    CODES_CHECK(codes_set_long(h,"backgroundProcess",255),0);
    CODES_CHECK(codes_set_long(h,"generatingProcessIdentifier",128),0);
    CODES_CHECK(codes_set_long(h,"hoursAfterDataCutoff",0),0);
    CODES_CHECK(codes_set_long(h,"minutesAfterDataCutoff",0),0);

    /* 1 = Hour (grib2/tables/4/4.4.table)  */
    CODES_CHECK(codes_set_long(h,"indicatorOfUnitOfTimeRange",1),0);


    /* 1 = Hour (stepUnits.table)  */
    CODES_CHECK(codes_set_long(h,"stepUnits",1),0);

    CODES_CHECK(codes_set_long(h,"forecastTime",0),0);

    /* 1 = Ground or water surface  (grib2/tables/4/4.5.table)  */
    CODES_CHECK(codes_set_long(h,"typeOfFirstFixedSurface",1),0);

    CODES_CHECK(codes_set_missing(h,"scaleFactorOfFirstFixedSurface"),0);
    CODES_CHECK(codes_set_missing(h,"scaledValueOfFirstFixedSurface"),0);

    /* 255 = Missing (grib2/tables/4/4.5.table)  */
    CODES_CHECK(codes_set_long(h,"typeOfSecondFixedSurface",255),0);

    CODES_CHECK(codes_set_missing(h,"scaleFactorOfSecondFixedSurface"),0);
    CODES_CHECK(codes_set_missing(h,"scaledValueOfSecondFixedSurface"),0);
    CODES_CHECK(codes_set_long(h,"level",0),0);
    CODES_CHECK(codes_set_long(h,"bottomLevel",0),0);
    CODES_CHECK(codes_set_long(h,"topLevel",0),0);
    CODES_CHECK(codes_set_long(h,"dummyc",0),0);
    CODES_CHECK(codes_set_long(h,"PVPresent",0),0);

    /* grib 2 Section 5 DATA REPRESENTATION SECTION */

    CODES_CHECK(codes_set_long(h,"numberOfValues",496),0);

    /* 0 = Grid point data - simple packing (grib2/tables/4/5.0.table)  */
    CODES_CHECK(codes_set_long(h,"dataRepresentationTemplateNumber",0),0);

    CODES_CHECK(codes_set_long(h,"decimalScaleFactor",0),0);
    CODES_CHECK(codes_set_long(h,"bitsPerValue",0),0);

    /* 0 = Floating point (grib2/tables/4/5.1.table)  */
    CODES_CHECK(codes_set_long(h,"typeOfOriginalFieldValues",0),0);

    CODES_CHECK(codes_set_long(h,"representationMode",0),0);

    /* grib 2 Section 6 BIT-MAP SECTION */


    /* 255 = A bit map does not apply to this product (grib2/tables/4/6.0.table)  */
    CODES_CHECK(codes_set_long(h,"bitMapIndicator",255),0);

    CODES_CHECK(codes_set_long(h,"bitmapPresent",0),0);

    /* grib 2 Section 7 data */

    size = 496;
    vdouble    = (double*)calloc(size,sizeof(double));
    if(!vdouble) {
        fprintf(stderr,"failed to allocate %lu bytes\n",size*sizeof(double));
        exit(1);
    }

    vdouble[   0] =       1; vdouble[   1] =       1; vdouble[   2] =       1; vdouble[   3] =       1;
    vdouble[   4] =       1; vdouble[   5] =       1; vdouble[   6] =       1; vdouble[   7] =       1;
    vdouble[   8] =       1; vdouble[   9] =       1; vdouble[  10] =       1; vdouble[  11] =       1;
    vdouble[  12] =       1; vdouble[  13] =       1; vdouble[  14] =       1; vdouble[  15] =       1;
    vdouble[  16] =       1; vdouble[  17] =       1; vdouble[  18] =       1; vdouble[  19] =       1;
    vdouble[  20] =       1; vdouble[  21] =       1; vdouble[  22] =       1; vdouble[  23] =       1;
    vdouble[  24] =       1; vdouble[  25] =       1; vdouble[  26] =       1; vdouble[  27] =       1;
    vdouble[  28] =       1; vdouble[  29] =       1; vdouble[  30] =       1; vdouble[  31] =       1;
    vdouble[  32] =       1; vdouble[  33] =       1; vdouble[  34] =       1; vdouble[  35] =       1;
    vdouble[  36] =       1; vdouble[  37] =       1; vdouble[  38] =       1; vdouble[  39] =       1;
    vdouble[  40] =       1; vdouble[  41] =       1; vdouble[  42] =       1; vdouble[  43] =       1;
    vdouble[  44] =       1; vdouble[  45] =       1; vdouble[  46] =       1; vdouble[  47] =       1;
    vdouble[  48] =       1; vdouble[  49] =       1; vdouble[  50] =       1; vdouble[  51] =       1;
    vdouble[  52] =       1; vdouble[  53] =       1; vdouble[  54] =       1; vdouble[  55] =       1;
    vdouble[  56] =       1; vdouble[  57] =       1; vdouble[  58] =       1; vdouble[  59] =       1;
    vdouble[  60] =       1; vdouble[  61] =       1; vdouble[  62] =       1; vdouble[  63] =       1;
    vdouble[  64] =       1; vdouble[  65] =       1; vdouble[  66] =       1; vdouble[  67] =       1;
    vdouble[  68] =       1; vdouble[  69] =       1; vdouble[  70] =       1; vdouble[  71] =       1;
    vdouble[  72] =       1; vdouble[  73] =       1; vdouble[  74] =       1; vdouble[  75] =       1;
    vdouble[  76] =       1; vdouble[  77] =       1; vdouble[  78] =       1; vdouble[  79] =       1;
    vdouble[  80] =       1; vdouble[  81] =       1; vdouble[  82] =       1; vdouble[  83] =       1;
    vdouble[  84] =       1; vdouble[  85] =       1; vdouble[  86] =       1; vdouble[  87] =       1;
    vdouble[  88] =       1; vdouble[  89] =       1; vdouble[  90] =       1; vdouble[  91] =       1;
    vdouble[  92] =       1; vdouble[  93] =       1; vdouble[  94] =       1; vdouble[  95] =       1;
    vdouble[  96] =       1; vdouble[  97] =       1; vdouble[  98] =       1; vdouble[  99] =       1;
    vdouble[ 100] =       1; vdouble[ 101] =       1; vdouble[ 102] =       1; vdouble[ 103] =       1;
    vdouble[ 104] =       1; vdouble[ 105] =       1; vdouble[ 106] =       1; vdouble[ 107] =       1;
    vdouble[ 108] =       1; vdouble[ 109] =       1; vdouble[ 110] =       1; vdouble[ 111] =       1;
    vdouble[ 112] =       1; vdouble[ 113] =       1; vdouble[ 114] =       1; vdouble[ 115] =       1;
    vdouble[ 116] =       1; vdouble[ 117] =       1; vdouble[ 118] =       1; vdouble[ 119] =       1;
    vdouble[ 120] =       1; vdouble[ 121] =       1; vdouble[ 122] =       1; vdouble[ 123] =       1;
    vdouble[ 124] =       1; vdouble[ 125] =       1; vdouble[ 126] =       1; vdouble[ 127] =       1;
    vdouble[ 128] =       1; vdouble[ 129] =       1; vdouble[ 130] =       1; vdouble[ 131] =       1;
    vdouble[ 132] =       1; vdouble[ 133] =       1; vdouble[ 134] =       1; vdouble[ 135] =       1;
    vdouble[ 136] =       1; vdouble[ 137] =       1; vdouble[ 138] =       1; vdouble[ 139] =       1;
    vdouble[ 140] =       1; vdouble[ 141] =       1; vdouble[ 142] =       1; vdouble[ 143] =       1;
    vdouble[ 144] =       1; vdouble[ 145] =       1; vdouble[ 146] =       1; vdouble[ 147] =       1;
    vdouble[ 148] =       1; vdouble[ 149] =       1; vdouble[ 150] =       1; vdouble[ 151] =       1;
    vdouble[ 152] =       1; vdouble[ 153] =       1; vdouble[ 154] =       1; vdouble[ 155] =       1;
    vdouble[ 156] =       1; vdouble[ 157] =       1; vdouble[ 158] =       1; vdouble[ 159] =       1;
    vdouble[ 160] =       1; vdouble[ 161] =       1; vdouble[ 162] =       1; vdouble[ 163] =       1;
    vdouble[ 164] =       1; vdouble[ 165] =       1; vdouble[ 166] =       1; vdouble[ 167] =       1;
    vdouble[ 168] =       1; vdouble[ 169] =       1; vdouble[ 170] =       1; vdouble[ 171] =       1;
    vdouble[ 172] =       1; vdouble[ 173] =       1; vdouble[ 174] =       1; vdouble[ 175] =       1;
    vdouble[ 176] =       1; vdouble[ 177] =       1; vdouble[ 178] =       1; vdouble[ 179] =       1;
    vdouble[ 180] =       1; vdouble[ 181] =       1; vdouble[ 182] =       1; vdouble[ 183] =       1;
    vdouble[ 184] =       1; vdouble[ 185] =       1; vdouble[ 186] =       1; vdouble[ 187] =       1;
    vdouble[ 188] =       1; vdouble[ 189] =       1; vdouble[ 190] =       1; vdouble[ 191] =       1;
    vdouble[ 192] =       1; vdouble[ 193] =       1; vdouble[ 194] =       1; vdouble[ 195] =       1;
    vdouble[ 196] =       1; vdouble[ 197] =       1; vdouble[ 198] =       1; vdouble[ 199] =       1;
    vdouble[ 200] =       1; vdouble[ 201] =       1; vdouble[ 202] =       1; vdouble[ 203] =       1;
    vdouble[ 204] =       1; vdouble[ 205] =       1; vdouble[ 206] =       1; vdouble[ 207] =       1;
    vdouble[ 208] =       1; vdouble[ 209] =       1; vdouble[ 210] =       1; vdouble[ 211] =       1;
    vdouble[ 212] =       1; vdouble[ 213] =       1; vdouble[ 214] =       1; vdouble[ 215] =       1;
    vdouble[ 216] =       1; vdouble[ 217] =       1; vdouble[ 218] =       1; vdouble[ 219] =       1;
    vdouble[ 220] =       1; vdouble[ 221] =       1; vdouble[ 222] =       1; vdouble[ 223] =       1;
    vdouble[ 224] =       1; vdouble[ 225] =       1; vdouble[ 226] =       1; vdouble[ 227] =       1;
    vdouble[ 228] =       1; vdouble[ 229] =       1; vdouble[ 230] =       1; vdouble[ 231] =       1;
    vdouble[ 232] =       1; vdouble[ 233] =       1; vdouble[ 234] =       1; vdouble[ 235] =       1;
    vdouble[ 236] =       1; vdouble[ 237] =       1; vdouble[ 238] =       1; vdouble[ 239] =       1;
    vdouble[ 240] =       1; vdouble[ 241] =       1; vdouble[ 242] =       1; vdouble[ 243] =       1;
    vdouble[ 244] =       1; vdouble[ 245] =       1; vdouble[ 246] =       1; vdouble[ 247] =       1;
    vdouble[ 248] =       1; vdouble[ 249] =       1; vdouble[ 250] =       1; vdouble[ 251] =       1;
    vdouble[ 252] =       1; vdouble[ 253] =       1; vdouble[ 254] =       1; vdouble[ 255] =       1;
    vdouble[ 256] =       1; vdouble[ 257] =       1; vdouble[ 258] =       1; vdouble[ 259] =       1;
    vdouble[ 260] =       1; vdouble[ 261] =       1; vdouble[ 262] =       1; vdouble[ 263] =       1;
    vdouble[ 264] =       1; vdouble[ 265] =       1; vdouble[ 266] =       1; vdouble[ 267] =       1;
    vdouble[ 268] =       1; vdouble[ 269] =       1; vdouble[ 270] =       1; vdouble[ 271] =       1;
    vdouble[ 272] =       1; vdouble[ 273] =       1; vdouble[ 274] =       1; vdouble[ 275] =       1;
    vdouble[ 276] =       1; vdouble[ 277] =       1; vdouble[ 278] =       1; vdouble[ 279] =       1;
    vdouble[ 280] =       1; vdouble[ 281] =       1; vdouble[ 282] =       1; vdouble[ 283] =       1;
    vdouble[ 284] =       1; vdouble[ 285] =       1; vdouble[ 286] =       1; vdouble[ 287] =       1;
    vdouble[ 288] =       1; vdouble[ 289] =       1; vdouble[ 290] =       1; vdouble[ 291] =       1;
    vdouble[ 292] =       1; vdouble[ 293] =       1; vdouble[ 294] =       1; vdouble[ 295] =       1;
    vdouble[ 296] =       1; vdouble[ 297] =       1; vdouble[ 298] =       1; vdouble[ 299] =       1;
    vdouble[ 300] =       1; vdouble[ 301] =       1; vdouble[ 302] =       1; vdouble[ 303] =       1;
    vdouble[ 304] =       1; vdouble[ 305] =       1; vdouble[ 306] =       1; vdouble[ 307] =       1;
    vdouble[ 308] =       1; vdouble[ 309] =       1; vdouble[ 310] =       1; vdouble[ 311] =       1;
    vdouble[ 312] =       1; vdouble[ 313] =       1; vdouble[ 314] =       1; vdouble[ 315] =       1;
    vdouble[ 316] =       1; vdouble[ 317] =       1; vdouble[ 318] =       1; vdouble[ 319] =       1;
    vdouble[ 320] =       1; vdouble[ 321] =       1; vdouble[ 322] =       1; vdouble[ 323] =       1;
    vdouble[ 324] =       1; vdouble[ 325] =       1; vdouble[ 326] =       1; vdouble[ 327] =       1;
    vdouble[ 328] =       1; vdouble[ 329] =       1; vdouble[ 330] =       1; vdouble[ 331] =       1;
    vdouble[ 332] =       1; vdouble[ 333] =       1; vdouble[ 334] =       1; vdouble[ 335] =       1;
    vdouble[ 336] =       1; vdouble[ 337] =       1; vdouble[ 338] =       1; vdouble[ 339] =       1;
    vdouble[ 340] =       1; vdouble[ 341] =       1; vdouble[ 342] =       1; vdouble[ 343] =       1;
    vdouble[ 344] =       1; vdouble[ 345] =       1; vdouble[ 346] =       1; vdouble[ 347] =       1;
    vdouble[ 348] =       1; vdouble[ 349] =       1; vdouble[ 350] =       1; vdouble[ 351] =       1;
    vdouble[ 352] =       1; vdouble[ 353] =       1; vdouble[ 354] =       1; vdouble[ 355] =       1;
    vdouble[ 356] =       1; vdouble[ 357] =       1; vdouble[ 358] =       1; vdouble[ 359] =       1;
    vdouble[ 360] =       1; vdouble[ 361] =       1; vdouble[ 362] =       1; vdouble[ 363] =       1;
    vdouble[ 364] =       1; vdouble[ 365] =       1; vdouble[ 366] =       1; vdouble[ 367] =       1;
    vdouble[ 368] =       1; vdouble[ 369] =       1; vdouble[ 370] =       1; vdouble[ 371] =       1;
    vdouble[ 372] =       1; vdouble[ 373] =       1; vdouble[ 374] =       1; vdouble[ 375] =       1;
    vdouble[ 376] =       1; vdouble[ 377] =       1; vdouble[ 378] =       1; vdouble[ 379] =       1;
    vdouble[ 380] =       1; vdouble[ 381] =       1; vdouble[ 382] =       1; vdouble[ 383] =       1;
    vdouble[ 384] =       1; vdouble[ 385] =       1; vdouble[ 386] =       1; vdouble[ 387] =       1;
    vdouble[ 388] =       1; vdouble[ 389] =       1; vdouble[ 390] =       1; vdouble[ 391] =       1;
    vdouble[ 392] =       1; vdouble[ 393] =       1; vdouble[ 394] =       1; vdouble[ 395] =       1;
    vdouble[ 396] =       1; vdouble[ 397] =       1; vdouble[ 398] =       1; vdouble[ 399] =       1;
    vdouble[ 400] =       1; vdouble[ 401] =       1; vdouble[ 402] =       1; vdouble[ 403] =       1;
    vdouble[ 404] =       1; vdouble[ 405] =       1; vdouble[ 406] =       1; vdouble[ 407] =       1;
    vdouble[ 408] =       1; vdouble[ 409] =       1; vdouble[ 410] =       1; vdouble[ 411] =       1;
    vdouble[ 412] =       1; vdouble[ 413] =       1; vdouble[ 414] =       1; vdouble[ 415] =       1;
    vdouble[ 416] =       1; vdouble[ 417] =       1; vdouble[ 418] =       1; vdouble[ 419] =       1;
    vdouble[ 420] =       1; vdouble[ 421] =       1; vdouble[ 422] =       1; vdouble[ 423] =       1;
    vdouble[ 424] =       1; vdouble[ 425] =       1; vdouble[ 426] =       1; vdouble[ 427] =       1;
    vdouble[ 428] =       1; vdouble[ 429] =       1; vdouble[ 430] =       1; vdouble[ 431] =       1;
    vdouble[ 432] =       1; vdouble[ 433] =       1; vdouble[ 434] =       1; vdouble[ 435] =       1;
    vdouble[ 436] =       1; vdouble[ 437] =       1; vdouble[ 438] =       1; vdouble[ 439] =       1;
    vdouble[ 440] =       1; vdouble[ 441] =       1; vdouble[ 442] =       1; vdouble[ 443] =       1;
    vdouble[ 444] =       1; vdouble[ 445] =       1; vdouble[ 446] =       1; vdouble[ 447] =       1;
    vdouble[ 448] =       1; vdouble[ 449] =       1; vdouble[ 450] =       1; vdouble[ 451] =       1;
    vdouble[ 452] =       1; vdouble[ 453] =       1; vdouble[ 454] =       1; vdouble[ 455] =       1;
    vdouble[ 456] =       1; vdouble[ 457] =       1; vdouble[ 458] =       1; vdouble[ 459] =       1;
    vdouble[ 460] =       1; vdouble[ 461] =       1; vdouble[ 462] =       1; vdouble[ 463] =       1;
    vdouble[ 464] =       1; vdouble[ 465] =       1; vdouble[ 466] =       1; vdouble[ 467] =       1;
    vdouble[ 468] =       1; vdouble[ 469] =       1; vdouble[ 470] =       1; vdouble[ 471] =       1;
    vdouble[ 472] =       1; vdouble[ 473] =       1; vdouble[ 474] =       1; vdouble[ 475] =       1;
    vdouble[ 476] =       1; vdouble[ 477] =       1; vdouble[ 478] =       1; vdouble[ 479] =       1;
    vdouble[ 480] =       1; vdouble[ 481] =       1; vdouble[ 482] =       1; vdouble[ 483] =       1;
    vdouble[ 484] =       1; vdouble[ 485] =       1; vdouble[ 486] =       1; vdouble[ 487] =       1;
    vdouble[ 488] =       1; vdouble[ 489] =       1; vdouble[ 490] =       1; vdouble[ 491] =       1;
    vdouble[ 492] =       1; vdouble[ 493] =       1; vdouble[ 494] =       1; vdouble[ 495] =       1;

    CODES_CHECK(codes_set_double_array(h,"values",vdouble,size),0);
    free(vdouble);
    CODES_CHECK(codes_set_long(h,"dirty_statistics",1),0);
    CODES_CHECK(codes_set_long(h,"changeDecimalPrecision",0),0);
    CODES_CHECK(codes_set_long(h,"decimalPrecision",0),0);
    CODES_CHECK(codes_set_long(h,"setBitsPerValue",0),0);
/* Save the message */

    f = fopen(argv[1],"w");
    if(!f) {
        perror(argv[1]);
        exit(1);
    }

    CODES_CHECK(codes_get_message(h,&buffer,&size),0);

    if(fwrite(buffer,1,size,f) != size) {
        perror(argv[1]);
        exit(1);
    }

    if(fclose(f)) {
        perror(argv[1]);
        exit(1);
    }

    codes_handle_delete(h);
    return 0;
}
