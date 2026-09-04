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

label="codes_interpreter_test"
tempMulti=temp.$label.multi.grib

result=$(${tools_dir}/codes_interpreter --help 2>&1)
printf '%s\n' "$result" | grep -q 'Usage:'
printf '%s\n' "$result" | grep -q -- '--non-fail'

result=$(printf 'print "[edition]";\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl")
printf '%s\n' "$result" | grep -q '^2$'

result=$(printf 'if (edition == 2) { print "OK"; }\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl")
printf '%s\n' "$result" | grep -q '^OK$'

result=$(printf ':accessors\nquit\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Accessors ('

result=$(printf ':accessors count_\nquit\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Accessors matching /count_/'
printf '%s\n' "$result" | grep -q '^  count_file$'

result=$(printf ':changes\nquit\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q 'not activated - use --log-key-changes'

result=$(printf ':list\nquit\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Keys ('

result=$(printf ':list ^editionNumber$\nquit\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Keys matching /\^editionNumber\$/'
printf '%s\n' "$result" | grep -q '^  editionNumber$'

result=$(printf 'transient toffset = 18;\n:changes\nquit\n' | ${tools_dir}/codes_interpreter --log-key-changes "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Changed keys ('
printf '%s\n' "$result" | grep -q '^  toffset: '

result=$(printf 'transient toffset = 18;\n:changes ^toff\nquit\n' | ${tools_dir}/codes_interpreter --log-key-changes "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Changed keys matching /\^toff/'
printf '%s\n' "$result" | grep -q '^  toffset: '

result=$(printf 'meta d1 validity_date(dataDate,dataTime,step,stepUnits);\n:changes\nquit\n' | ${tools_dir}/codes_interpreter --log-key-changes "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^  d1: '

result=$(printf ':accessors [\nquit\n' | ${tools_dir}/codes_interpreter --non-fail "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q 'invalid regex'

result=$(printf ':list [\nquit\n' | ${tools_dir}/codes_interpreter --non-fail "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q 'invalid regex'

result=$(printf 'transient toffset = 18;\n:changes [\nquit\n' | ${tools_dir}/codes_interpreter --non-fail --log-key-changes "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q 'invalid regex'

result=$(printf 'print "[does_not_exist_key]";\nprint "[edition]";\nquit\n' | ${tools_dir}/codes_interpreter --non-fail "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q 'Key/value not found'
printf '%s\n' "$result" | grep -q '2'

result=$(printf 'meta sum_of_pl_array sum(pl);\nprint "[sum_of_pl_array]";\nquit\n' | ${tools_dir}/codes_interpreter --non-fail "$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q 'sum_of_pl_array'
printf '%s\n' "$result" | grep -q '6114'

result=$(printf 'print "ED=[edition]";\ntransient q = 24;\nprint "Q=[q]";\nquit\n' | ${tools_dir}/codes_interpreter --non-fail "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
[ "$(printf '%s\n' "$result" | grep -c '^ED=2$')" = "1" ]
[ "$(printf '%s\n' "$result" | grep -c '^Q=24$')" = "1" ]

cat "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" "$ECCODES_SAMPLES_PATH/GRIB1.tmpl" > "$tempMulti"
result=$(printf 'print "ED=[edition]";\nquit\n' | ${tools_dir}/codes_interpreter --message 2 "$tempMulti" 2>&1)
printf '%s\n' "$result" | grep -q '^ED=1$'

result=$(printf ':next\nprint "ED=[edition]";\n:goto 1\nprint "ED=[edition]";\nquit\n' | ${tools_dir}/codes_interpreter --non-fail "$tempMulti" 2>&1)
[ "$(printf '%s\n' "$result" | grep -c '^ED=1$')" = "1" ]
[ "$(printf '%s\n' "$result" | grep -c '^ED=2$')" = "1" ]

result=$(printf 'print "C=[count]";\n:goto 2\nprint "C=[count]";\nquit\n' | ${tools_dir}/codes_interpreter --non-fail "$tempMulti" 2>&1)
[ "$(printf '%s\n' "$result" | grep -c '^C=1$')" = "1" ]
[ "$(printf '%s\n' "$result" | grep -c '^C=2$')" = "1" ]

rm -f "$tempMulti"
