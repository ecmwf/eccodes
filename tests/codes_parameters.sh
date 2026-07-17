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

label="codes_parameters_test"
tempA=temp.$label.A.out
tempB=temp.$label.B.out
tempErr=temp.$label.err

# Help output should advertise the scope option
${tools_dir}/codes_parameters --help > $tempA
grep -q -- "--scope SCOPE" $tempA
grep -q -- "--nattrkey NATTRKEY" $tempA
grep -q -- "--nattr NATTR" $tempA
grep -q -- "--attrkey ATTRKEY" $tempA
grep -q -- "--order {asc,desc}" $tempA
grep -q -- "--order-by FIELD" $tempA

# Trailing slash in ECCODES_DEFINITION_PATH should be accepted
saved_def_path=$ECCODES_DEFINITION_PATH
export ECCODES_DEFINITION_PATH="${saved_def_path}/"
${tools_dir}/codes_parameters --paramId 167 > $tempA
[ -s $tempA ]
export ECCODES_DEFINITION_PATH=$saved_def_path

# Scope filter: single localConcept scope
${tools_dir}/codes_parameters --paramId 167 --scope grib2/localConcepts/tigge > $tempA
[ -s $tempA ]
grep -q "scope=grib2/localConcepts/tigge" $tempA
if grep -q "scope=grib1" $tempA; then
    echo "Unexpected grib1 scope in filtered output"
    exit 1
fi
if grep -q "scope=grib2 " $tempA; then
    echo "Unexpected base grib2 scope in localConcept-only filtered output"
    exit 1
fi

# Scope filter: multiple explicit scopes
${tools_dir}/codes_parameters --paramId 167 --scope grib2,grib2/localConcepts/tigge > $tempB
[ -s $tempB ]
grep -q "scope=grib2 " $tempB
grep -q "scope=grib2/localConcepts/tigge" $tempB

# shortName filter
${tools_dir}/codes_parameters --shortName 2t > $tempA
[ -s $tempA ]
grep -q "shortName=2t" $tempA
if grep -q "shortName=MIXHGT-M" $tempA; then
    echo "Unexpected shortName in --shortName filtered output"
    exit 1
fi

# name filter (wildcard)
${tools_dir}/codes_parameters --name '*2 metre temperature*' > $tempA
[ -s $tempA ]
grep -q "name=2 metre temperature" $tempA
if grep -qi "mixed layer" $tempA; then
    echo "Unexpected name in --name filtered output"
    exit 1
fi

# units filter
${tools_dir}/codes_parameters --units K > $tempA
[ -s $tempA ]
grep -q "units=K" $tempA
if grep -q "units=m" $tempA; then
    echo "Unexpected units in --units filtered output"
    exit 1
fi

# attr filter
${tools_dir}/codes_parameters --edition 2 --attr discipline=0,parameterCategory=0 > $tempA
[ -s $tempA ]
grep -q "edition=2" $tempA
if grep -q "edition=1" $tempA; then
    echo "Unexpected edition in --attr filtered output"
    exit 1
fi

# Invalid attr key should fail with suggestion/error message
set +e
${tools_dir}/codes_parameters --attr disciplin=0 > $tempB 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q "seems to be misspelled" $tempErr

# is_mtg2_switch filter
${tools_dir}/codes_parameters --edition 2 --paramId 167 --is_mtg2_switch 1 > $tempA
[ -s $tempA ]
grep -q "is_mtg2_switch=1" $tempA
if grep -q "is_mtg2_switch=0" $tempA; then
    echo "Unexpected is_mtg2_switch value in filtered output"
    exit 1
fi
if grep -q "is_mtg2_switch=2" $tempA; then
    echo "Unexpected is_mtg2_switch value in filtered output"
    exit 1
fi

# show-sources output
${tools_dir}/codes_parameters --paramId 167 --show-sources > $tempA
[ -s $tempA ]
grep -q "sources:" $tempA
grep -q "paramId\.def" $tempA

# paramId regex filter
${tools_dir}/codes_parameters --paramId-regex '^16[56]$' > $tempA
[ -s $tempA ]
grep -q "paramId=165" $tempA
grep -q "paramId=166" $tempA
if grep -q "paramId=167" $tempA; then
    echo "Unexpected paramId in --paramId-regex filtered output"
    exit 1
fi

# Numeric ordering of paramId in table output
${tools_dir}/codes_parameters --paramId-regex '^[0-9]{1,3}$' --scope grib2 --columns paramId --is_mtg2_switch 1 --format table > $tempA
[ -s $tempA ]
awk 'NR>2 {gsub(/ /,""); if ($1!="") print $1}' $tempA | \
awk 'NR==1{p=$1;next} {if ($1+0 < p+0) exit 1; p=$1} END{exit 0}'

