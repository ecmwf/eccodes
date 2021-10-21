#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
set -u

label="grib_ecc-1212-test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
tempOut=temp.$label.out
tempRef=temp.$label.ref
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Create a GRIB with the local time template
cat > $tempFilt <<EOF
    set setLocalDefinition=1;
    set localDefinitionNumber = 1;

    set tablesVersion=27;
    set localTablesVersion = 1;
    set typeOfProcessedData = "fc";

    set significanceOfReferenceTime = 4;
    set year = 2020;
    set month = 8;
    set day = 5;
    set generatingProcessIdentifier = 254;

    set productDefinitionTemplateNumber = 88;
    set numberOfForecastsUsedInLocalTime = 1;
    set forecastTime = 24;

    set localTimeMethod = 1;
    set yearOfForecastUsedInLocalTime  = 2020;
    set monthOfForecastUsedInLocalTime = 8;
    set dayOfForecastUsedInLocalTime   = 4;
    set numberOfTimeIncrementsOfForecastsUsedInLocalTime = 8;
    set indicatorOfUnitForTimeIncrement = 1;
    set timeIncrement = 3;
    write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2

# Check MARS keys
${tools_dir}/grib_ls -j -m $tempGrib > $tempOut
cat > $tempRef <<EOF
{ "messages" : [ 
  {
    "domain": "g",
    "expver": "0001",
    "class": "od",
    "type": "an",
    "stream": "oper",
    "levtype": "sfc",
    "date": 20200804,
    "time": "0000",
    "step": 36,
    "param": 130
  }
]}
EOF
cat $tempOut
diff $tempRef $tempOut
grib_check_key_equals $tempGrib dateOfForecastUsedInLocalTime '20200804'
grib_check_key_equals $tempGrib timeOfForecastUsedInLocalTime '0'


# Check "ls" namespace
${tools_dir}/grib_get -n ls $tempGrib > $tempOut
#     edition centre date     time dataType gridType   typeOfLevel level shortName packingType
echo "2       ecmf   20200805 1200 an       regular_ll surface     0     t         grid_simple" > $tempRef
cat $tempOut
diff -w $tempRef $tempOut


# Clean up
rm -f $tempGrib $tempFilt $tempOut $tempRef
