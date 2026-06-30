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

# ---------------------------------------------------------
# Regression test for matrix-driven sequential PDTN selection
#
# Start from PDTN 0 (instant deterministic) and set keys that
# require step-wise PDTN transitions:
#   perturbationNumber            : 0 -> 1
#   typeOfStatisticalProcessing   : 1 -> 11
# ---------------------------------------------------------

label="grib_ecc-2301_test"
tempGribA=temp.$label.A.grib
tempGribB=temp.$label.B.grib
tempGribC=temp.$label.C.grib

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Use matrix files from the parent workspace (outside eccodes/)
export ECCODES_PDTN_MATRIX_CSV="${proj_dir}/../keys_in_PDTNS.csv"
export ECCODES_PDTN_MATRIX_INDEX="${proj_dir}/../keys_in_PDTNS_pdtns.txt"

# Sanity: start at PDTN 0
grib_check_key_equals $sample_grib2 productDefinitionTemplateNumber 0

# First missing key should move PDTN 0 -> 1
${tools_dir}/grib_set -s perturbationNumber=17 $sample_grib2 $tempGribA
grib_check_key_equals $tempGribA productDefinitionTemplateNumber,perturbationNumber '1 17'

# Next missing key should move PDTN 1 -> 11
${tools_dir}/grib_set -s typeOfStatisticalProcessing=1 $tempGribA $tempGribB
grib_check_key_equals $tempGribB productDefinitionTemplateNumber,perturbationNumber,typeOfStatisticalProcessing '11 17 1'

# One-shot set should also end up at PDTN 11 with both keys preserved
${tools_dir}/grib_set -s perturbationNumber=23,typeOfStatisticalProcessing=1 $sample_grib2 $tempGribC
grib_check_key_equals $tempGribC productDefinitionTemplateNumber,perturbationNumber,typeOfStatisticalProcessing '11 23 1'

# Clean up
rm -f $tempGribA $tempGribB $tempGribC
