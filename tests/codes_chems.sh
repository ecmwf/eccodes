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

# Invalid chemFormula regex must fail
set +e
${tools_dir}/codes_chems --chemFormula-regex '[' > $tempB 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid pattern for" $tempErr

# nattrkey filter: exclude records containing a key
${tools_dir}/codes_chems --chemFormula O3 --show-encoding --nattrkey constituentType > $tempA
if [ -s $tempA ]; then
    echo "Expected no output for --nattrkey constituentType"
    exit 1
fi

# nattr filter: exclude records containing a specific key/value pair
${tools_dir}/codes_chems --chemFormula O3 --show-encoding --nattr constituentType=0 > $tempA
if [ -s $tempA ]; then
    echo "Expected no output for --nattr constituentType=0"
    exit 1
fi

# Clean up
rm -f $tempA $tempB $tempErr
