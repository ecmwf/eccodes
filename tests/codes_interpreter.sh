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
tempLog=temp.$label.log
tempFilter=temp.$label.filter

result=$(${tools_dir}/codes_interpreter --help 2>&1)
printf '%s\n' "$result" | grep -q 'Usage:'
printf '%s\n' "$result" | grep -q -- '--non-fail'
printf '%s\n' "$result" | grep -q -- '--log-session FILE'

result=$(printf 'print "[edition]";\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl")
printf '%s\n' "$result" | grep -q '^2$'

result=$(printf 'if (edition == 2) { print "OK"; }\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl")
printf '%s\n' "$result" | grep -q '^OK$'

result=$(printf ':accessors\nquit\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Accessors ('

result=$(printf ':accessors count_\nquit\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Accessors matching /count_/'
printf '%s\n' "$result" | grep -q '^  count_file$'

result=$(printf ':accessors --ignore-case ^COUNT_\nquit\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Accessors matching /\^COUNT_/'
printf '%s\n' "$result" | grep -q '^  count_file$'

result=$(printf ':changes\nquit\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q 'not activated - use --log-key-changes'

result=$(printf ':list\nquit\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Keys ('

result=$(printf ':list ^editionNumber$\nquit\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Keys matching /\^editionNumber\$/'
printf '%s\n' "$result" | grep -q '^  editionNumber$'

result=$(printf ':list --values ^editionNumber$\nquit\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Keys with values matching /\^editionNumber\$/'
printf '%s\n' "$result" | grep -q '^  editionNumber = L:2$'

result=$(printf ':list --values -i ^EDITIONNUMBER$\nquit\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Keys with values matching /\^EDITIONNUMBER\$/'
printf '%s\n' "$result" | grep -q '^  editionNumber = L:2$'

result=$(printf ':show editionNumber\nquit\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Key: editionNumber$'
printf '%s\n' "$result" | grep -q '^  type: long$'
printf '%s\n' "$result" | grep -q '^  value: L:2$'

result=$(printf ':show --ignore-case ^EDITIONNUMBER$\nquit\n' | ${tools_dir}/codes_interpreter "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Show matching /\^EDITIONNUMBER\$/ (1):$'
printf '%s\n' "$result" | grep -q '^Key: editionNumber$'

result=$(printf 'transient toffset = 18;\n:changes\nquit\n' | ${tools_dir}/codes_interpreter --log-key-changes "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Changed keys ('
printf '%s\n' "$result" | grep -q '^  toffset: '

result=$(printf 'transient toffset = 18;\n:changes ^toff\nquit\n' | ${tools_dir}/codes_interpreter --log-key-changes "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Changed keys matching /\^toff/'
printf '%s\n' "$result" | grep -q '^  toffset: '

result=$(printf 'transient toffset = 18;\n:changes -i ^TOFF\nquit\n' | ${tools_dir}/codes_interpreter --log-key-changes "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Changed keys matching /\^TOFF/'
printf '%s\n' "$result" | grep -q '^  toffset: '

result=$(printf 'set forecastTime = 36;\n:changes --touched\nquit\n' | ${tools_dir}/codes_interpreter --log-key-changes "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Touched but unchanged keys ('

result=$(printf 'transient toffset = 18;\n:diff ^toffset$\nquit\n' | ${tools_dir}/codes_interpreter --log-key-changes "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Changed keys matching /\^toffset\$/'
printf '%s\n' "$result" | grep -q '^  toffset: '

result=$(printf 'transient toffset = 18;\n:diff --ignore-case ^TOFFSET$\nquit\n' | ${tools_dir}/codes_interpreter --log-key-changes "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Changed keys matching /\^TOFFSET\$/'
printf '%s\n' "$result" | grep -q '^  toffset: '

rm -f "$tempFilter"
result=$(printf 'transient toffset = 18;\n:save %s\n:undo\nprint "TOFF=[toffset]";\n:load %s\nprint "TOFF=[toffset]";\nquit\n' "$tempFilter" "$tempFilter" | ${tools_dir}/codes_interpreter --non-fail "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^Saved session to '
printf '%s\n' "$result" | grep -q '^Undid last statement$'
printf '%s\n' "$result" | grep -q '^Loaded session from '
[ "$(printf '%s\n' "$result" | grep -c '^TOFF=undef$')" = "1" ]
[ "$(printf '%s\n' "$result" | grep -c '^TOFF=18$')" = "1" ]

result=$(printf 'meta d1 validity_date(dataDate,dataTime,step,stepUnits);\n:changes\nquit\n' | ${tools_dir}/codes_interpreter --log-key-changes "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^  d1: '

result=$(printf ':accessors [\nquit\n' | ${tools_dir}/codes_interpreter --non-fail "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q 'invalid regex'

result=$(printf ':list [\nquit\n' | ${tools_dir}/codes_interpreter --non-fail "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q 'invalid regex'

result=$(printf 'transient toffset = 18;\n:changes [\nquit\n' | ${tools_dir}/codes_interpreter --non-fail --log-key-changes "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q 'invalid regex'

rm -f "$tempLog"
result=$(printf 'print "ED=[edition]";\nquit\n' | ${tools_dir}/codes_interpreter --log-session "$tempLog" "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
printf '%s\n' "$result" | grep -q '^ED=2$'
grep -q '^codes_interpreter> print "ED=\[edition\]";$' "$tempLog"

rm -f "$tempLog"
result=$(printf 'set forecastTime = 36;\nquit\n' | ${tools_dir}/codes_interpreter --log-session "$tempLog" --log-key-changes "$ECCODES_SAMPLES_PATH/GRIB2.tmpl" 2>&1)
grep -q '^codes_interpreter> set forecastTime = 36;$' "$tempLog"
grep -q '^Changed keys (' "$tempLog"
grep -q '^  forecastTime: ' "$tempLog"

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

rm -f "$tempMulti" "$tempLog" "$tempFilter"