# Descending numeric ordering with --order desc
${tools_dir}/codes_parameters --paramId-regex '^[0-9]{1,3}$' --scope grib2 --columns paramId --is_mtg2_switch 1 --format table --order desc > $tempA
[ -s $tempA ]
awk 'NR>2 {gsub(/ /,""); if ($1!="") print $1}' $tempA | \
awk 'NR==1{p=$1;next} {if ($1+0 > p+0) exit 1; p=$1} END{exit 0}'

# Field-specific ordering: shortName descending
${tools_dir}/codes_parameters --paramId-regex '^[0-9]{1,3}$' --scope grib2 --columns shortName --is_mtg2_switch 1 --format table --order-by shortName --order desc > $tempA
[ -s $tempA ]
awk 'NR>2 {gsub(/ /,""); if ($1!="") print $1}' $tempA | \
awk 'NR==1{p=$1;next} {if ($1 > p) exit 1; p=$1} END{exit 0}'

# Multi-field ordering: scope then paramId ascending
${tools_dir}/codes_parameters --paramId-regex '^[0-9]{1,3}$' --columns scope,paramId --format table --order-by scope,paramId --order asc > $tempA
[ -s $tempA ]
awk -F'\|' '
function trim(s){gsub(/^ +| +$/, "", s); return s}
NR>2 {
    scope=trim($1); id=trim($2);
    if (scope=="" || id=="") next;
    if (seen) {
        if (scope < pscope) exit 1;
        if (scope == pscope && id+0 < pid+0) exit 1;
    }
    pscope=scope; pid=id; seen=1;
}
END {exit 0}
' $tempA

# Invalid scope argument must fail
set +e
${tools_dir}/codes_parameters --paramId 167 --scope , > $tempA 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "--scope must contain at least one non-empty scope" $tempErr

# Unknown scope must fail with explicit message
set +e
${tools_dir}/codes_parameters --paramId 167 --scope xyz > $tempA 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "Scope 'xyz' is not defined" $tempErr
grep -q -- "Available scopes:" $tempErr

# attrkey filter: include only records that have a key (any value)
${tools_dir}/codes_parameters --edition 2 --paramId 167 --show-encoding --attrkey typeOfStatisticalProcessing > $tempA
[ -s $tempA ]
grep -q "typeOfStatisticalProcessing" $tempA
# All returned lines should have the encoding key present
if grep -v "typeOfStatisticalProcessing" $tempA | grep -q "encoding:"; then
    echo "Record without --attrkey key slipped through"
    exit 1
fi

# attrkey must fail when given a key containing '='
set +e
${tools_dir}/codes_parameters --attrkey typeOfStatisticalProcessing=0 > $tempA 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "Keys must not contain '='" $tempErr

# attr-strict + attrkey should allow strict match on known key/values plus key presence
${tools_dir}/codes_parameters --attr discipline=10,parameterCategory=0,parameterNumber=3 --is_mtg2_switch 1 --attr-strict --scope grib2 --attrkey typeOfStatisticalProcessing > $tempA
[ -s $tempA ]
grep -q "paramId=132216" $tempA
grep -q "paramId=141229" $tempA
grep -q "paramId=143229" $tempA
grep -q "paramId=144229" $tempA
grep -q "paramId=145229" $tempA

# attr-strict + attrkey only: should be accepted (may legitimately return no matches)
set +e
${tools_dir}/codes_parameters --attr-strict --attrkey typeOfStatisticalProcessing --scope grib2 > $tempA 2> $tempErr
status=$?
set -e
[ $status -ne 2 ]
if grep -q -- "--attr-strict requires --attr" $tempErr; then
    echo "Unexpected parser rejection for --attr-strict with --attrkey only"
    exit 1
fi

# attr-strict with neither --attr nor --attrkey must fail
set +e
${tools_dir}/codes_parameters --attr-strict > $tempA 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "--attr-strict requires --attr and/or --attrkey" $tempErr

# Invalid nattrkey containing '=' must fail
set +e
${tools_dir}/codes_parameters --nattrkey typeOfStatisticalProcessing=0 > $tempA 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "Keys must not contain '='" $tempErr

# nattrkey filter: exclude records containing a key
${tools_dir}/codes_parameters --edition 2 --paramId 167 --show-encoding --nattrkey typeOfStatisticalProcessing > $tempA
[ -s $tempA ]
if grep -q "typeOfStatisticalProcessing" $tempA; then
    echo "Unexpected excluded key in --nattrkey filtered output"
    exit 1
fi

# nattr filter: exclude records containing a specific key/value pair
${tools_dir}/codes_parameters --edition 2 --paramId 167 --show-encoding --nattr typeOfStatisticalProcessing=0 > $tempA
[ -s $tempA ]
if grep -q "typeOfStatisticalProcessing=0" $tempA; then
    echo "Unexpected excluded key/value in --nattr filtered output"
    exit 1
fi

# Clean up
rm -f $tempA $tempB $tempErr
