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

result=$(${tools_dir}/codes_interpreter --help 2>&1)
printf '%s\n' "$result" | grep -q 'Usage:'
printf '%s\n' "$result" | grep -q -- '--non-fail'

result=$(printf 'print "[edition]";\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl")
printf '%s\n' "$result" | grep -q '^2$'

result=$(printf 'if (edition == 2) { print "OK"; }\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl")
printf '%s\n' "$result" | grep -q '^OK$'

result=$(printf 'meta dateOfForecast validity_date(dataDate,dataTime,anoffset);\nprint "[edition]";\nquit\n' | ${tools_dir}/codes_interpreter --non-fail "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q 'ECCODES ERROR'
printf '%s\n' "$result" | grep -q '2'

result=$(printf 'meta sum_of_pl_array sum(pl);\nprint "[sum_of_pl_array]";\nquit\n' | ${tools_dir}/codes_interpreter --non-fail "$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q 'sum_of_pl_array'
printf '%s\n' "$result" | grep -q '6114'

result=$(printf 'print "ED=[edition]";\ntransient q = 24;\nprint "Q=[q]";\nquit\n' | ${tools_dir}/codes_interpreter --non-fail "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
[ "$(printf '%s\n' "$result" | grep -c '^ED=2$')" = "1" ]
[ "$(printf '%s\n' "$result" | grep -c '^Q=24$')" = "1" ]
