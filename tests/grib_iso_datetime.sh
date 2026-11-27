#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.ctest.sh

# Read-only ISO 8601 keys dataDateTimeISO and validityDateTimeISO

label="grib_iso_datetime_test"
temp=temp.$label.grib
temp2=temp2.$label.grib
tempFilt=temp.$label.filt
tempOut=temp.$label.txt

sample1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
keys='dataDateTimeISO,validityDateTimeISO'

# check <input> <set-keys> <keys> <expected>
check()
{
    ${tools_dir}/grib_set -s $2 $1 $temp 2>/dev/null
    grib_check_key_equals $temp $3 "$4"
}

# check_error <input> <set-keys> <key> <expected error text>
check_error()
{
    ${tools_dir}/grib_set -s $2 $1 $temp 2>/dev/null
    set +e
    ${tools_dir}/grib_get -p $3 $temp > $tempOut 2>&1
    status=$?
    set -e
    [ $status -ne 0 ]
    grep -q "$4" $tempOut
}

for sample in $sample1 $sample2; do
    # Analysis; forecast across the end of the year; leap day; sub-hourly step
    check $sample dataDate=20261121,dataTime=1130,step=0 $keys '2026-11-21T11:30:00+00:00 2026-11-21T11:30:00+00:00'
    check $sample dataDate=20261231,dataTime=1800,step=36 $keys '2026-12-31T18:00:00+00:00 2027-01-02T06:00:00+00:00'
    check $sample dataDate=20240228,dataTime=1200,step=24 validityDateTimeISO '2024-02-29T12:00:00+00:00'
    check $sample dataDate=20261121,dataTime=1130,stepUnits=m,step=45 validityDateTimeISO '2026-11-21T12:15:00+00:00'
    # 24:00 (accepted by ecCodes) is 00:00 of the next day
    check $sample dataDate=20261231,dataTime=2400 $keys '2027-01-01T00:00:00+00:00 2027-01-01T00:00:00+00:00'

    # The keys follow changes of the underlying keys in the same handle
    cat > $tempFilt <<EOF
    set dataDate = 20200101;
    set dataTime = 600;
    set step = 6;
    print "[dataDateTimeISO] [validityDateTimeISO]";
    set month = 3;
    set hour = 23;
    print "[dataDateTimeISO] [validityDateTimeISO]";
EOF
    ${tools_dir}/grib_filter $tempFilt $sample > $tempOut
    cat > $temp2 <<EOF
2020-01-01T06:00:00+00:00 2020-01-01T12:00:00+00:00
2020-03-01T23:00:00+00:00 2020-03-02T05:00:00+00:00
EOF
    diff $temp2 $tempOut

    # Not in any namespace: the time namespace is unchanged
    ${tools_dir}/grib_get -n time $sample > $tempOut
    if grep -q "+00:00" $tempOut; then exit 1; fi

    # Read-only
    for key in dataDateTimeISO validityDateTimeISO; do
        set +e
        ${tools_dir}/grib_set -s $key=2026-11-21T11:30:00+00:00 $sample $temp 2> $tempOut
        status=$?
        set -e
        [ $status -ne 0 ]
        grep -q "Value is read only" $tempOut
    done

    # Invalid date or time
    check_error $sample month=13 dataDateTimeISO "Invalid date/time"
    check_error $sample month=2,day=31 dataDateTimeISO "Invalid date/time"
    check_error $sample day=0 dataDateTimeISO "Invalid date/time"
    check_error $sample hour=25 dataDateTimeISO "Invalid date/time"
    check_error $sample minute=61 dataDateTimeISO "Invalid date/time"
done

# GRIB1: statistically processed field; date without a year (yearOfCentury=255)
check $sample1 dataDate=20261121,dataTime=0,stepType=accum,stepRange=6-30 $keys '2026-11-21T00:00:00+00:00 2026-11-22T06:00:00+00:00'
check_error $sample1 yearOfCentury=255 dataDateTimeISO "Invalid date/time"

# GRIB2: negative step; steps in days and months
check $sample2 dataDate=20260101,dataTime=0000,stepUnits=h,forecastTime=-1 validityDateTimeISO '2025-12-31T23:00:00+00:00'
check $sample2 dataDate=20261121,dataTime=1130,stepUnits=D,step=10 validityDateTimeISO '2026-12-01T11:30:00+00:00'
${tools_dir}/grib_set -s dataDate=20261121,dataTime=1130,stepUnits=M,step=2 $sample2 $temp
vdt=`${tools_dir}/grib_get -p validityDate,validityTime:s $temp | tr -d ' '`
iso=`${tools_dir}/grib_get -p validityDateTimeISO $temp | sed 's/[-:T]//g' | cut -c1-12`
[ "$iso" = "$vdt" ]

# GRIB2: seconds of the reference time and steps in seconds (exact to the second,
# although validityTime has only minutes)
check $sample2 dataDate=20261121,hour=11,minute=30,second=15,stepUnits=m,step=90 $keys '2026-11-21T11:30:15+00:00 2026-11-21T13:00:15+00:00'
check $sample2 dataDate=20261121,hour=11,minute=30,second=45,stepUnits=s,step=90 validityDateTimeISO '2026-11-21T11:32:15+00:00'
check $sample2 dataDate=20261231,hour=23,minute=59,second=59,stepUnits=s,step=59 validityDateTimeISO '2027-01-01T00:00:58+00:00'
check $sample2 dataDate=20261121,hour=11,minute=30,second=0,stepUnits=s,step=3601 validityDateTimeISO '2026-11-21T12:30:01+00:00'

# GRIB2: statistically processed field (validity = end of the overall time interval)
${tools_dir}/grib_set -s productDefinitionTemplateNumber=8 $sample2 $temp2
check $temp2 dataDate=20261121,dataTime=0,stepType=accum,stepRange=0-30 $keys '2026-11-21T00:00:00+00:00 2026-11-22T06:00:00+00:00'
check $temp2 dataDate=20261121,dataTime=0,stepType=accum,stepRange=0-6,secondOfEndOfOverallTimeInterval=30 validityDateTimeISO '2026-11-21T06:00:30+00:00'

# GRIB2: template without a validity time (PDT 20): no validityDateTimeISO
check $sample2 productDefinitionTemplateNumber=20 dataDateTimeISO '2007-03-23T12:00:00+00:00'
check_error $sample2 productDefinitionTemplateNumber=20 validityDateTimeISO "not found"

# GRIB2: the local time template (PDT 88) redefines dataDate as the date of the forecast used.
# dataDateTimeISO is still the reference time of section 1.
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
    set productDefinitionTemplateNumber = 88;
    set numberOfForecastsUsedInLocalTime = 1;
    set forecastTime = 24;
    set localTimeMethod = 1;
    set yearOfForecastUsedInLocalTime  = 2020;
    set monthOfForecastUsedInLocalTime = 8;
    set dayOfForecastUsedInLocalTime   = 4;
    write;
EOF
${tools_dir}/grib_filter -o $temp $tempFilt $sample2 2>/dev/null
grib_check_key_equals $temp dataDate,dataDateTimeISO '20200804 2020-08-05T12:00:00+00:00'

rm -f $temp $temp2 $tempFilt $tempOut
