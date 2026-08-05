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

grib2_sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
label="grib_crra_test"
tempSample=tempSample.${label}.grib2

temp1=temp1.${label}.grib2
temp2=temp2.${label}.grib2

#tablesVersion=$( ${tools_dir}/grib_get -p tablesVersionLatest $grib2_sample )
# use tablesVersion=32 i.e. latest one before mtg2 stuff like timespan etc

# ----------------------------------------------
# epic: PS-74 Support CARRA/CERRA projects
# epic: DGOV-198 Support C3S Regional Reanalysis in MARS (CARRA, CERRA)
# ----------------------------------------------

# ECC-993 GRIB: Support for Copernicus regional reanalysis (CARRA/CERRA)
# ----------------------------------------------
# create rr prod file with local section
${tools_dir}/grib_set -s tablesVersion=23,centre=eswi,type=an,productionStatusOfProcessedData=10,grib2LocalSectionPresent=1,grib2LocalSectionNumber=0,suiteName=se-al-ec $grib2_sample $temp1
# Check that crra/cerise related keys are present and correct
grib_check_key_exists $temp1 crraLocalVersion,marsClass,marsType,marsStream,marsExpver
grib_check_key_equals $temp1 "marsClass,marsType,marsStream,marsExpver,crraLocalVersion,productionStatusOfProcessedData,suiteName:s" "rr an oper prod 1 10 se-al-ec"

# change to rr test, type to fc and stream to enda
${tools_dir}/grib_set -s productionStatusOfProcessedData=11,type=fc,productDefinitionTemplateNumber=1 $temp1 $temp2
grib_check_key_equals $temp2 "marsClass,marsType,marsStream,marsExpver,crraLocalVersion,productionStatusOfProcessedData,origin:s" "rr fc enda test 1 11 se-al-ec"

# ECC-1532 GRIB: Allow setting 'expver' on CARRA/CERRA data
# ----------------------------------------------

# create crra prod file with local section allowing expver
${tools_dir}/grib_set -s tablesVersion=23,productionStatusOfProcessedData=10,grib2LocalSectionPresent=1,crraLocalVersion=2 $grib2_sample $temp1

# change type and try some expver
${tools_dir}/grib_set -s expver=ciao,type=an $temp1 $tempSample
grib_check_key_equals $tempSample "marsClass,marsType,marsStream,marsExpver,crraLocalVersion,productionStatusOfProcessedData" "rr an oper ciao 2 10"


# ECC-2053 GRIB: Allow setting class=ci on CARRA/CERRA data
#   DGOV-469 CARRA/CERRA reanalysis on limited area in the frame of CERISE
# ----------------------------------------------

# create crra prod file with local section allowing expver, class=ci and 3 new origins (no-su-ce, no-su-pa, se-ar-ec)
${tools_dir}/grib_set -s tablesVersion=32,productionStatusOfProcessedData=10,grib2LocalSectionPresent=1,crraLocalVersion=3 $grib2_sample $temp1
# change class, type origin and expver to prod (link of the productionStatusOfProcessedData to expver is broken to allow more expvers..)
${tools_dir}/grib_set -s class=ci,type=an,origin=se-ar-ec,expver=prod $temp1 $tempSample

# Check that crra/cerise related keys are present and correct
grib_check_key_exists $tempSample crraLocalVersion,marsClass,marsType,marsStream,marsExpver
grib_check_key_equals $tempSample "marsClass,marsType,marsStream,marsExpver,crraLocalVersion,productionStatusOfProcessedData,origin:s" "ci an oper prod 3 10 se-ar-ec"

# change to crra/ci test
${tools_dir}/grib_set -s class=ci,productionStatusOfProcessedData=11,type=fc,origin=no-su-ce,expver=test $temp1 $temp2
grib_check_key_equals $temp2 "marsClass,marsType,marsStream,marsExpver,crraLocalVersion,productionStatusOfProcessedData,origin:s" "ci fc oper test 3 11 no-su-ce"


# Clean up
rm -f $temp1 $temp2 $tempSample
