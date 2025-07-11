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

label="grib_compare_test"
REDIRECT=/dev/null

outfile=temp.$label.grib
rm -f $outfile

# Use of -a/-c
set +e
infile1=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
infile2=$ECCODES_SAMPLES_PATH/reduced_gg_pl_48_grib2.tmpl
${tools_dir}/grib_compare -a -c longitudeOfLastGridPoint $infile1 $infile2
status=$?
set -e
[ $status -eq 1 ]


# Use of -c namespace
infile1=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
${tools_dir}/grib_set -s year=2019 $infile1 $outfile
${tools_dir}/grib_compare -c data:n $infile1 $outfile
set +e
${tools_dir}/grib_compare -a -c data:n $infile1 $outfile
status=$?
set -e
[ $status -eq 1 ]

# Header (meta-data) keys
infile=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl
${tools_dir}/grib_set -d4 $infile $outfile
set +e
${tools_dir}/grib_compare -v $infile $outfile
status=$?
set -e
[ $status -eq 1 ]
${tools_dir}/grib_compare -b referenceValue,binaryScaleFactor $infile $outfile
${tools_dir}/grib_compare -H $infile $outfile

# ECC-2012
for infile in $ECCODES_SAMPLES_PATH/gg_sfc_grib1.tmpl $ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl; do
  grib_check_key_equals $infile bitsPerValue,binaryScaleFactor '16 -9'
  ${tools_dir}/grib_set -d4 $infile $outfile
  grib_check_key_equals $outfile bitsPerValue,binaryScaleFactor '0 0'
done

infile="${data_dir}/regular_latlon_surface.grib1"
${tools_dir}/grib_set -s shortName=2d $infile $outfile
${tools_dir}/grib_compare -b indicatorOfParameter,paramId,shortName $infile $outfile > $REDIRECT

# ----------------------------------------
# Test the -r switch
# ----------------------------------------
infile=${data_dir}/tigge_cf_ecmwf.grib2
for i in 1 2 3; do
  ${tools_dir}/grib_copy -wcount=$i $infile temp.$label.$i
done
cat temp.$label.2 temp.$label.1 temp.$label.3 > temp.$label.213
cat temp.$label.3 temp.$label.2 temp.$label.1 > temp.$label.321

# Compare files in which the messages are not in the same order
${tools_dir}/grib_compare -r temp.$label.213 temp.$label.321

# Make a change in the data values of 2nd file
${tools_dir}/grib_set -s scaleValuesBy=1.1 temp.$label.2 temp.$label.2.changed
cat temp.$label.2 temp.$label.1 temp.$label.3 > temp.$label.213
cat temp.$label.3 temp.$label.2.changed temp.$label.1 > temp.$label.321
set +e
${tools_dir}/grib_compare -d -r temp.$label.213 temp.$label.321
status=$?
set -e
[ $status -eq 1 ]

cmp temp.$label.2.changed error2_1.grib
cmp temp.$label.2         error1_1.grib
rm -f error1_1.grib error2_1.grib
rm -f temp.$label.2.changed
rm -f temp.$label.1 temp.$label.2 temp.$label.3 temp.$label.213 temp.$label.321

# ----------------------------------------------
# GRIB-797: test last argument being a directory
# ----------------------------------------------
temp_dir=tempdir.$label
rm -rf $temp_dir
mkdir $temp_dir
cp $infile $temp_dir
${tools_dir}/grib_compare $infile  $temp_dir
rm -rf $temp_dir

# ----------------------------------------
# ECC-1350: First arg is a directory
# ----------------------------------------
temp_dir=tempdir.$label
temp_err=temp.$label.err
rm -rf $temp_dir
mkdir $temp_dir
set +e
${tools_dir}/grib_compare $temp_dir $temp_dir 2>$temp_err
status=$?
set -e
[ $status -eq 1 ]
grep -q "ERROR:.*Is a directory" $temp_err
rm -rf $temp_dir $temp_err


# ----------------------------------------
# ECC-245: blacklist and 2nd order packing
# ----------------------------------------
temp1=grib_compare_temp1.grib
temp2=grib_compare_temp2.grib
${tools_dir}/grib_copy -w count=25 ${data_dir}/lfpw.grib1 $temp1
${tools_dir}/grib_copy -w count=30 ${data_dir}/lfpw.grib1 $temp2

# Compare only message headers
${tools_dir}/grib_compare -H -b level,totalLength $temp1 $temp2

# This should fail but not crash! so check exit code is not 134
set +e
${tools_dir}/grib_compare -b firstOrderValues $temp1 $temp2 >/dev/null
status=$?
set -e
[ $status -eq 1 ]

