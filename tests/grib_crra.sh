#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#
# ----------------------------------------------
# This is the test for CARRA/CERRA
#    epic: PS-74 Support CARRA/CERRA projects
#    epic: DGOV-198 Support C3S Regional Reanalysis in MARS (CARRA, CERRA)
# ----------------------------------------------

. ./include.ctest.sh

grib2_sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
label="grib_crra_test"
tempSample=tempSample.${label}.grib2

temp1=temp1.${label}.grib2
temp2=temp2.${label}.grib2

tempLog=temp.${label}.log

origins="no-ar-ce no-ar-cw no-ar-pa se-al-ec fr-ms-ec"
centres="enmi eswi lfpw"
pspds="10 11"
types="an fc"
pdtns="0 1" # oper/enda as per products_crra.def

#tablesVersion=$( ${tools_dir}/grib_get -p tablesVersionLatest $grib2_sample )
# use tablesVersion=32 i.e. latest one before mtg2 stuff like timespan etc


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

# ECC-1724
# Stream 'dame'
# ----------------------------------------------
${tools_dir}/grib_set -s productionStatusOfProcessedData=10 $grib2_sample $temp1
grib_check_key_equals $temp1 'mars.time,mars.step' '1200 0'

${tools_dir}/grib_set -s \
 productDefinitionTemplateNumber=8,productionStatusOfProcessedData=10,outerLoopTypeOfTimeIncrement=1,outerLoopLengthOfTimeRange=21 \
 $grib2_sample $temp1
grib_check_key_equals $temp1 'mars.stream' 'dame'
result=$(${tools_dir}/grib_get -fp mars.time,mars.step $temp1)
[ "$result" = "not_found not_found" ]

# ECC-1532
# ---------
# By default crraLocalVersion=1 which does not allow expver to be set
# because it is a constant (prod or test)
set +e
${tools_dir}/grib_set -s \
  productionStatusOfProcessedData=10,grib2LocalSectionPresent=1,marsExpver=coco \
$grib2_sample $temp1
status=$?
set -e
[ $status -ne 0 ]

# ECC-1532
# crraLocalVersion=2 has a coded key for experimentVersionNumber
${tools_dir}/grib_set -s \
  productionStatusOfProcessedData=10,grib2LocalSectionPresent=1,crraLocalVersion=2,marsExpver=coco \
$grib2_sample $temp1
grib_check_key_equals $temp1 'marsExpver,mars.expver' 'coco coco'

${tools_dir}/grib_set -s \
  productionStatusOfProcessedData=11,grib2LocalSectionPresent=1,crraLocalVersion=2,experimentVersionNumber=0078 \
$grib2_sample $temp1
grib_check_key_equals $temp1 'marsExpver,mars.expver' '0078 0078'

# ECC-2053: GRIB: alias mars.class not working in a new local concept
# crraLocalVersion=3 has a coded key for mars class
${tools_dir}/grib_set -s centre=255,productionStatusOfProcessedData=10,grib2LocalSectionPresent=1,crraLocalVersion=3 $grib2_sample $temp1
${tools_dir}/grib_set -s class=ci,expver=at99 $temp1 $temp2
grib_check_key_equals $temp1 'mars.class,mars.expver' 'rr 0002'
grib_check_key_equals $temp2 'mars.class,marsClass,mars.expver' 'ci ci at99'
${tools_dir}/grib_ls -jm $temp2 > $tempLog
grep -q "class.*ci" $tempLog

# ECC-2066
${tools_dir}/grib_set -s centre=255,productionStatusOfProcessedData=10,grib2LocalSectionPresent=1,crraLocalVersion=3,suiteName=8 \
   $grib2_sample $temp1
${tools_dir}/grib_dump -O -p section_2 $temp1 > $tempLog
grep -q "HARMONIE-AROME reanalysis by SMHI on EURO-CORDEX domain" $tempLog
grib_check_key_equals $temp1 "suiteName:s" "se-ar-ec"


# ECC-1913
# ----------
# types em/es for class=rr and expver=prod/test (and suiteName=se-al-ec though it works generally)
for pspd in $pspds ; do
  if [ $pspd -eq 10 ]; then
    expver='prod'
  else
    expver='test'
  fi
  for type in em es ; do
    # Param 228228 (accum)
    ${tools_dir}/grib_set -s \
     paramId=228228,productionStatusOfProcessedData=$pspd,grib2LocalSectionPresent=1,crraLocalVersion=1,suiteName=4 \
     $grib2_sample $temp1
    ${tools_dir}/grib_set -s \
     productionStatusOfProcessedData=$pspd,productDefinitionTemplateNumber=12,type=$type \
     $temp1 $temp2
    grib_check_key_equals $temp2 'mars.stream,mars.type,mars.expver' "enda $type $expver"
     # Param 167 (instant)
    ${tools_dir}/grib_set -s \
     paramId=167,productionStatusOfProcessedData=$pspd,grib2LocalSectionPresent=1,crraLocalVersion=1,suiteName=4 \
     $grib2_sample $temp1
    ${tools_dir}/grib_set -s \
     productionStatusOfProcessedData=$pspd,productDefinitionTemplateNumber=12,type=$type \
     $temp1 $temp2
    grib_check_key_equals $temp2 'mars.stream,mars.type,mars.expver' "enda $type $expver"
  done
done

# ECC-2294
# ----------
# ML/tiles support

expver=0001
origins="no-ar-tiles-pa no-ar-ai-ce"

for pspd in $pspds ; do
for centre in $centres ; do
for origin in $origins ; do
for type in $types ; do
for pdtn in $pdtns ; do
  if [ "$pdtn" -eq 0 -o "$pdtn" -eq 8 ] ; then
    stream=oper
  elif [ "$pdtn" -eq 1 -o "$pdtn" -eq 2 -o "$pdtn" -eq 11 -o "$pdtn" -eq 12 ] ; then
    stream=enda
  else
    echo "unknown productDefinitionTemplateNumber=$pdtn"
    exit -1
  fi
  grib_set -s tablesVersion=32,marsExpver=$expver,centre=$centre,suiteName=$origin,type=$type,productionStatusOfProcessedData=$pspd,\
grib2LocalSectionPresent=1,grib2LocalSectionNumber=0,crraLocalVersion=2,productDefinitionTemplateNumber=$pdtn $grib2_sample $temp1
  grib_check_key_equals $temp1 'centre,origin:s,mars.stream,mars.type,mars.expver' "$centre $origin $stream $type $expver"
done
done
done
done
done


# Clean up
rm -f $temp1 $temp2 $tempSample $tempLog
