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

# Separator for several definitions roots, as used by ecCodes on this platform
if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    psep=';'
else
    psep=':'
fi

label="grib_parameters_test"
tempA=temp.$label.A.out
tempB=temp.$label.B.out
tempErr=temp.$label.err
tempGrib=temp.$label.grib2

# Help output should advertise the scope option
${tools_dir}/grib_parameters --help > $tempA
grep -q -- "--scope SCOPE" $tempA
grep -q -- "--nattrkey NATTRKEY" $tempA
grep -q -- "--nattr NATTR" $tempA
grep -q -- "--attrkey ATTRKEY" $tempA
grep -q -- "--order {asc,desc}" $tempA
grep -q -- "--is_legacy {0,1}" $tempA
grep -q -- "--order-by FIELD" $tempA

# Trailing slash in ECCODES_DEFINITION_PATH should be accepted
saved_def_path=$ECCODES_DEFINITION_PATH
export ECCODES_DEFINITION_PATH="${saved_def_path}/"
${tools_dir}/grib_parameters --paramId 167 > $tempA
[ -s $tempA ]
export ECCODES_DEFINITION_PATH=$saved_def_path

# Scope filter: single localConcept scope
${tools_dir}/grib_parameters --paramId 167 --scope grib2/localConcepts/tigge > $tempA
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
${tools_dir}/grib_parameters --paramId 167 --scope grib2,grib2/localConcepts/tigge > $tempB
[ -s $tempB ]
grep -q "scope=grib2 " $tempB
grep -q "scope=grib2/localConcepts/tigge" $tempB

# shortName filter
${tools_dir}/grib_parameters --shortName 2t > $tempA
[ -s $tempA ]
grep -q "shortName=2t" $tempA
if grep -q "shortName=MIXHGT-M" $tempA; then
    echo "Unexpected shortName in --shortName filtered output"
    exit 1
fi

# name filter (wildcard)
${tools_dir}/grib_parameters --name '*2 metre temperature*' > $tempA
[ -s $tempA ]
grep -q "name=2 metre temperature" $tempA
if grep -qi "mixed layer" $tempA; then
    echo "Unexpected name in --name filtered output"
    exit 1
fi

# units filter
${tools_dir}/grib_parameters --units K > $tempA
[ -s $tempA ]
grep -q "units=K" $tempA
if grep -q "units=m" $tempA; then
    echo "Unexpected units in --units filtered output"
    exit 1
fi

# attr filter
${tools_dir}/grib_parameters --edition 2 --attr discipline=0,parameterCategory=0 > $tempA
[ -s $tempA ]
grep -q "edition=2" $tempA
if grep -q "edition=1" $tempA; then
    echo "Unexpected edition in --attr filtered output"
    exit 1
fi

# Invalid attr key should fail with suggestion/error message
set +e
${tools_dir}/grib_parameters --attr disciplin=0 > $tempB 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q "seems to be misspelled" $tempErr

# is_mtg2_switch filter
${tools_dir}/grib_parameters --edition 2 --paramId 167 --is_mtg2_switch 1 > $tempA
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
${tools_dir}/grib_parameters --paramId 167 --show-sources > $tempA
[ -s $tempA ]
grep -q "sources:" $tempA
grep -q "paramId\.def" $tempA

# paramId regex filter
${tools_dir}/grib_parameters --paramId-regex '^16[56]$' > $tempA
[ -s $tempA ]
grep -q "paramId=165" $tempA
grep -q "paramId=166" $tempA
if grep -q "paramId=167" $tempA; then
    echo "Unexpected paramId in --paramId-regex filtered output"
    exit 1
fi

# Numeric ordering of paramId in table output
${tools_dir}/grib_parameters --paramId-regex '^[0-9]{1,3}$' --scope grib2 --columns paramId --is_mtg2_switch 1 --format table > $tempA
[ -s $tempA ]
awk 'NR>2 {gsub(/ /,""); if ($1!="") print $1}' $tempA | \
awk 'NR==1{p=$1;next} {if ($1+0 < p+0) exit 1; p=$1}'

# Descending numeric ordering with --order desc
${tools_dir}/grib_parameters --paramId-regex '^[0-9]{1,3}$' --scope grib2 --columns paramId --is_mtg2_switch 1 --format table --order desc > $tempA
[ -s $tempA ]
awk 'NR>2 {gsub(/ /,""); if ($1!="") print $1}' $tempA | \
awk 'NR==1{p=$1;next} {if ($1+0 > p+0) exit 1; p=$1}'

