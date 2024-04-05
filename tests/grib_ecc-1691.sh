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

# ECC-1691: Destination Earth ClimateDT metadata support

label="grib_ecc-1691_test"
temp_grib_a=temp.$label.a.grib
temp_grib_b=temp.$label.b.grib
destine_sample=temp.$label.destine.grib
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

tablesVersionLatest=$( ${tools_dir}/grib_get -p tablesVersionLatest $sample_grib2 )

# Setup Destine pseudo-centre GRIB message
# First latest tables version and add local section with MARS labeling
${tools_dir}/grib_set -s tablesVersion=$tablesVersionLatest,setLocalDefinition=1,grib2LocalSectionNumber=1 $sample_grib2 $temp_grib_a

# Then change to processed data = 12 --> DestinE
${tools_dir}/grib_set -s productionStatusOfProcessedData=12,class=d1 $temp_grib_a $destine_sample

# Check DestinE base related keys are present and correct
grib_check_key_exists $destine_sample destineLocalVersion,dataset,marsClass,marsType,marsStream,experimentVersionNumber
grib_check_key_equals $destine_sample "marsClass,dataset,destineLocalVersion" "d1 0 1"

# Check setting dataset to climate-dt (1). Check keys are present and equal defaults
${tools_dir}/grib_set -s dataset=1 $destine_sample $temp_grib_a

grib_check_key_exists $temp_grib_a dataset,generation,activity,experiment,model,realization,resolution
grib_check_key_equals $temp_grib_a "dataset,generation,activity,experiment,model,realization,resolution" "1 255 0 0 0 255 0"

# Check an example where a few additional things are set in climate-dt
${tools_dir}/grib_set -s dataset=1,generation=2,activity=1,experiment=1,model=2,realization=1,resolution=1 $destine_sample $temp_grib_a

grib_check_key_equals $temp_grib_a "dataset,generation,activity,experiment,model,realization,resolution" "1 2 1 1 2 1 1"
grib_check_key_equals $temp_grib_a "dataset:s,activity:s,experiment:s,model:s,resolution:s" "climate-dt CMIP6 hist IFS-NEMO standard"

# Check setting dataset to extremes-dt (2). This time we will check string also
${tools_dir}/grib_set -s dataset=2 $destine_sample $temp_grib_a

grib_check_key_equals $temp_grib_a "dataset,dataset:s" "2 extremes-dt"

# Check concepts. First check default value and then example of mapping
grib_check_key_exists $destine_sample destineOrigin,gridSpecification
grib_check_key_equals $destine_sample "destineOrigin,gridSpecification" "unknown unknown"

# Below mapping is for MeteoFrance
${tools_dir}/grib_set -s centre=98,subCentre=85 $destine_sample $temp_grib_a

grib_check_key_equals $temp_grib_a "destineOrigin" "Meteo-France"

# Below mapping is for H1024
${tools_dir}/grib_set -s numberOfDataPoints=12582912,gridDefinitionTemplateNumber=150,Nside=1024 $destine_sample $temp_grib_a

grib_check_key_equals $temp_grib_a "gridSpecification" "H1024"

# Now check streams for use in climate-dt.
# Setting stream clte and type fc should set mars.date and mars.time to dataDate and dataTime,
# and mars.step should be unaliased
# We must also set dataset=climate-dt, since in other datasets mars.step is set to stepRange

${tools_dir}/grib_set -s dataset=climate-dt,stream=clte,type=fc $destine_sample $temp_grib_a

result1=$( ${tools_dir}/grib_get -p mars.date,mars.time $temp_grib_a )
result2=$( ${tools_dir}/grib_get -p dataDate,dataTime $temp_grib_a )
[ "$result1" = "$result2" ]

${tools_dir}/grib_ls -jm $temp_grib_a
[ $( ${tools_dir}/grib_get -f -p mars.step $temp_grib_a ) = "not_found" ]

# Setting stream clmn and type fc should set mars.year and mars.month to year and month,
# and mars.date, mars.time, mars.step should be unaliased
# We must also set dataset=climate-dt, since in other datasets mars.step is set to stepRange

${tools_dir}/grib_set -s dataset=climate-dt,stream=clmn,type=fc $destine_sample $temp_grib_a

result1=$( ${tools_dir}/grib_get -p mars.year,mars.month $temp_grib_a )
result2=$( ${tools_dir}/grib_get -p year,month $temp_grib_a )
[ "$result1" = "$result2" ]

${tools_dir}/grib_ls -jm $temp_grib_a
[ $( ${tools_dir}/grib_get -f -p mars.date $temp_grib_a ) = "not_found" ]
[ $( ${tools_dir}/grib_get -f -p mars.time $temp_grib_a ) = "not_found" ]
[ $( ${tools_dir}/grib_get -f -p mars.step $temp_grib_a ) = "not_found" ]

# Clean up
rm -f $temp_grib_a $temp_grib_b $destine_sample
