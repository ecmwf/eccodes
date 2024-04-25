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

REDIRECT=/dev/null
label="grib_filter_test"
tempFilt="temp.$label.filt"
tempGrib="temp.$label.grib"
tempOut="temp.$label.txt"
tempRef="temp.$label.ref"

if [ -f ${data_dir}/geavg.t12z.pgrbaf108 ]; then
   tmpdata=grib_api.$$.grib
#    rm -f $tmpdata
   ${tools_dir}/grib_filter ${data_dir}/filter_rules ${data_dir}/geavg.t12z.pgrbaf108 > $REDIRECT
   rm -f $tmpdata
fi

cat >${data_dir}/f.rules <<EOF
 write "${data_dir}/split/[centre]_[date]_[dataType]_[gridType]_[levelType]_[level]_[short_name]_[packingType].grib[editionNumber]";
EOF

[ -d ${data_dir}/split ] || mkdir -p ${data_dir}/split 

${tools_dir}/grib_filter ${data_dir}/f.rules ${data_dir}/tigge_pf_ecmwf.grib2 2> $REDIRECT > $REDIRECT

rm -f ${data_dir}/split/*
rmdir ${data_dir}/split
rm -f ${data_dir}/f.rules

echo "Test GRIB-308: format specifier for integer keys"
# ----------------------------------------------------
cat > ${data_dir}/formatint.rules <<EOF
 # Pad edition with leading zeroes and level with blanks
 print "edition=[edition%.3d], level=[level%5ld]";
EOF

result=`${tools_dir}/grib_filter  ${data_dir}/formatint.rules  $ECCODES_SAMPLES_PATH/GRIB1.tmpl`
[ "$result" = "edition=001, level=  500" ]

echo "Test ECC-636: print all array entries on one line, num columns=0"
# --------------------------------------------------------------------
cat > ${data_dir}/formatint.rules <<EOF
 print "[values\!0]";
EOF
numlines=`${tools_dir}/grib_filter  ${data_dir}/formatint.rules  $ECCODES_SAMPLES_PATH/GRIB1.tmpl | wc -l | tr -d ' '`
[ "$numlines" = "1" ]

# If there is an error in the num columns, use 8
cat > ${data_dir}/formatint.rules <<EOF
 print "[values\!XXX]";
EOF
numlines=`${tools_dir}/grib_filter  ${data_dir}/formatint.rules  $ECCODES_SAMPLES_PATH/GRIB1.tmpl | wc -l | tr -d ' '`
[ "$numlines" = "8146" ]

echo "Test conversion from grib1 to grib2 'Generalized vertical height coordinates'"
# --------------------------------------------------------------------------------
cat >$tempFilt <<EOF
 set edition=2;
 set typeOfLevel="generalVertical";
 set nlev=41.0;
 write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $ECCODES_SAMPLES_PATH/sh_ml_grib1.tmpl
result=`${tools_dir}/grib_get -p typeOfFirstFixedSurface,NV,nlev $tempGrib`
[ "$result" = "150 6 41" ]

echo "Test GRIB-394: grib_filter arithmetic operators not correct for floating point values"
# -----------------------------------------------------------------------------------------
cat > ${data_dir}/binop.rules <<EOF
 transient val_exact=209.53530883789062500000;
 if (referenceValue == val_exact) {
   print "OK Equality test passed";
 }
 else {
   print "***ERROR: float equality test";
   assert(0);
 }
 transient val_lower=209;
 if (referenceValue > val_lower) {
   print "OK [referenceValue] > [val_lower]";
 }
 else {
   print "***ERROR: [referenceValue] <= [val_lower]";
   assert(0);
 }
 transient val_higher=209.99;
 if (referenceValue < val_higher) {
   print "OK [referenceValue] < [val_higher]";
 }
 else {
   print "***ERROR: [referenceValue] >= [val_higher]";
   assert(0);
 }
EOF
${tools_dir}/grib_filter  ${data_dir}/binop.rules $ECCODES_SAMPLES_PATH/gg_sfc_grib1.tmpl >/dev/null


echo "Test GRIB-526: grib_filter very picky about format of floats"
# ----------------------------------------------------------------
cat >$tempFilt <<EOF
 set values = {
   -1000.0,
   3.1e5,
   66,
   -77,
   .4,
   45. };
EOF
${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/GRIB1.tmpl


echo "Test reading from stdin"
# ----------------------------
echo 'set centre="kwbc";write;' | ${tools_dir}/grib_filter -o $tempGrib - $ECCODES_SAMPLES_PATH/GRIB2.tmpl
result=`${tools_dir}/grib_get -p centre $tempGrib`
[ "$result" = "kwbc" ]


echo "Test ECC-365: placeholder in the output filename"
# -----------------------------------------------------
input=${data_dir}/tigge_cf_ecmwf.grib2
echo 'write;' | ${tools_dir}/grib_filter -o 'temp.out.gfilter.[date].[level].grib' - $input
[ -f temp.out.gfilter.20070122.925.grib ]
[ -f temp.out.gfilter.20070122.320.grib ]
[ -f temp.out.gfilter.20070122.2.grib ]
[ -f temp.out.gfilter.20070122.10.grib ]
[ -f temp.out.gfilter.20070122.0.grib ]
[ -f temp.out.gfilter.20060630.0.grib ]
[ -f temp.out.gfilter.20060623.0.grib ]
# Check the contents of one of the output files
c=`${tools_dir}/grib_count temp.out.gfilter.20070122.320.grib`
[ "$c" = 1 ]
grib_check_key_equals temp.out.gfilter.20070122.320.grib "date,level" "20070122 320"
rm -f temp.out.gfilter.*.grib


echo "Test ECC-648: Set codetable key to array"
# ---------------------------------------------
cat >$tempFilt <<EOF
 set productDefinitionTemplateNumber = 11;
 set numberOfTimeRange = 3;
 set typeOfStatisticalProcessing = {3, 1, 2};
 write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl
stats=`echo 'print "[typeOfStatisticalProcessing]";' | ${tools_dir}/grib_filter - $tempGrib`
[ "$stats" = "3 1 2" ]


echo "Test 'append'"
# -------------------
cp ${data_dir}/tigge_pf_ecmwf.grib2  $tempGrib # now has 38 msgs
${tools_dir}/grib_count $tempGrib
cat > $tempFilt <<EOF
  append "$tempGrib";
EOF
${tools_dir}/grib_filter $tempFilt ${data_dir}/tigge_pf_ecmwf.grib2 # should end up with 38*2 msgs
${tools_dir}/grib_count $tempGrib
count=`${tools_dir}/grib_count $tempGrib`
[ $count -eq 76 ]

echo "Test ECC-1233"
# ------------------
sample1=$ECCODES_SAMPLES_PATH/sh_ml_grib1.tmpl
padding=`echo 'print "[padding_grid50_1]";' | ${tools_dir}/grib_filter - $sample1`
[ "$padding" = "000000000000000000000000000000000000" ]

echo "Test switch statement"
# --------------------------
cat >$tempFilt <<EOF
switch (edition) {
  case 1: print "1";
  case 2: print "2";
  default: print "[file]: what is this?"; assert(0);
}
EOF
${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/GRIB1.tmpl $ECCODES_SAMPLES_PATH/GRIB2.tmpl

cat >$tempFilt <<EOF
switch (packingType) {
  case "grid_simple":      print "simple";
  case "grid_ccsds":       print "ccsds";
  case "spectral_complex": print "spectral";
  default: print "[file]: what is this?"; assert(0);
}
switch (referenceValue) {
  case 42.0: print "42.0";
  default: print "default case";
}
EOF
${tools_dir}/grib_filter $tempFilt $data_dir/sample.grib2 ${data_dir}/ccsds.grib2 $data_dir/spherical_model_level.grib2

cat >$tempFilt <<EOF
switch (length(packingType)) {
  # Expression 'length' evaluated as a string. Length of grid_simple is 11
  case "11": print "ok";
  default: print "[file]: bad length?"; assert(0);
}
EOF
${tools_dir}/grib_filter $tempFilt $data_dir/sample.grib2

echo "Test MISSING"
# -----------------
input="${data_dir}/reduced_gaussian_pressure_level.grib2"
grib_check_key_equals $input scaleFactorOfFirstFixedSurface 0
cat >$tempFilt <<EOF
  set scaleFactorOfFirstFixedSurface = MISSING;   # has to be uppercase
  set scaledValueOfFirstFixedSurface = missing(); # has to have parens
  write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $input
grib_check_key_equals $tempGrib scaleFactorOfFirstFixedSurface MISSING
grib_check_key_equals $tempGrib scaledValueOfFirstFixedSurface MISSING


echo "Test for the sum accessor"
# -------------------------------
input="${samp_dir}/reduced_gg_pl_32_grib2.tmpl"
cat >$tempFilt <<EOF
  meta sum_of_pl_array sum(pl);
  # Default is double
  print "sum_of_pl_array =[sum_of_pl_array]";
  print "sum_of_pl_array as ints=[sum_of_pl_array:i]";
  print "sum_of_pl_array as strs=[sum_of_pl_array:s]";
EOF
${tools_dir}/grib_filter $tempFilt $input > $tempOut


echo "Test from_scale_factor_scaled_value"
# -----------------------------------------
input="${samp_dir}/reduced_gg_pl_32_grib2.tmpl"
cat >$tempFilt <<EOF
  meta pl_scaled  from_scale_factor_scaled_value(one, pl);
  print "pl_scaled=[pl_scaled%.2f]";
EOF
${tools_dir}/grib_filter $tempFilt $input > $tempOut

cat >$tempRef <<EOF
pl_scaled=2.00 2.70 3.60 4.00 4.50 5.00 6.00 6.40 
7.20 7.50 8.00 9.00 9.00 9.60 10.00 10.80 
10.80 12.00 12.00 12.00 12.80 12.80 12.80 12.80 
12.80 12.80 12.80 12.80 12.80 12.80 12.80 12.80 
12.80 12.80 12.80 12.80 12.80 12.80 12.80 12.80 
12.80 12.80 12.80 12.80 12.00 12.00 12.00 10.80 
10.80 10.00 9.60 9.00 9.00 8.00 7.50 7.20 
6.40 6.00 5.00 4.50 4.00 3.60 2.70 2.00

EOF
diff $tempRef $tempOut


echo "Test environment variables"
# -----------------------------------------
input="${samp_dir}/GRIB2.tmpl"
cat >$tempFilt <<EOF
  transient cds = environment_variable(CDS);
  if (cds == 0) {
    print "Either CDS is undefined or defined but equal to 0";
  } else {
    print "CDS is defined and equal to [cds]";
  }
EOF
# No env var or zero
${tools_dir}/grib_filter $tempFilt $input > $tempOut
grep -q "undefined" $tempOut
CDS=0 ${tools_dir}/grib_filter $tempFilt $input > $tempOut
grep -q "defined but equal to 0" $tempOut
# Set to a non-zero integer
CDS=1 ${tools_dir}/grib_filter $tempFilt $input > $tempOut
grep -q "defined and equal to 1" $tempOut
CDS=-42 ${tools_dir}/grib_filter $tempFilt $input > $tempOut
grep -q "defined and equal to -42" $tempOut


echo "Padded count for filenames"
# -----------------------------------------
input=${data_dir}/tigge_af_ecmwf.grib2
tempDir=temp.${label}.dir
rm -fr $tempDir
mkdir -p $tempDir
cd $tempDir
cat >$tempFilt <<EOF
  meta count_padded sprintf("%.2d", count);
  write "out__[count_padded].grib";
EOF
${tools_dir}/grib_filter $tempFilt $input
[ -f out__01.grib ]
[ -f out__02.grib ]
[ -f out__39.grib ]
[ -f out__40.grib ]
cd ..
rm -rf $tempDir

# Use of 'defined' functor
cat >$tempFilt <<EOF
  if (defined(Ni)) { print "Ni defined: true"; }
  else             { print "Ni defined: false"; }
EOF
${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempOut
grep -q "Ni defined: true" $tempOut


cat >$tempFilt <<EOF
  if (defined(N)) { print "N defined: true"; }
  else            { print "N defined: false"; }
EOF
${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempOut
grep -q "N defined: false" $tempOut

cat >$tempFilt <<EOF
  if (defined()) { print "No args: true"; }
  else           { print "No args: false"; }
EOF
${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempOut
grep -q "No args: false" $tempOut

# Bit on off
cat >$tempFilt <<EOF
  transient xx1 = 7 bit 0;    print "xx1=[xx1]";
  transient xx2 = 7 notbit 0; print "xx2=[xx2]";
  transient yy1 = 6 bit 0;    print "yy1=[yy1]";
  transient yy2 = 6 notbit 0; print "yy2=[yy2]";
EOF
${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempOut
cat $tempOut
grep -q "xx1=1" $tempOut
grep -q "xx2=0" $tempOut
grep -q "yy1=0" $tempOut
grep -q "yy2=1" $tempOut


# Use of dummy expression (=true)
cat >$tempFilt <<EOF
  if (~) { print "case 1"; }
  if (!~) { assert(0); }
  else    { print "case 2"; }
EOF
${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempOut
grep -q "case 1" $tempOut
grep -q "case 2" $tempOut

# Rules
cat >$tempFilt <<EOF
 x = 8;
 y = (edition == 1);
 z = (edition == 2);
 skip;
EOF
${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempOut


# Use of the "length" expression
cat >$tempFilt <<EOF
 assert( length(identifier) == 4 );
 if (length(edition) == referenceValue) { print "matched"; }
EOF
${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl #> $tempOut


# Logical OR with doubles
cat >$tempFilt <<EOF
 if (referenceValue > 45.01 || referenceValue < 1.1) { print "yes"; }
EOF
${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl #> $tempOut


# Decode an integer key as string
cat >$tempFilt <<EOF
 print "[scaleFactorOfSecondFixedSurface:s]";
EOF
${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempOut
grep "MISSING" $tempOut

# Test string_compare
cat >$tempFilt <<EOF
 if (rubbish is "ppp") { print "yes"; } else { print "rubbish must fail"; }
 if ("ppp" is garbage) { print "yes"; } else { print "garbage must fail"; }
EOF
${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl > $tempOut 2>&1
cat $tempOut
grep "rubbish must fail" $tempOut
grep "garbage must fail" $tempOut
grep "Unable to get rubbish as string" $tempOut
grep "Unable to get garbage as string" $tempOut


# Use of "abs"
cat >$tempFilt <<EOF
 meta abs_twice_bsf evaluate( abs(binaryScaleFactor * 2) );
 assert(abs_twice_bsf == 20);
EOF
${tools_dir}/grib_filter $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl


# Write statement with padding
# ------------------------------------------------------------------------
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

echo 'write;' | ${tools_dir}/grib_filter -o $tempGrib - $input
cmp $input $tempGrib # No padding added

echo 'write(0);' | ${tools_dir}/grib_filter -o $tempGrib - $input
cmp $input $tempGrib # zero bytes padding

echo 'write(10);' | ${tools_dir}/grib_filter -o $tempGrib - $input
set +e
cmp $input $tempGrib # output should be different byte-wise
status=$?
set -e
[ $status -ne 0 ]
${tools_dir}/grib_compare $input $tempGrib # compare should succeed


# GTS header
# ---------------
input=$data_dir/gts.grib
echo 'write;' | ${tools_dir}/grib_filter -g -o $tempGrib - $input
cmp $input $tempGrib

echo 'write;' | ${tools_dir}/grib_filter -o $tempGrib - $input
set +e
cmp $input $tempGrib
status=$?
set -e
[ $status -ne 0 ]


# Setting step
# -------------
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
echo 'set step = 12; write;' | ${tools_dir}/grib_filter -o $tempGrib - $input
${tools_dir}/grib_compare -b forecastTime $input $tempGrib
grib_check_key_equals $tempGrib step 12
grib_check_key_equals $tempGrib forecastTime 12
echo 'set endStep = 12; write;' | ${tools_dir}/grib_filter -o $tempGrib - $input
grib_check_key_equals $tempGrib step 12
grib_check_key_equals $tempGrib forecastTime 12

# Functions: grib_op_ne_d
# ------------------------
input=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
cat >$tempFilt <<EOF
  assert( referenceValue != 99 );
  if (referenceValue != 9) { print "it is different"; }
EOF
${tools_dir}/grib_filter $tempFilt $input


# Clean up
rm -f $tempGrib $tempFilt $tempOut $tempRef
rm -f ${data_dir}/formatint.rules ${data_dir}/binop.rules