# GRIB-915: blacklisting totalLength key
${tools_dir}/grib_copy -w count=1 ${data_dir}/v.grib2 $temp1
${tools_dir}/grib_copy -w count=2 ${data_dir}/v.grib2 $temp2
# This should fail as we only blacklisted one key
set +e
${tools_dir}/grib_compare -b totalLength $temp1 $temp2 >/dev/null
status=$?
set -e
[ $status -eq 1 ]

# ----------------------------------------
# Test -A switch
# ----------------------------------------
infile=${data_dir}/sample.grib2
${tools_dir}/grib_set -s scaleValuesBy=1.01 $infile $temp1
# max absolute diff. = 3.11
set +e
${tools_dir}/grib_compare -b referenceValue -A 3.1  $infile $temp1
status=$?
set -e
[ $status -eq 1 ]
# Raise the tolerance
${tools_dir}/grib_compare -b referenceValue -A 3.2  $infile $temp1

# Invalid value
set +e
${tools_dir}/grib_compare -A badnum $infile $temp1 >$outfile 2>&1
status=$?
set -e
[ $status -eq 1 ]
grep -q "Invalid absolute error" $outfile

set +e
${tools_dir}/grib_compare -A badnum -R 88 $infile $temp1 >$outfile 2>&1
status=$?
set -e
[ $status -eq 1 ]
grep -q "Invalid absolute error" $outfile


# ----------------------------------------
# ECC-355: -R with "all" option
# ----------------------------------------
${tools_dir}/grib_copy -w count=1 ${data_dir}/tigge_cf_ecmwf.grib2 $temp1
${tools_dir}/grib_copy -w count=1 ${data_dir}/tigge_pf_ecmwf.grib2 $temp2
BLACKLIST="typeOfProcessedData,typeOfEnsembleForecast,perturbationNumber"
# Specify relative tolerances for each floating point key
${tools_dir}/grib_compare -b $BLACKLIST -R referenceValue=0.03,codedValues=2 $temp1 $temp2
# Now try the "all" option with the highest relative diff value
${tools_dir}/grib_compare -b $BLACKLIST -R all=2 $temp1 $temp2

# ----------------------------------------
# Use -w switch
# ----------------------------------------
cp ${data_dir}/tigge_cf_ecmwf.grib2 $temp1
${tools_dir}/grib_compare -w typeOfLevel=surface ${data_dir}/tigge_cf_ecmwf.grib2 $temp1


# ----------------------------------------
# Use -T switch
# ----------------------------------------
${tools_dir}/grib_compare -T5 ${data_dir}/tigge_cf_ecmwf.grib2 ${data_dir}/tigge_cf_ecmwf.grib2

# ----------------------------------------
# Summary mode (-f)
# ----------------------------------------
set +e
${tools_dir}/grib_compare -f ${data_dir}/tigge_cf_ecmwf.grib2 ${data_dir}/tigge_pf_ecmwf.grib2 > $outfile 2>&1
status=$?
set -e
[ $status -eq 1 ]
grep -q "indicatorOfUnitForTimeIncrement . 7 different" $outfile


# ----------------------------------------
# ECC-651: Two-way (symmetric) comparison
# ----------------------------------------
sample_g1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
temp_nold=$temp1
${tools_dir}/grib_set -s deleteLocalDefinition=1 $sample_g1 $temp_nold
# Now the sample has a local definition but $temp_nold does not
set +e
${tools_dir}/grib_compare $temp_nold $sample_g1 > $outfile
status=$?
set -e
[ $status -eq 1 ]
reffile=grib_compare_temp1.ref
cat > $reffile <<EOF

-- GRIB #1 -- shortName=z paramId=129 stepRange=0 levelType=pl level=500 packingType=grid_simple gridType=regular_ll --
long [totalLength]: [84] != [107]
long [section1Length]: [28] != [52]
long [section4Length]: [12] != [11]
EOF

diff $reffile $outfile

# Two-way mode enabled
set +e
${tools_dir}/grib_compare -2 $temp_nold $sample_g1 > $outfile
status=$?
set -e
[ $status -eq 1 ]
cat > $reffile <<EOF

-- GRIB #1 -- shortName=z paramId=129 stepRange=0 levelType=pl level=500 packingType=grid_simple gridType=regular_ll --
long [totalLength]: [84] != [107]
long [section1Length]: [28] != [52]
long [section4Length]: [12] != [11]
[reservedNeedNotBePresent] not found in 1st field
[localDefinitionNumber] not found in 1st field
[marsClass] not found in 1st field
[marsType] not found in 1st field
[marsStream] not found in 1st field
[experimentVersionNumber] not found in 1st field
[perturbationNumber] not found in 1st field
[numberOfForecastsInEnsemble] not found in 1st field
[padding_local1_1] not found in 1st field
EOF