# Field-specific ordering: shortName descending
${tools_dir}/grib_parameters --paramId-regex '^[0-9]{1,3}$' --scope grib2 --columns shortName --is_mtg2_switch 1 --format table --order-by shortName --order desc > $tempA
[ -s $tempA ]
awk 'NR>2 {gsub(/ /,""); if ($1!="") print $1}' $tempA | \
awk 'NR==1{p=$1;next} {if ($1 > p) exit 1; p=$1}'

# Multi-field ordering: scope then paramId ascending
${tools_dir}/grib_parameters --paramId-regex '^[0-9]{1,3}$' --columns scope,paramId --format table --order-by scope,paramId --order asc > $tempA
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
' $tempA

# Invalid scope argument must fail
set +e
${tools_dir}/grib_parameters --paramId 167 --scope , > $tempA 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "--scope must contain at least one non-empty scope" $tempErr

# Unknown scope must fail with explicit message
set +e
${tools_dir}/grib_parameters --paramId 167 --scope xyz > $tempA 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "Scope 'xyz' is not defined" $tempErr
grep -q -- "Available scopes:" $tempErr

# attrkey filter: include only records that have a key (any value)
${tools_dir}/grib_parameters --edition 2 --paramId 167 --show-encoding --attrkey typeOfStatisticalProcessing > $tempA
[ -s $tempA ]
grep -q "typeOfStatisticalProcessing" $tempA
# All returned lines should have the encoding key present
if grep -v "typeOfStatisticalProcessing" $tempA | grep -q "encoding:"; then
    echo "Record without --attrkey key slipped through"
    exit 1
fi

# attrkey must fail when given a key containing '='
set +e
${tools_dir}/grib_parameters --attrkey typeOfStatisticalProcessing=0 > $tempA 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "Keys must not contain '='" $tempErr

# attr-strict + attrkey should allow strict match on known key/values plus key presence
${tools_dir}/grib_parameters --attr discipline=10,parameterCategory=0,parameterNumber=3 --is_mtg2_switch 1 --attr-strict --scope grib2 --attrkey typeOfStatisticalProcessing > $tempA
[ -s $tempA ]
grep -q "paramId=132216" $tempA
grep -q "paramId=141229" $tempA
grep -q "paramId=143229" $tempA
grep -q "paramId=144229" $tempA
grep -q "paramId=145229" $tempA

# attr-strict + attrkey only: should be accepted (may legitimately return no matches)
set +e
${tools_dir}/grib_parameters --attr-strict --attrkey typeOfStatisticalProcessing --scope grib2 > $tempA 2> $tempErr
status=$?
set -e
[ $status -ne 2 ]
if grep -q -- "--attr-strict requires --attr" $tempErr; then
    echo "Unexpected parser rejection for --attr-strict with --attrkey only"
    exit 1
fi

# attr-strict with neither --attr nor --attrkey must fail
set +e
${tools_dir}/grib_parameters --attr-strict > $tempA 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "--attr-strict requires --attr and/or --attrkey" $tempErr

# Invalid nattrkey containing '=' must fail
set +e
${tools_dir}/grib_parameters --nattrkey typeOfStatisticalProcessing=0 > $tempA 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "Keys must not contain '='" $tempErr

# nattrkey filter: exclude records containing a key
${tools_dir}/grib_parameters --edition 2 --paramId 167 --show-encoding --nattrkey typeOfStatisticalProcessing > $tempA
[ -s $tempA ]
if grep -q "typeOfStatisticalProcessing" $tempA; then
    echo "Unexpected excluded key in --nattrkey filtered output"
    exit 1
fi

# nattr filter: exclude records containing a specific key/value pair
${tools_dir}/grib_parameters --edition 2 --paramId 167 --show-encoding --nattr typeOfStatisticalProcessing=0 > $tempA
[ -s $tempA ]
if grep -q "typeOfStatisticalProcessing=0" $tempA; then
    echo "Unexpected excluded key/value in --nattr filtered output"
    exit 1
fi

# Single-line concept entries must be parsed, e.g. in grib1/localConcepts/kwbc/paramId.def:
#   '260056' = { table2Version=128; indicatorOfParameter =141; }
${tools_dir}/grib_parameters --scope grib1/localConcepts/kwbc --show-encoding > $tempA
[ -s $tempA ]
grep -q "scope=grib1/localConcepts/kwbc paramId=260056 shortName=sd " $tempA
grep -q "scope=grib1/localConcepts/kwbc paramId=3096 shortName=vice " $tempA
grep -q "encoding: indicatorOfParameter=141, table2Version=128" $tempA

