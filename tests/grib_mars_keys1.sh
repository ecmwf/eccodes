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

label="grib_mars_keys1_test"
tempOut=temp.${label}.out
tempGrib=temp.${label}.grib
tempRef=temp.${label}.ref

grib1_sample=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
grib2_sample=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
types_table=$ECCODES_DEFINITION_PATH/mars/type.table
classes_table=$ECCODES_DEFINITION_PATH/mars/class.table
streams_table=$ECCODES_DEFINITION_PATH/mars/stream.table

# Check basic mars namespace keys
# --------------------------------
${tools_dir}/grib_ls -jm $grib2_sample > $tempOut
cat > $tempRef << EOF
{ "messages" : [ 
  {
    "domain": "g",
    "date": 20100912,
    "time": 1200,
    "expver": "0001",
    "class": "od",
    "type": "an",
    "stream": "oper",
    "step": 0,
    "levelist": 1000,
    "levtype": "pl",
    "param": 130
  }
]}
EOF
diff $tempRef $tempOut

# Ensemble case
${tools_dir}/grib_set -s \
    paramId=240018,stream=enfo,type=pf,productDefinitionTemplateNumber=1,perturbationNumber=5 \
    $grib2_sample $tempGrib
${tools_dir}/grib_ls -jm $tempGrib > $tempOut
cat > $tempRef << EOF
{ "messages" : [ 
  {
    "domain": "g",
    "date": 20100912,
    "time": 1200,
    "expver": "0001",
    "class": "od",
    "type": "pf",
    "stream": "enfo",
    "step": 0,
    "levelist": 1000,
    "levtype": "pl",
    "number": 5,
    "param": 240018
  }
]}
EOF
diff $tempRef $tempOut


# Check numeric codes are unique
# -------------------------------
tables="$types_table $classes_table $streams_table"
for table in $tables; do
    echo "Testing numeric codes in $table ..."
    awk '{print $1}' < $table > $tempOut
    count1=`sort -u $tempOut | wc -l`
    count2=`wc -l $tempOut | awk '{print $1}'`
    [ $count1 = $count2 ]
done

# Check abbreviations are unique
# -------------------------------
tables="$types_table $classes_table $streams_table"
for table in $tables; do
    echo "Testing abbreviations in $table ..."
    awk '{print $2}' < $table > $tempOut
    count1=`sort -u $tempOut | wc -l`
    count2=`wc -l $tempOut | awk '{print $1}'`
    [ $count1 = $count2 ]
done

# Check MARS types
# ------------------------------
# Exclude type=34 (go, Gridded observations) as it loads definitions/mars/grib.oper.go.def
# which requires the key 'N' to exist
mars_types=`awk '$1 !~ /34/ {print $1}' < $types_table`

for t in $mars_types; do
    #echo "Doing MARS type |$t|"
    ${tools_dir}/grib_set -s marsType=$t,edition=2 $grib1_sample $tempGrib
    grib_check_key_equals $tempGrib "mars.type:i" $t
done


rm -f $tempGrib $tempOut $tempRef