diff $reffile $outfile
rm -f $reffile

sample_g2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# --------------------------------------------
# Key='Missing' in one field and not the other
# --------------------------------------------
${tools_dir}/grib_set -s scaleFactorOfFirstFixedSurface=1 $sample_g2 $temp1
set +e
${tools_dir}/grib_compare -v $sample_g2 $temp1 > $outfile
status=$?
set -e
[ $status -eq 1 ]
grep -q "scaleFactorOfFirstFixedSurface is set to missing in 1st field but is not missing in 2nd field" $outfile

set +e
${tools_dir}/grib_compare -v $temp1 $sample_g2 > $outfile
status=$?
set -e
[ $status -eq 1 ]
grep -q "scaleFactorOfFirstFixedSurface is set to missing in 2nd field but is not missing in 1st field" $outfile

${tools_dir}/grib_compare -b scaleFactorOfFirstFixedSurface $sample_g2 $temp1 > $outfile


# ----------------------------------------
# Test -R overriding "referenceValueError"
# ----------------------------------------
echo 'set values = { 9.99999957911723157871e-26 }; write;' | ${tools_dir}/grib_filter -o $temp1 - $sample_g2
echo 'set values = { 1.00000001954148137826e-25 }; write;' | ${tools_dir}/grib_filter -o $temp2 - $sample_g2
# Plain grib_compare uses the referenceValueError as tolerance and will see the files as identical
${tools_dir}/grib_compare $temp1 $temp2

# Use relative error of 0 for all keys. Now comparison detects the difference
set +e
${tools_dir}/grib_compare -Rall=0 $temp1 $temp2 2>$outfile
status=$?
set -e
[ $status -eq 1 ]

# Now use relative error of 0 for the referenceValue only
set +e
${tools_dir}/grib_compare -R referenceValue=0 $temp1 $temp2
status=$?
set -e
[ $status -eq 1 ]

# ----------------------------------------
# Through index using -S and -E
# ----------------------------------------
tempIndex1=temp.$label.1.idx
tempIndex2=temp.$label.2.idx
${tools_dir}/grib_index_build -o $tempIndex1 -N -k time,date $data_dir/tigge_ecmwf.grib2
${tools_dir}/grib_index_build -o $tempIndex2 -N -k time,date $data_dir/tigge_cf_ecmwf.grib2
set +e
${tools_dir}/grib_compare -S 4 -E 5 $tempIndex1 $tempIndex2
status=$?
set -e
[ $status -ne 0 ]

rm -f $tempIndex1 $tempIndex2

# Key unpackedValues
#-------------------
${tools_dir}/grib_set -s scaleValuesBy=1.01 $ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl $temp1
${tools_dir}/grib_compare -c unpackedValues -A 1.86 $ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl $temp1


# -----------------
# Angle comparison
# -----------------
$tools_dir/grib_set -s longitudeOfFirstGridPoint=360000 $ECCODES_SAMPLES_PATH/GRIB1.tmpl $temp1
$tools_dir/grib_set -s longitudeOfFirstGridPoint=0      $ECCODES_SAMPLES_PATH/GRIB1.tmpl $temp2
# Comparing low-level keys of type integer
set +e
${tools_dir}/grib_compare $temp1 $temp2
status=$?
set -e
[ $status -ne 0 ]
# Comparing computed keys of type double (longitudeOfFirstGridPointInDegrees)
$tools_dir/grib_compare -c geography:n $temp1 $temp2


# -----------------
# Failing cases
# -----------------
set +e
${tools_dir}/grib_compare -Rxxxx $temp1 $temp2 > $outfile 2>&1
status=$?
set -e
[ $status -eq 1 ]
grep "Invalid argument" $outfile

set +e
${tools_dir}/grib_compare -H -c data:n $temp1 $temp2 > $outfile 2>&1
status=$?
set -e
[ $status -eq 1 ]
grep -q "options are incompatible" $outfile

set +e
${tools_dir}/grib_compare -a $temp1 $temp2 > $outfile 2>&1
status=$?
set -e
[ $status -eq 1 ]
grep -q "a option requires -c option" $outfile


echo GRIB > $temp1
echo GRIB > $temp2
set +e
${tools_dir}/grib_compare $temp1 $temp2 > $outfile 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep "unreadable message" $outfile

# Clean up
# ---------
rm -f $temp1 $temp2
rm -f $outfile
