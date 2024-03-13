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

label="grib_indexing_test"
temp=temp.$label.index.out
infile=${data_dir}/index.grib
sample1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl

# Writes to "out.gribidx"
$EXEC ${test_dir}/grib_indexing ${infile} > $temp
diff ${data_dir}/index.ok $temp

# reads from "out.gribidx"
$EXEC ${test_dir}/grib_read_index ${infile} > $temp
diff ${data_dir}/index.ok $temp

rm -f out.gribidx $temp

#-------------------------------
# Test grib_index_build
#-------------------------------
tempIndex=temp.$label.$$.idx
tempOut=temp.$label.$$.out
tempRef=temp.$label.$$.ref
${tools_dir}/grib_index_build -N -o $tempIndex ${infile} >/dev/null

# Must remove first two lines (filename specifics)
${tools_dir}/grib_dump ${tempIndex} | sed '1,2d' > $tempOut

cat > $tempRef <<EOF
Index keys:
key name = mars.date
values = 20090129
key name = mars.time
values = 0
key name = mars.expver
values = 0001
key name = mars.stream
values = enfo
key name = mars.class
values = od
key name = mars.type
values = pf
key name = mars.step
values = 12, 24, 48, 60
key name = mars.param
values = 130.128, 129.128
key name = mars.levtype
values = pl
key name = mars.levelist
values = 10, 50, 100, 200, 250, 300, 400, 500, 700, 850, 925, 1000
key name = mars.number
values = 1, 2, 3, 4
key name = mars.iteration
values = undef
key name = mars.domain
values = g
key name = mars.fcmonth
values = undef
key name = mars.fcperiod
values = undef
key name = mars.hdate
values = undef
key name = mars.method
values = undef
key name = mars.model
values = undef
key name = mars.origin
values = undef
key name = mars.quantile
values = undef
key name = mars.range
values = undef
key name = mars.refdate
values = undef
key name = mars.direction
values = undef
key name = mars.frequency
values = undef
Index count = 384
EOF

diff $tempRef $tempOut

# ECC-1748
${tools_dir}/grib_dump -t ${tempIndex} > $tempOut
grep -q "key type = string" $tempOut
grep -q "key type = long" $tempOut


${tools_dir}/grib_index_build -N -k mars.levtype -o $tempIndex ${data_dir}/tigge_cf_ecmwf.grib2 |\
   grep -q "mars.levtype = { sfc, pl, pv, pt }"


echo "grib_compare with index files..."
# ------------------------------------
tempIndex1=temp.$label.$$.1.idx
tempIndex2=temp.$label.$$.2.idx
tempGribFile1=temp.$label.$$.file1.grib
tempGribFile2=temp.$label.$$.file2.grib
cat ${data_dir}/high_level_api.grib2 ${data_dir}/sample.grib2          > $tempGribFile1
cat ${data_dir}/sample.grib2         ${data_dir}/high_level_api.grib2  > $tempGribFile2

${tools_dir}/grib_index_build -N -o $tempIndex1 $tempGribFile1
${tools_dir}/grib_index_build -N -o $tempIndex2 $tempGribFile2
${tools_dir}/grib_compare -v $tempIndex1 $tempIndex2
rm -f $tempIndex1 $tempIndex2 $tempGribFile1 $tempGribFile2

# Indexes containing different keys
${tools_dir}/grib_index_build -k date,time -N -o $tempIndex1 $sample1
${tools_dir}/grib_index_build -k level     -N -o $tempIndex2 $sample1
set +e
${tools_dir}/grib_compare -v $tempIndex1 $tempIndex2 2>$tempOut
status=$?
set -e
[ $status -ne 0 ]
grep -q "Indexes contained in the input files have different keys" $tempOut
rm -f $tempIndex1 $tempIndex2 $tempOut


${tools_dir}/grib_index_build -k date,time       -N -o $tempIndex1 $sample1
${tools_dir}/grib_index_build -k date,time,level -N -o $tempIndex2 $sample1
set +e
${tools_dir}/grib_compare -v $tempIndex2 $tempIndex1 2>$tempOut
status=$?
set -e
[ $status -ne 0 ]
grep -q "Indexes contained in the input files have different keys" $tempOut
rm -f $tempIndex1 $tempIndex2 $tempOut

# Indexing with directory traversal
# ----------------------------------
temp_dir_A=tempdir.$label
temp_dir_B=tempdir.$label/subdir.$label
rm -rf $temp_dir_A
mkdir $temp_dir_A
mkdir $temp_dir_B
cp ${data_dir}/tigge/tigge_rjtd_pl_*grib  $temp_dir_A
cp ${data_dir}/tigge/tigge_rjtd_sfc_*grib $temp_dir_B

${tools_dir}/grib_index_build -o $tempIndex1 $temp_dir_A > /dev/null
${tools_dir}/grib_dump $tempIndex1 > /dev/null

rm -rf $temp_dir_A


# ECC-1516
# ---------
${tools_dir}/grib_index_build -N -o $tempIndex1 $sample1 > /dev/null
${tools_dir}/grib_dump $tempIndex1 >/dev/null


# ECC-1773: GRIB2 multi-field messages
# -------------------------------------
infile=$data_dir/multi.grib2
${tools_dir}/grib_index_build -o $tempIndex1 $infile > $temp 2>&1
grep -q "Indexing multi-field messages is not fully supported" $temp

# Change keys before indexing
# ----------------------------
infile=$data_dir/tigge_pf_ecmwf.grib2

${tools_dir}/grib_index_build -N -o $tempIndex1 $infile > $temp
grep -q "mars.stream = { enfo }" $temp
grep -q "mars.type = { pf }" $temp

ECCODES_INDEX_SET_KEYS='typeOfProcessedData=af' ${tools_dir}/grib_index_build -N -o $tempIndex1 $infile > $temp
grep -q "mars.stream = { oper }" $temp
grep -q "mars.type = { fc }" $temp

set +e
ECCODES_INDEX_SET_KEYS='nosuchkey=1,typeOfProcessedData=af' ${tools_dir}/grib_index_build -N -o $tempIndex1 $infile > $temp 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Unable to set nosuchkey=1,typeOfProcessedData=af" $temp

set +e
ECCODES_INDEX_SET_KEYS='rubbish' ${tools_dir}/grib_index_build -N -o $tempIndex1 $infile > $temp 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Unable to parse" $temp


# ------------------
# Error conditions
# ------------------
echo GRIB > $tempGribFile1
set +e
${tools_dir}/grib_index_build $tempGribFile1 > $tempOut 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "End of resource reached" $tempOut


# Clean up
rm -f $tempOut $tempRef
rm -f $tempIndex $tempIndex1 $tempIndex2 $tempGribFile1 $tempGribFile2