# Legacy concepts (*.legacy.def): ecCodes reads them from grib2 and grib2/localConcepts/ecmf as the
# last fallback, for every MTG2 switch value. This encoding is only in ecmf/paramId.legacy.def
${tools_dir}/grib_parameters --attr discipline=192,parameterCategory=131,parameterNumber=60 > $tempA
[ -s $tempA ]
[ $(grep -c "is_legacy=1 scope=grib2/localConcepts/ecmf paramId=131060 shortName=tpg1 " $tempA) -eq 3 ]
grep -q "is_mtg2_switch=0 is_legacy=1" $tempA
grep -q "is_mtg2_switch=1 is_legacy=1" $tempA
grep -q "is_mtg2_switch=2 is_legacy=1" $tempA
# ... and agrees with the library
${tools_dir}/grib_set -s discipline=192,parameterCategory=131,parameterNumber=60 $ECCODES_SAMPLES_PATH/GRIB2.tmpl $tempGrib
[ "$(${tools_dir}/grib_get -p paramId $tempGrib)" = "131060" ]
rm -f $tempGrib

# --is_legacy filter
${tools_dir}/grib_parameters --paramId 131060 --is_legacy 0 > $tempA
[ -s $tempA ]
if grep -q "is_legacy=1" $tempA; then
    echo "Unexpected legacy entry with --is_legacy 0"
    exit 1
fi
${tools_dir}/grib_parameters --paramId 131060 --is_legacy 1 > $tempA
[ -s $tempA ]
if grep -v -q "is_legacy=1" $tempA; then
    echo "Unexpected non-legacy entry with --is_legacy 1"
    exit 1
fi
# Legacy files not read by ecCodes (e.g. grib2/localConcepts/era) must not be read either
${tools_dir}/grib_parameters --is_legacy 1 --show-sources > $tempA
if grep -q "localConcepts/era/" $tempA; then
    echo "Unexpected legacy file outside grib2 and grib2/localConcepts/ecmf"
    exit 1
fi
set +e
${tools_dir}/grib_parameters --is_legacy 2 > $tempB 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "--is_legacy must be one of 0,1" $tempErr

# Symbolic links to concept files and directories are followed, as ecCodes opens files by
# path; link cycles are skipped
if [ $ECCODES_ON_WINDOWS -eq 0 ]; then
    linkDir=temp.$label.links
    rm -rf $linkDir
    mkdir -p $linkDir/real/centre $linkDir/defs/grib2/localConcepts
    sed "s/^'2 metre temperature' = {/'2 metre temperature LINKED' = {/" $def_dir/grib2/name.lte33.def > $linkDir/real/name.lte33.def
    ln -s ../../real/name.lte33.def $linkDir/defs/grib2/name.lte33.def
    cp $def_dir/grib2/paramId.def $linkDir/real/centre/paramId.def
    ln -s ../../../real/centre $linkDir/defs/grib2/localConcepts/zzzz
    ln -s .. $linkDir/defs/grib2/localConcepts/loop
    # The linked override wins over the file in the second root
    ${tools_dir}/grib_parameters --definitions-root "$linkDir/defs${psep}$def_dir" --paramId 167 --scope grib2 --is_mtg2_switch 0 > $tempA
    grep -q "name=2 metre temperature LINKED" $tempA
    # The linked directory is scanned
    ${tools_dir}/grib_parameters --definitions-root "$linkDir/defs${psep}$def_dir" --paramId 167 --scope grib2/localConcepts/zzzz > $tempA
    [ -s $tempA ]
    rm -rf $linkDir
fi

# Concept files which cannot be parsed are errors (no partial results), as in ecCodes;
# files without entries are valid
badDir=temp.$label.bad
rm -rf $badDir
mkdir -p $badDir/malformed/grib2 $badDir/empty/grib2 $badDir/first/grib2 $badDir/other/grib2
printf "'x' = {\n discipline = \n" > $badDir/malformed/grib2/name.lte33.def
set +e
${tools_dir}/grib_parameters --definitions-root "$badDir/malformed${psep}$def_dir" --paramId 167 --scope grib2 --is_mtg2_switch 0 > $tempA 2> $tempErr
status=$?
set -e
[ $status -eq 2 ]
grep -q "Cannot parse concept file .*malformed/grib2/name.lte33.def" $tempErr
if grep -q "edition=" $tempA; then
    echo "Unexpected partial result with a malformed concept file"
    exit 1
