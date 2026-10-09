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

# ---------------------------------------------------------
# This is the test for JIRA issue ECC-2342
# stepRange/endStep with two time ranges
#  - switches off (MTG2Switch=0 and typeOfTimeIncrementSwitch=0):
#    backwards-compatible, i.e. the first time range with typeOfTimeIncrement=2 is used
#  - switches on (MTG2Switch!=0 or typeOfTimeIncrementSwitch!=0):
#    the largest time range with typeOfTimeIncrement=1, 2, 3, 4 or 5 wins
# ---------------------------------------------------------

label="grib_ecc-2342_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
tempOut=temp.$label.txt

${tools_dir}/grib_set -s productDefinitionTemplateNumber=8 $ECCODES_SAMPLES_PATH/GRIB2.tmpl $tempGrib

# Arguments:
#   switches:            off or on. With "on" both MTG2Switch=1 and typeOfTimeIncrementSwitch=1 are tried
#   start:               forecast time with its unit e.g., 6h, 30m
#   typeOfTimeIncrement: one per time range e.g., "1, 2"
#   lengthOfTimeRange:   one per time range, with its unit e.g., "744h, 24h"
#   stepRange:           the expected stepRange e.g., 6-30, 30m-45m, or "error"
check()
{
    switches=$1
    start=$2
    types=$3
    ranges=$4
    expected=$5
    this="switches=$switches start=$start typeOfTimeIncrement={$types} lengthOfTimeRange={$ranges}"

    # "744h, 24h" -> lengths: 744, 24 and units: "h", "h"
    lengths=`echo "$ranges" | tr -d 'a-zA-Z'`
    units=`echo "$ranges" | sed 's/[0-9]*\([a-zA-Z]\)/"\1"/g'`

    # The other step keys must agree with the stepRange.
    # 30m-45m -> endStep: 45m, endStep as double: 45 and stepUnits: m
    end=${expected#*-}
    endValue=`echo $end | tr -d 'a-zA-Z'`
    unit=`echo $end | tr -d '0-9'`
    [ -z "$unit" ] && unit=h

    enablers="none"
    [ $switches = on ] && enablers="tablesVersion=35 typeOfTimeIncrementSwitch=1"

    for enabler in $enablers; do
        cat > $tempFilt <<EOF
set numberOfTimeRanges = `echo $types | wc -w`;
set indicatorOfUnitForForecastTime = "`echo $start | tr -d '0-9'`";
set forecastTime = `echo $start | tr -d 'a-zA-Z'`;
set typeOfTimeIncrement = {$types};
set indicatorOfUnitForTimeRange = {$units};
set lengthOfTimeRange = {$lengths};
EOF
        if [ $switches = on ]; then
            echo "set $enabler;"                                              >> $tempFilt
            echo "assert( MTG2Switch != 0 || typeOfTimeIncrementSwitch != 0 );" >> $tempFilt
        else
            echo "assert( MTG2Switch == 0 && typeOfTimeIncrementSwitch == 0 );" >> $tempFilt
        fi
        echo 'print "[stepRange] [endStep] [endStep:d] [stepUnits:s]";'      >> $tempFilt

        set +e
        ${tools_dir}/grib_filter $tempFilt $tempGrib > $tempOut 2>&1
        status=$?
        set -e
        result=`cat $tempOut`

        if [ "$expected" = "error" ]; then
            if [ $status -eq 0 ]; then
                echo "$this: expected an error but got '$result'"
                exit 1
            fi
            grep -q "Cannot calculate endStep" $tempOut
        elif [ "$result" != "$expected $end $endValue $unit" ]; then
            echo "$this: expected '$expected $end $endValue $unit' but got '$result'"
            exit 1
        fi
    done
}

#     switches start typeOfTimeIncrement lengthOfTimeRange stepRange

# Switches off: the first time range with typeOfTimeIncrement=2 is used,
# whether it is the largest one or not
check off      6h    "1, 2"              "744h, 24h"       6-30
check off      6h    "1, 2"              "24h, 744h"       6-750
check off      6h    "2, 1"              "744h, 24h"       6-750
check off      6h    "2, 1"              "24h, 744h"       6-30
check off      6h    "2, 2"              "744h, 24h"       6-750
check off      6h    "2, 2"              "24h, 744h"       6-30
check off      6h    "3, 2"              "744h, 24h"       6-30
check off      6h    "2, 5"              "24h, 744h"       6-30

# Switches off: without typeOfTimeIncrement=2 it is an error
check off      6h    "1, 1"              "744h, 24h"       error
check off      6h    "3, 4"              "744h, 24h"       error
check off      6h    "5, 5"              "744h, 24h"       error
check off      6h    "255, 255"          "744h, 24h"       error

# Switches on: the largest time range wins
check on       6h    "1, 2"              "744h, 24h"       6-750
check on       6h    "1, 2"              "24h, 744h"       6-750
check on       6h    "2, 1"              "24h, 744h"       6-750
check on       6h    "2, 2"              "24h, 744h"       6-750
check on       6h    "1, 1"              "744h, 24h"       6-750

# Switches on: typeOfTimeIncrement 3, 4 and 5 take part too
check on       6h    "3, 2"              "744h, 24h"       6-750
check on       6h    "2, 4"              "24h, 744h"       6-750
check on       6h    "1, 5"              "24h, 744h"       6-750
check on       6h    "3, 3"              "744h, 24h"       6-750
check on       6h    "4, 4"              "24h, 744h"       6-750
check on       6h    "5, 5"              "744h, 24h"       6-750
check on       6h    "5, 3"              "24h, 744h"       6-750

# Switches on: the other values of typeOfTimeIncrement do not take part
check on       6h    "255, 2"            "744h, 24h"       6-30
check on       6h    "6, 2"              "744h, 24h"       6-30
check on       6h    "0, 5"              "744h, 24h"       6-30
check on       6h    "255, 255"          "744h, 24h"       error

# Sub-hourly
check off      30m   "1, 2"              "90m, 15m"        30m-45m
check off      30m   "2, 2"              "15m, 90m"        30m-45m
check off      30m   "3, 2"              "90m, 15m"        30m-45m
check on       30m   "1, 2"              "90m, 15m"        30m-120m
check on       30m   "2, 2"              "15m, 90m"        30m-120m
check on       30m   "3, 2"              "90m, 15m"        30m-120m
check on       30m   "5, 4"              "900s, 5400s"     30m-120m

# Time ranges with different units
check off      30m   "1, 2"              "2h, 15m"         30m-45m
check off      30m   "2, 2"              "900s, 45m"       30m-45m
check on       30m   "1, 2"              "2h, 15m"         30m-150m
check on       30m   "1, 2"              "45m, 900s"       30m-75m
check on       30m   "2, 2"              "900s, 45m"       30m-75m

# The steps are in the unit of the time ranges
check off      30m   "2, 2"              "90s, 30s"        1800s-1890s
check off      30m   "1, 2"              "90s, 30s"        1800s-1830s
check on       30m   "1, 2"              "90s, 30s"        1800s-1890s
check off      6h    "2, 2"              "90m, 30m"        360m-450m
check off      6h    "1, 2"              "90m, 30m"        360m-390m
check on       6h    "1, 2"              "90m, 30m"        360m-450m

# One time range: lengthOfTimeRange is added for typeOfTimeIncrement 3, 4 and 5
for type in 3 4 5; do
    check off  6h    "$type"             "24h"             6-30
    check on   6h    "$type"             "24h"             6-30
done

# Clean up
rm -f $tempGrib $tempFilt $tempOut
