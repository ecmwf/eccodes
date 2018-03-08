#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

REDIRECT=/dev/null

if [ -f ${data_dir}/geavg.t12z.pgrbaf108 ]
then
	tmpdata=grib_api.$$.grib
	rm -f $tmpdata || true
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

# Test with nonexistent keys. Note spelling of centre!
# ----------------------------------------------------
cat >${data_dir}/nonexkey.rules <<EOF
set center="john";
EOF
# Invoke without -f i.e. should fail if error encountered
set +e
${tools_dir}/grib_filter ${data_dir}/nonexkey.rules ${data_dir}/tigge_pf_ecmwf.grib2 2> $REDIRECT > $REDIRECT
if [ $? -eq 0 ]; then
   echo "grib_filter should have failed if key not found" >&2
   exit 1
fi
set -e
# Now repeat with -f option (do not exit on error)
${tools_dir}/grib_filter -f ${data_dir}/nonexkey.rules ${data_dir}/tigge_pf_ecmwf.grib2 2> $REDIRECT > $REDIRECT

rm -f ${data_dir}/nonexkey.rules

# GRIB-308 format specifier for integer keys
# -------------------------------------------
cat > ${data_dir}/formatint.rules <<EOF
# Pad edition with leading zeroes and level with blanks
print "edition=[edition%.3d], level=[level%5ld]";
EOF

result=`${tools_dir}/grib_filter  ${data_dir}/formatint.rules  $ECCODES_SAMPLES_PATH/GRIB1.tmpl`
[ "$result" = "edition=001, level=  500" ]

# ECC-636 print all array entries on one line, num columns=0
# ----------------------------------------------------------
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

# Convert from grib1 to grib2 "Generalized vertical height coordinates"
# ----------------------------------------------------------------------
cat >temp.filt <<EOF
set edition=2;
set typeOfLevel="generalVertical";
set nlev=41.0;
write;
EOF

${tools_dir}/grib_filter -o temp_filt.grib2 temp.filt $ECCODES_SAMPLES_PATH/sh_ml_grib1.tmpl
result=`${tools_dir}/grib_get -p typeOfFirstFixedSurface,NV,nlev temp_filt.grib2`
[ "$result" = "150 6 41" ]

# GRIB-394: grib_filter arithmetic operators not correct for floating point values
# --------------------------------------------------------------------------------
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


# GRIB-526 grib_filter very picky about format of floats
# -------------------------------------------------------
cat >temp.filt <<EOF
set values = {
  -1000.0,
  3.1e5,
  66,
  -77,
  .4,
  45. };
EOF
${tools_dir}/grib_filter temp.filt $ECCODES_SAMPLES_PATH/GRIB1.tmpl

# Test reading from stdin
echo 'set centre="kwbc";write;' | ${tools_dir}/grib_filter -o temp_filt.grib2 - $ECCODES_SAMPLES_PATH/GRIB2.tmpl
result=`${tools_dir}/grib_get -p centre temp_filt.grib2`
[ "$result" = "kwbc" ]


# ECC-365: placeholder in the output filename
# -------------------------------------------
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

# ECC-648: Set codetable key to array
# ------------------------------------
cat >temp.filt <<EOF
  set productDefinitionTemplateNumber = 11;
  set numberOfTimeRange = 3;
  set typeOfStatisticalProcessing = {3, 1, 2};
  write;
EOF
${tools_dir}/grib_filter -o temp_filt.grib2 temp.filt $ECCODES_SAMPLES_PATH/GRIB2.tmpl
stats=`echo 'print "[typeOfStatisticalProcessing]";' | ${tools_dir}/grib_filter - temp_filt.grib2`
[ "$stats" = "3 1 2" ]

rm -f temp_filt.grib2 temp.filt
rm -f ${data_dir}/formatint.rules ${data_dir}/binop.rules
