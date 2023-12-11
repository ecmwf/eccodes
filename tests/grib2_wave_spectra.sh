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
tempSample=temp.$label.tmpl
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

latest=`${tools_dir}/grib_get -p tablesVersionLatest $sample2`

# Wave 2D spectra with explicit list of frequencies and directions
${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=99 $sample2 $tempSample
${tools_dir}/grib_set -s numberOfWaveDirections=3,numberOfWaveFrequencies=3 $tempSample $temp

# Wave 2D spectra with explicit list of frequencies and directions, ensemble
${tools_dir}/grib_set -s tablesVersion=$latest,productDefinitionTemplateNumber=100 $sample2 $tempSample
grib_check_key_equals $tempSample numberOfWaveDirections,numberOfWaveFrequencies,perturbationNumber '1 1 0'

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


# Clean up
rm -f $tempSample $temp
