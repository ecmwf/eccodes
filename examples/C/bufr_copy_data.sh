#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh
set -x

TEMP=out.bufr
REF=compare.log.ref
MYLOG=compare.log

rm -f ${TEMP} ${REF} ${MYLOG} | true

cat > ${REF} <<EOF
== 1 == DIFFERENCE == Different size for "unexpandedDescriptors"  [43]  [28]
== 1 == DIFFERENCE == [#2#icaoLocationIndicator] not found in 2nd field
== 1 == DIFFERENCE == [#2#icaoLocationIndicator->units] not found in 2nd field
== 1 == DIFFERENCE == [#2#stationType] not found in 2nd field
== 1 == DIFFERENCE == [#2#stationType->units] not found in 2nd field
== 1 == DIFFERENCE == [#2#year] not found in 2nd field
== 1 == DIFFERENCE == [#2#year->units] not found in 2nd field
== 1 == DIFFERENCE == [#2#month] not found in 2nd field
== 1 == DIFFERENCE == [#2#month->units] not found in 2nd field
== 1 == DIFFERENCE == [#2#day] not found in 2nd field
== 1 == DIFFERENCE == [#2#day->units] not found in 2nd field
== 1 == DIFFERENCE == [#2#hour] not found in 2nd field
== 1 == DIFFERENCE == [#2#hour->units] not found in 2nd field
== 1 == DIFFERENCE == [#2#minute] not found in 2nd field
== 1 == DIFFERENCE == [#2#minute->units] not found in 2nd field
== 1 == DIFFERENCE == [#2#latitude] not found in 2nd field
== 1 == DIFFERENCE == [#2#latitude->units] not found in 2nd field
== 1 == DIFFERENCE == [#2#longitude] not found in 2nd field
== 1 == DIFFERENCE == [#2#longitude->units] not found in 2nd field
== 1 == DIFFERENCE == [#2#heightOfStation] not found in 2nd field
== 1 == DIFFERENCE == [#2#heightOfStation->units] not found in 2nd field
== 1 == DIFFERENCE == [#4#heightAboveStation] not found in 2nd field
== 1 == DIFFERENCE == [#4#heightAboveStation->units] not found in 2nd field
== 1 == DIFFERENCE == [#2#windDirection] not found in 2nd field
== 1 == DIFFERENCE == [#2#windDirection->units] not found in 2nd field
== 1 == DIFFERENCE == [#2#extremeCounterclockwiseWindDirectionOfAVariableWind] not found in 2nd field
== 1 == DIFFERENCE == [#2#extremeCounterclockwiseWindDirectionOfAVariableWind->units] not found in 2nd field
== 1 == DIFFERENCE == [#2#extremeClockwiseWindDirectionOfAVariableWind] not found in 2nd field
== 1 == DIFFERENCE == [#2#extremeClockwiseWindDirectionOfAVariableWind->units] not found in 2nd field
== 1 == DIFFERENCE == [#2#windSpeed] not found in 2nd field
== 1 == DIFFERENCE == [#2#windSpeed->units] not found in 2nd field
EOF

INPUT=${data_dir}/bufr/metar_with_2_bias.bufr
${examples_dir}c_bufr_copy_data ${INPUT} ${TEMP}  > /dev/null
${tools_dir}bufr_compare ${TEMP} ${INPUT} > ${MYLOG} | true
diff ${MYLOG} ${REF}

rm -f ${TEMP} ${REF} ${MYLOG}
