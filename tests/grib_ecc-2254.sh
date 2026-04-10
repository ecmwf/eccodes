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

# ECC-2254: Destination Earth tile metadata support

label="grib_destine_tilescheme_test"
temp_grib_a=temp.$label.a.grib
temp_grib_b=temp.$label.b.grib
destine_sample=temp.$label.destine.grib
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

tablesVersionLatest=$( ${tools_dir}/grib_get -p tablesVersionLatest $sample_grib2 )

# Setup Destine pseudo-centre GRIB message
# !!!!!!
# This ensures the dataset key is created which is needed for subsequent logic based on its value
# !!!!!!
# First latest tables version and add local section with MARS labeling
${tools_dir}/grib_set -s tablesVersion=$tablesVersionLatest,setLocalDefinition=1,grib2LocalSectionNumber=1 $sample_grib2 $temp_grib_a

# Then change to processed data = 12 --> DestinE and destineLocalVersion=2 --> DestinE tile metadata
${tools_dir}/grib_set -s productionStatusOfProcessedData=12,class=d1,destineLocalVersion=2 $temp_grib_a $destine_sample

# Check DestinE base related keys are present and correct
grib_check_key_exists $destine_sample destineLocalVersion,dataset,marsClass,marsType,marsStream,experimentVersionNumber
grib_check_key_equals $destine_sample "marsClass,dataset,destineLocalVersion" "d1 0 2"

# Check setting dataset to on-demand-extremes-dt (4). Check keys are present and equal defaults
${tools_dir}/grib_set -s dataset=4 $destine_sample $temp_grib_a

grib_check_key_exists $temp_grib_a dataset,georef,model,tilescheme
grib_check_key_equals $temp_grib_a "dataset,dataset:s,georef,mars.georef,model,mars.model,tilescheme,mars.tilescheme" "4 on-demand-extremes-dt s0000000 s0000000 IFS IFS 0 0"

# Clean up
rm -f $temp_grib_a $temp_grib_b $destine_sample
