#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

# Tests for Pseudo-GRIB format "DIAG"

. ./include.ctest.sh
label="pseudo_diag_test"
tempOut=temp.${label}.out
tempTxt=temp.${label}.txt
tempRef=temp.${label}.ref

REDIRECT=/dev/null

sample=$ECCODES_SAMPLES_PATH/diag.tmpl

# Basic grib commands should not fail
# ------------------------------------
${tools_dir}/grib_ls $sample > $REDIRECT
${tools_dir}/grib_dump $sample > $REDIRECT

count=`${tools_dir}/grib_count $sample`
[ $count -eq 1 ]


echo "Check setting integer keys..."
# ----------------------------------
echo 'set numberOfIntegers=3; set integerValues={55, 44, 66}; write;' | ${tools_dir}/grib_filter -o $tempOut - $sample
${tools_dir}/grib_dump -p numberOfFloats,numberOfIntegers,floatValues,integerValues $tempOut | sed 1d > $tempTxt
cat > $tempRef <<EOF
  numberOfFloats = 1;
  numberOfIntegers = 3;
  floatValues = 3600;
  integerValues(3) =  {
  55, 44, 66
  } 
EOF
diff $tempRef $tempTxt


echo "Check setting float keys..."
# ---------------------------------
echo 'set numberOfFloats=3; set floatValues={8.8, 9.9, 10.10}; write;' | ${tools_dir}/grib_filter -o $tempOut - $sample
${tools_dir}/grib_dump -p numberOfFloats,numberOfIntegers,floatValues,integerValues $tempOut | sed 1d > $tempTxt
cat > $tempRef <<EOF
  numberOfFloats = 3;
  numberOfIntegers = 1;
  floatValues(3) =  {
  8.8, 9.9, 10.1
  } 
  integerValues = -557;
EOF
diff $tempRef $tempTxt


echo "Check setting integer & float keys..."
# ------------------------------------------
echo 'set numberOfIntegers=4; set integerValues={33, 55, -44, 66}; set numberOfFloats=3; set floatValues={-8.8, 9.9, 10.10}; write;' |\
   ${tools_dir}/grib_filter -o $tempOut - $sample
${tools_dir}/grib_dump -p numberOfFloats,numberOfIntegers,floatValues,integerValues $tempOut | sed 1d > $tempTxt
cat > $tempRef <<EOF
  numberOfFloats = 3;
  numberOfIntegers = 4;
  floatValues(3) =  {
  -8.8, 9.9, 10.1
  } 
  integerValues(4) =  {
  33, 55, -44, 66
  } 
EOF
diff $tempRef $tempTxt


echo "Check setting charValues (as ints)..."
# ------------------------------------------
echo 'set numberOfCharacters=6; set charValues={69,67,77,87,70,32}; write;' | ${tools_dir}/grib_filter -o $tempOut - $sample
${tools_dir}/grib_dump -O $tempOut > $tempTxt
grep -q "'E', 'C', 'M', 'W', 'F', ' '" $tempTxt

# TODO encoding of characters not fully working. We are using one-byte integers instead
#echo 'set numberOfCharacters=4; set charValues={"J","u","m","p"}; write;'| ${tools_dir}/grib_filter -o $tempOut - $sample
#res=`${tools_dir}/grib_dump $tempOut | grep charValues | tr -d '\n' | tr -d ' '`
#[ "$res" = "charValues=J;charValues=u;charValues=m;charValues=p;" ]


# Clean up
rm -f $tempOut $tempRef $tempTxt
