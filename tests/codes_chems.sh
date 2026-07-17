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

label="codes_chems_test"
tempA=temp.$label.A.out
tempB=temp.$label.B.out
tempErr=temp.$label.err

# Help output should advertise chemFormula options
${tools_dir}/codes_chems --help > $tempA
grep -q -- "--chemFormula CHEMFORMULA" $tempA
grep -q -- "--chemFormula-regex CHEMFORMULA_REGEX" $tempA
grep -q -- "--nattrkey NATTRKEY" $tempA
grep -q -- "--nattr NATTR" $tempA
grep -q -- "--attrkey ATTRKEY" $tempA
grep -q -- "--order {asc,desc}" $tempA
grep -q -- "--order-by FIELD" $tempA

# Trailing slash in ECCODES_DEFINITION_PATH should be accepted
saved_def_path=$ECCODES_DEFINITION_PATH
export ECCODES_DEFINITION_PATH="${saved_def_path}/"
${tools_dir}/codes_chems --chemFormula O3 > $tempA
[ -s $tempA ]
export ECCODES_DEFINITION_PATH=$saved_def_path

# chemFormula filter
${tools_dir}/codes_chems --chemFormula O3 > $tempA
[ -s $tempA ]
grep -q "chemFormula=O3" $tempA
grep -q "chemShortName=O3" $tempA

# Selected columns in table output should include chemFormula
${tools_dir}/codes_chems --columns edition,scope,chemId,chemShortName,chemName,chemFormula --chemFormula O3 --format table > $tempA
grep -q "chemFormula" $tempA
grep -q "O3" $tempA

# show-sources should print concept source files
${tools_dir}/codes_chems --chemFormula O3 --show-sources > $tempA
[ -s $tempA ]
grep -q "sources:" $tempA
grep -q "chemFormula.def" $tempA

# chemId regex filter
${tools_dir}/codes_chems --chemId-regex '^16[56]$' > $tempA
[ -s $tempA ]
grep -q "chemId=166" $tempA
if grep -E -v "chemId=16[56]( |$)" $tempA >/dev/null; then
    echo "Unexpected chemId in --chemId-regex filtered output"
    exit 1
fi

# Descending numeric ordering with --order desc
${tools_dir}/codes_chems --chemId-regex '^[0-9]{1,3}$' --scope grib2 --columns chemId --format table --order desc > $tempA
[ -s $tempA ]
awk 'NR>2 {gsub(/ /,""); if ($1!="") print $1}' $tempA | \
awk 'NR==1{p=$1;next} {if ($1+0 > p+0) exit 1; p=$1} END{exit 0}'

# Field-specific ordering: chemShortName descending
${tools_dir}/codes_chems --chemId-regex '^[0-9]{1,3}$' --scope grib2 --columns chemShortName --format table --order-by chemShortName --order desc > $tempA
[ -s $tempA ]
awk 'NR>2 {gsub(/ /,""); if ($1!="") print $1}' $tempA | \
awk 'NR==1{p=$1;next} {if ($1 > p) exit 1; p=$1} END{exit 0}'

# Multi-field ordering: scope then chemId ascending
${tools_dir}/codes_chems --chemId-regex '^[0-9]{1,3}$' --columns scope,chemId --format table --order-by scope,chemId --order asc > $tempA
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

# Invalid chemFormula regex must fail
set +e
${tools_dir}/codes_chems --chemFormula-regex '[' > $tempB 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid pattern for" $tempErr

# Unknown scope must fail with explicit message
set +e
${tools_dir}/codes_chems --scope xyz > $tempB 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "Scope 'xyz' is not defined" $tempErr
grep -q -- "Available scopes:" $tempErr

# attrkey filter: include only records that have a key (any value)
${tools_dir}/codes_chems --show-encoding --attrkey constituentType > $tempA
[ -s $tempA ]
grep -q "constituentType" $tempA

# attr-strict + attrkey only: should be accepted (may legitimately return no matches)
set +e
${tools_dir}/codes_chems --attr-strict --attrkey constituentType > $tempA 2> $tempErr
status=$?
set -e
[ $status -ne 2 ]
if grep -q -- "--attr-strict requires --attr" $tempErr; then
    echo "Unexpected parser rejection for --attr-strict with --attrkey only"
    exit 1
fi

# attr-strict with neither --attr nor --attrkey must fail
set +e
${tools_dir}/codes_chems --attr-strict > $tempA 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "--attr-strict requires --attr and/or --attrkey" $tempErr

# attrkey must fail when given a key containing '='
set +e
${tools_dir}/codes_chems --attrkey constituentType=0 > $tempA 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "Keys must not contain '='" $tempErr

# Invalid nattrkey containing '=' must fail
set +e
${tools_dir}/codes_chems --nattrkey constituentType=0 > $tempB 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "Keys must not contain '='" $tempErr

# nattrkey filter: exclude records containing a selective key
${tools_dir}/codes_chems --show-encoding --nattrkey typeOfSizeInterval > $tempA
[ -s $tempA ]
if grep -q "typeOfSizeInterval" $tempA; then
    echo "Unexpected excluded key in --nattrkey filtered output"
    exit 1
fi

# nattr filter: exclude records containing a specific key/value pair
set +e
${tools_dir}/codes_chems --chemFormula O3 --show-encoding --nattr constituentType=0 > $tempA 2> $tempErr
status=$?
set -e
[ $status -eq 1 ]
grep -q "No matching parameters found." $tempA

# Clean up
rm -f $tempA $tempB $tempErr
