#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

#Define a common label for all the tmp files
label="bufr_copy_data_py"

TEMP=$label.out.bufr
REF=$label.compare.log.ref
MYLOG=$label.compare.log

cd ${data_dir}/bufr
rm -f ${TEMP} ${REF} ${MYLOG}

cat > ${REF} <<EOF
== 1 == DIFFERENCE == Different size for "unexpandedDescriptors"  [43]  [28]
== 1 == DIFFERENCE == [#2#icaoLocationIndicator] not found in 2nd field
== 1 == DIFFERENCE == [#2#stationType] not found in 2nd field
== 1 == DIFFERENCE == [#2#year] not found in 2nd field
== 1 == DIFFERENCE == [#2#month] not found in 2nd field
== 1 == DIFFERENCE == [#2#day] not found in 2nd field
== 1 == DIFFERENCE == [#2#hour] not found in 2nd field
== 1 == DIFFERENCE == [#2#minute] not found in 2nd field
== 1 == DIFFERENCE == [#2#latitude] not found in 2nd field
== 1 == DIFFERENCE == [#2#longitude] not found in 2nd field
== 1 == DIFFERENCE == [#2#heightOfStation] not found in 2nd field
== 1 == DIFFERENCE == [#4#heightAboveStation] not found in 2nd field
== 1 == DIFFERENCE == [#2#windDirection] not found in 2nd field
== 1 == DIFFERENCE == [#2#extremeCounterclockwiseWindDirectionOfAVariableWind] not found in 2nd field
== 1 == DIFFERENCE == [#2#extremeClockwiseWindDirectionOfAVariableWind] not found in 2nd field
== 1 == DIFFERENCE == [#2#windSpeed] not found in 2nd field
EOF

INPUT=metar_with_2_bias.bufr
$PYTHON ${examples_src}/bufr_copy_data.py ${INPUT} ${TEMP}
# The input and output BUFR messages should be different
set +e
${tools_dir}/bufr_compare ${TEMP} ${INPUT} > ${MYLOG}
status=$?
set -e
[ $status -eq 1 ]

diff ${MYLOG} ${REF}

# Test for operator 203YYY
# --------------------------
INPUT=israel_observations_2017041010.bufr
rm -f ${TEMP}
$PYTHON ${examples_src}/bufr_copy_data2.py ${INPUT} ${TEMP}
${tools_dir}/bufr_compare -b meanWindDirectionForSurfaceTo1500M,unexpandedDescriptors ${TEMP} ${INPUT}

# WIGOS test
INPUT=synop_multi_subset.bufr
rm -f ${TEMP}
$PYTHON ${examples_src}/bufr_copy_data3.py ${INPUT} ${TEMP}
val=`${tools_dir}/bufr_get -s unpack=1 -p '#2#wigosIssuerOfIdentifier' ${TEMP}`
[ "$val" = "6" ]
val=`${tools_dir}/bufr_get -s unpack=1 -p '#2#stationOrSiteName' ${TEMP}`
[ "$val" = "PASVIK" ]


# Clean up
rm -f ${TEMP} ${REF} ${MYLOG}