fi
# Only white space and comments: valid, no entries
printf "# no entries\n\n" > $badDir/empty/grib2/name.lte33.def
${tools_dir}/grib_parameters --definitions-root "$badDir/empty${psep}$def_dir" --paramId 167 --scope grib2 --is_mtg2_switch 0 > $tempA
grep -q "name=- " $tempA
# Other definitions than concepts, read after a valid concept file: an error, not a stale result
cp $def_dir/grib2/units.lte33.def $badDir/first/grib2/units.lte33.def
printf "transient dummy = 1;\n" > $badDir/other/grib2/name.lte33.def
set +e
${tools_dir}/grib_parameters --definitions-root "$badDir/first${psep}$badDir/other${psep}$def_dir" --paramId 167 --scope grib2 --is_mtg2_switch 0 > $tempA 2> $tempErr
status=$?
set -e
[ $status -eq 2 ]
grep -q "Cannot parse concept file .*other/grib2/name.lte33.def" $tempErr
rm -rf $badDir

# Array values in --attr/--nattr: commas inside brackets do not separate pairs
${tools_dir}/grib_parameters --edition 2 --attr 'typeOfStatisticalProcessing=[102,2]' > $tempA
grep -q "paramId=132044 " $tempA
${tools_dir}/grib_parameters --edition 2 --paramId 132044 --attr 'discipline=0, typeOfStatisticalProcessing=[102, 2]' > $tempB
grep -q "paramId=132044 " $tempB
set +e
${tools_dir}/grib_parameters --edition 2 --paramId 132044 --nattr 'typeOfStatisticalProcessing=[102,2]' > $tempB
status=$?
set -e
[ $status -eq 1 ]
set +e
${tools_dir}/grib_parameters --attr 'typeOfStatisticalProcessing=[102,2' > $tempB 2> $tempErr
status=$?
set -e
[ $status -ne 0 ]
grep -q -- "Unbalanced brackets in --attr" $tempErr

# No usable definitions root: an error, no guessed tree and no results
set +e
${tools_dir}/grib_parameters --paramId 167 --definitions-root temp.$label.nonexistent > $tempA 2> $tempErr
status=$?
set -e
[ $status -eq 2 ]
grep -q "No valid definitions roots were found" $tempErr
if grep -q "edition=" $tempA; then
    echo "Unexpected results without a usable definitions root"
    exit 1
fi

# Malformed values of --edition and --is_mtg2_switch are usage errors, not a real mode
for bad in "--edition 2junk" "--edition 3" "--is_mtg2_switch garbage" "--is_mtg2_switch 1x"; do
    set +e
    ${tools_dir}/grib_parameters --paramId 167 $bad > $tempA 2> $tempErr
    status=$?
    set -e
    [ $status -eq 2 ]
    grep -q "must be" $tempErr
done

# --definitions-root uses only the given directories: the built-in definitions are not
# added and ECCODES_EXTRA_DEFINITION_PATH is ignored (e.g. to query an older definitions set)
onlyDir=temp.$label.only
rm -rf $onlyDir
mkdir -p $onlyDir/grib2
cp $def_dir/grib2/paramId.def $onlyDir/grib2/paramId.def
${tools_dir}/grib_parameters --definitions-root $onlyDir --paramId 167 --scope grib2 --is_mtg2_switch 1 > $tempA
grep -q "paramId=167 shortName=- name=- units=-" $tempA
[ $(grep -c "^edition=" $tempA) -eq 1 ]
ECCODES_EXTRA_DEFINITION_PATH=$def_dir ${tools_dir}/grib_parameters --definitions-root $onlyDir --paramId 167 --scope grib2 --is_mtg2_switch 1 > $tempB
cmp $tempA $tempB
rm -rf $onlyDir

