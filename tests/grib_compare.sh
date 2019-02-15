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

infile="${data_dir}/regular_latlon_surface.grib1"
outfile=${infile}.compare.$$

rm -f $outfile

${tools_dir}/grib_set -s shortName=2d $infile $outfile
${tools_dir}/grib_compare -b indicatorOfParameter,paramId,shortName $infile $outfile > $REDIRECT

# ----------------------------------------
# Test the -r switch
# ----------------------------------------
infile=${data_dir}/v.grib2
for i in 1 2 3; do
  ${tools_dir}/grib_copy -wcount=$i $infile temp_comp.$i
done
cat temp_comp.1 temp_comp.2 temp_comp.3 > temp_comp.123
cat temp_comp.3 temp_comp.2 temp_comp.1 > temp_comp.321

# Compare files in which the messages are not in the same order
${tools_dir}/grib_compare -r temp_comp.123 temp_comp.321

rm -f temp_comp.1 temp_comp.2 temp_comp.3 temp_comp.123 temp_comp.321

# ----------------------------------------
# GRIB-797: test last argument being a directory
# ----------------------------------------
temp_dir=tempdir.grib_compare
mkdir -p $temp_dir
cp $infile $temp_dir
${tools_dir}/grib_compare $infile  $temp_dir
rm -rf $temp_dir

# ----------------------------------------
# ECC-245: blacklist and 2nd order packing
# ----------------------------------------
temp1=grib_compare_temp1.grib
temp2=grib_compare_temp2.grib
${tools_dir}/grib_copy -w count=25 ${data_dir}/lfpw.grib1 $temp1
${tools_dir}/grib_copy -w count=30 ${data_dir}/lfpw.grib1 $temp2

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

# Clean up
# --------------
rm -f $temp1 $temp2
rm -f $outfile
