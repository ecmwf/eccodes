#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

label="grib_indexing"
temp=temp.$label.index.out
infile=${data_dir}/index.grib

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
#cat $tempOut

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

${tools_dir}/grib_index_build -N -k mars.levtype -o $tempIndex ${data_dir}/tigge_cf_ecmwf.grib2 |\
   grep -q "mars.levtype = { sfc, pl, pv, pt }"

${tools_dir}/grib_index_build -N -k mars.levtype:i -o $tempIndex ${data_dir}/tigge_cf_ecmwf.grib2 |\
   grep -q "mars.levtype = { 103, 1, 106, 100, 101, 8, 109, 107 }"

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
${tools_dir}/grib_compare $tempIndex1 $tempIndex2
rm -f $tempIndex1 $tempIndex2 $tempGribFile1 $tempGribFile2

# Clean up
rm -f $tempIndex $tempOut $tempRef