# A definitions directory which exists but cannot be listed is an error: ecCodes could still
# open overrides in it by name. A missing grib1/ or grib2/ is fine.
if [ $ECCODES_ON_WINDOWS -eq 0 ] && [ "$(id -u)" != "0" ]; then
    permDir=temp.$label.perm
    rm -rf $permDir
    mkdir -p $permDir/grib2
    cp $def_dir/grib2/name.lte33.def $permDir/grib2/name.lte33.def
    chmod 311 $permDir/grib2
    set +e
    ${tools_dir}/grib_parameters --definitions-root "$permDir${psep}$def_dir" --paramId 167 > $tempA 2> $tempErr
    status=$?
    set -e
    chmod 755 $permDir/grib2
    [ $status -eq 2 ]
    grep -q "Cannot list definitions directory .*perm/grib2" $tempErr
    if grep -q "edition=" $tempA; then
        echo "Unexpected results with an unlistable definitions directory"
        exit 1
    fi
    rm -rf $permDir
fi

# '*' in a wildcard pattern is a wildcard also where the value has a literal '*'
${tools_dir}/grib_parameters --units 'm*' > $tempA
grep -q "units=m\*\*2 s\*\*-1$" $tempA

# A root written with trailing slashes is the same root: same relative paths, scopes and
# precedence as without them
${tools_dir}/grib_parameters --definitions-root "$def_dir" --paramId 167 --scope grib2 --is_mtg2_switch 0 --show-sources > $tempA
${tools_dir}/grib_parameters --definitions-root "$def_dir//" --paramId 167 --scope grib2 --is_mtg2_switch 0 --show-sources > $tempB
cmp $tempA $tempB
slashDir=temp.$label.slash
rm -rf $slashDir
mkdir -p $slashDir/grib2
sed "s/^'2 metre temperature' = {/'2 metre temperature SLASH' = {/" $def_dir/grib2/name.lte33.def > $slashDir/grib2/name.lte33.def
${tools_dir}/grib_parameters --definitions-root "$slashDir//${psep}$def_dir" --paramId 167 --scope grib2 --is_mtg2_switch 0 > $tempA
grep -q "name=2 metre temperature SLASH" $tempA
rm -rf $slashDir

# MEMFS cannot be listed, so the tool has a fixed list of the concept files it looks
# for in MEMFS. Every concept file in the definitions must be on that list. Checked in
# all builds, as a file missing from the list would only show up in a MEMFS build
${tools_dir}/grib_parameters --debug-path-resolution > $tempA 2> $tempErr
grep -Eq "effective backend: (filesystem|mixed)" $tempErr
if grep "not in MEMFS candidate list" $tempErr; then
    echo "Add the localConcepts directories above to memfs_candidate_files() in tools/grib_parameters.cc"
    exit 1
fi
# ... and the check must detect a new centre
tempDir=temp.$label.dir
rm -rf $tempDir
mkdir -p $tempDir/grib2/localConcepts/zzzz
cp $def_dir/grib2/paramId.def $tempDir/grib2/localConcepts/zzzz/paramId.def
${tools_dir}/grib_parameters --definitions-root $tempDir --debug-path-resolution > $tempA 2> $tempErr
grep -q "not in MEMFS candidate list: grib2/localConcepts/zzzz/paramId.def" $tempErr
rm -rf $tempDir

# MEMFS: definitions embedded in the library must give the same records as
# the definitions on the filesystem
if [ $HAVE_MEMFS -eq 1 ]; then
    ${tools_dir}/grib_parameters --show-encoding > $tempA
    saved_def_path=$ECCODES_DEFINITION_PATH
    unset ECCODES_DEFINITION_PATH
    ${tools_dir}/grib_parameters --paramId 167 --debug-path-resolution > $tempB 2> $tempErr
    grep -q "effective backend: memfs" $tempErr
    ${tools_dir}/grib_parameters --show-encoding > $tempB
    export ECCODES_DEFINITION_PATH=$saved_def_path
    [ -s $tempB ]
    cmp $tempA $tempB
    grep -q "scope=grib1/localConcepts/kwbc paramId=260056 shortName=sd " $tempB
    # Distinct MEMFS roots are kept: a wrong first root must not hide the second
    ${tools_dir}/grib_parameters --paramId 167 --definitions-root "/MEMFS/wrong${psep}/MEMFS/definitions" > $tempB
    [ -s $tempB ]
    # Trailing slash in a MEMFS ECCODES_DEFINITION_PATH should be accepted
    export ECCODES_DEFINITION_PATH=/MEMFS/definitions/
    ${tools_dir}/grib_parameters --paramId 167 --debug-path-resolution > $tempB 2> $tempErr
    [ -s $tempB ]
    grep -q "effective backend: memfs" $tempErr
    export ECCODES_DEFINITION_PATH=$saved_def_path
fi

