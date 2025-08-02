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

label="grib2_wave_spectra_test"
temp=temp.$label
tempGribA=temp.$label.A.grib
tempGribB=temp.$label.B.grib
tempSample=temp.$label.tmpl
tempRef=temp.$label.ref
tempOut=temp.$label.txt

sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

latest=`${tools_dir}/grib_get -p tablesVersionLatest $sample2`

# Wave 2D spectra with explicit list of frequencies and directions
${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=99 $sample2 $tempSample
${tools_dir}/grib_set -s numberOfWaveDirections=3,numberOfWaveFrequencies=3 $tempSample $temp

# ECC-1906
# --------
${tools_dir}/grib_set -s productDefinitionTemplateNumber=99,numberOfWaveDirections=5 $sample2 $tempGribA
echo 'print "[numberOfWaveDirections=] [scaledValuesOfWaveDirections=!0]";' | ${tools_dir}/grib_filter - $tempGribA > $tempOut
cat > $tempRef <<EOF
numberOfWaveDirections=5 scaledValuesOfWaveDirections=0 0 0 0 0
EOF
diff $tempRef $tempOut

echo 'set productDefinitionTemplateNumber=99; set numberOfWaveDirections=5; write;' | ${tools_dir}/grib_filter -o $tempGribB - $sample2
${tools_dir}/grib_compare $tempGribA $tempGribB

rm -f $tempGribA $tempGribB

# Wave 2D spectra with explicit list of frequencies and directions, ensemble
${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=100 $sample2 $tempSample
grib_check_key_equals $tempSample numberOfWaveDirections,numberOfWaveFrequencies,perturbationNumber '0 0 0'

# Wave 2D spectra with frequencies and directions defined by formulae
${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=101 $sample2 $tempSample
${tools_dir}/grib_set -s numberOfWaveDirections=3,numberOfWaveFrequencies=3 $tempSample $temp
grib_check_key_equals $temp numberOfWaveFrequencySequenceParameters 1
grib_check_key_equals $temp numberOfWaveDirectionSequenceParameters 1

# Wave 2D spectra with frequencies and directions defined by formulae, ensemble
${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=102 $sample2 $tempSample
${tools_dir}/grib_set -s numberOfWaveDirections=3,numberOfWaveFrequencies=3 $tempSample $temp
grib_check_key_equals $temp perturbationNumber 0
grib_check_key_equals $temp typeOfWaveFrequencySequence 0
grib_check_key_equals $temp typeOfWaveDirectionSequence 0

# Wave waves selected by period range
${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=103 $sample2 $temp
grib_check_key_equals $temp scaleFactorOfLowerWavePeriodLimit,scaledValueOfLowerWavePeriodLimit 'MISSING MISSING'

${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=104 $sample2 $temp
grib_check_key_equals $temp scaleFactorOfLowerWavePeriodLimit,scaledValueOfLowerWavePeriodLimit,perturbationNumber 'MISSING MISSING 0'

${tools_dir}/grib_set -s \
  tablesVersion=$latest,productDefinitionTemplateNumber=108,scaleFactorOfFirstWavelength=9,scaledValueOfFirstWavelength=12 \
$sample2 $temp
grib_check_key_equals $temp firstWavelengthInNanometres '12'
grib_check_key_equals $temp firstWavelengthInMetres     '1.2e-08'


# ECC-1867: Unexpected PDT change for wave template for ensemble DA streams
sample_ld=$ECCODES_SAMPLES_PATH/reduced_gg_pl_32_grib2.tmpl # Sample with a mars local def
${tools_dir}/grib_set -s tablesVersion=32,productDefinitionTemplateNumber=99,waveFrequencyNumber=14 $sample_ld $tempGribA
${tools_dir}/grib_set -s stream=ewla $tempGribA $tempGribB
grib_check_key_equals $tempGribB productDefinitionTemplateNumber,waveFrequencyNumber '99 14'
${tools_dir}/grib_compare -b marsStream $tempGribA $tempGribB

${tools_dir}/grib_set -s type=em $tempGribA $tempGribB
${tools_dir}/grib_compare -b marsType,typeOfProcessedData,typeOfGeneratingProcess $tempGribA $tempGribB
${tools_dir}/grib_set -s type=es $tempGribA $tempGribB
${tools_dir}/grib_compare -b marsType,typeOfProcessedData,typeOfGeneratingProcess $tempGribA $tempGribB

# Check PDTN
#
${tools_dir}/grib_set -s paramId=140114 $sample2 $tempGribA
grib_check_key_equals $tempGribA productDefinitionTemplateNumber,shortName '103 h1012'

# PDTN selection
# typeOfStatisticalProcessing and typeOfWavePeriodInterval combined
${tools_dir}/grib_set -s tablesVersion=35 $sample2 $tempGribA # post mtg2
${tools_dir}/grib_set -s paramId=141114 $tempGribA $tempGribB
grib_check_key_equals $tempGribB productDefinitionTemplateNumber 144

${tools_dir}/grib_set -s tablesVersion=35,eps=1,number=66 $sample2 $tempGribA # post mtg2, ensemble
${tools_dir}/grib_set -s paramId=141114 $tempGribA $tempGribB
grib_check_key_equals $tempGribB productDefinitionTemplateNumber,number '145 66'


# Clean up
rm -f $tempSample $temp $tempGribA $tempGribB $tempRef $tempOut