# JSON output
${tools_dir}/grib_parameters --help > $tempA
grep -q -- "--format {line,table,json}" $tempA
${tools_dir}/grib_parameters --paramId 167 --scope grib2 --is_mtg2_switch 1 --format json > $tempA
cat > $tempB <<EOF
[
  {"edition": 2, "is_mtg2_switch": 1, "is_legacy": 0, "scope": "grib2", "paramId": 167, "shortName": "2t", "name": "2 metre temperature", "units": "K"}
]
EOF
diff $tempB $tempA
# Edition 1 has no MTG2 switch and no legacy files
${tools_dir}/grib_parameters --paramId 167 --scope grib1 --columns edition,is_mtg2_switch,is_legacy --format json > $tempA
grep -q '{"edition": 1, "is_mtg2_switch": null, "is_legacy": null}' $tempA
# One object per result, as in the line format
n_line=$(${tools_dir}/grib_parameters --paramId 167 --format line | wc -l)
n_json=$(${tools_dir}/grib_parameters --paramId 167 --format json | grep -c '^  {')
[ $n_line -eq $n_json ]
# Merged duplicates are arrays
${tools_dir}/grib_parameters --paramId 500417 --scope grib2/localConcepts/edzw --is_mtg2_switch 1 --columns paramId,shortName --format json > $tempA
grep -q '{"paramId": \[500417, 500582\], "shortName": null}' $tempA
# Encoding is an object: numbers stay numbers, integer arrays become arrays
${tools_dir}/grib_parameters --paramId 132044 --edition 2 --is_mtg2_switch 1 --columns encoding --format json > $tempA
grep -q '"typeOfStatisticalProcessing": \[102, 2\]' $tempA
grep -q '"scaleFactorOfFirstFixedSurface": "missing()"' $tempA
grep -q '"parameterCategory": 7,' $tempA
# Quotes in names are escaped, sources are an array
${tools_dir}/grib_parameters --name '*"first guess - analysis"*' --columns name,sources --format json > $tempA
grep -q '"name": "RMS of difference \\"first guess - analysis\\" of kinetic energy"' $tempA
grep -q '"sources": \["grib2/localConcepts/edzw/[^]]*"grib2/localConcepts/edzw/name.def"' $tempA
# No results: stdout stays valid JSON, message on stderr, exit status 1
set +e
${tools_dir}/grib_parameters --paramId 123456789 --format json > $tempA 2> $tempErr
status=$?
set -e
[ $status -eq 1 ]
[ "$(cat $tempA)" = "[]" ]
grep -q "No matching parameters found." $tempErr

# Only the concept files which can match --edition, --is_mtg2_switch, --is_legacy and --scope are parsed
${tools_dir}/grib_parameters --paramId 167 --edition 2 --scope grib2 --is_mtg2_switch 1 --debug-path-resolution > $tempA 2> $tempErr
grep -q "paramId=167 shortName=2t " $tempA
grep -q "DEBUG: concept files parsed: [1-9][0-9]*, skipped by filters: [1-9][0-9]*$" $tempErr
if grep -q "reading all concept files" $tempErr; then
    echo "Unexpected full read for a query with results"
    exit 1
fi
${tools_dir}/grib_parameters --paramId 167 --debug-path-resolution > $tempA 2> $tempErr
grep -q "skipped by filters: 0$" $tempErr
# The checks which need all definitions read them all: same messages as without the filters
set +e
${tools_dir}/grib_parameters --scope grib2/localConcepts/ecmff --edition 2 --debug-path-resolution > $tempA 2> $tempErr
status=$?
set -e
[ $status -eq 2 ]
grep -q "reading all concept files" $tempErr
grep -q "Scope 'grib2/localConcepts/ecmff' is not defined. Did you mean 'grib2/localConcepts/ecmf'?" $tempErr
# A GRIB2 key with --edition 1: not misspelled, just no results
set +e
${tools_dir}/grib_parameters --edition 1 --attr discipline=0 > $tempA 2> $tempErr
status=$?
set -e
[ $status -eq 1 ]
grep -q "No matching parameters found." $tempA
# No files at all match: no results, not a missing definitions root
set +e
${tools_dir}/grib_parameters --edition 1 --is_mtg2_switch 1 > $tempA 2> $tempErr
status=$?
set -e
[ $status -eq 1 ]
grep -q "No matching parameters found." $tempA

# Clean up
rm -f $tempA $tempB $tempErr $tempGrib
