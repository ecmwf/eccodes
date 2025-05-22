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

echo "SHELL is = $SHELL"
uname -a

label="grib_mtg2_switch_test"
tempGrib=temp.$label.grib
base_grib2=temp.$label.base.grib2
tempFilt=temp.$label.filt

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

#################################################
# This test is designed to test all the cases which must be catered for by the Migration to GRIB2 Switch (MTG2Switch).
# The primary test case that will be used is the migration from parameters including time ranges (e.g. max in the last
# 6 hours) to generic parameters (e.g. max). Specifically, we will test the change from:
#
# 228035; mxcape6; Maximum CAPE in the last 6 hours
#
# to:
#
# 237287; max_cape; Time-maximum CAPE
#
# We will test this for various flavours of ECMWF data (detailed below), as well as data from various datasets,
# such as CARRA/CERRA, CERISE, ERA5, ERA6, HYDRO, S2S, TIGGE and UERRA

#################################################

# First check that the keys we have added exist and are set to their default values
grib_check_key_exists $sample_grib2 tablesVersionMTG2Switch,MTG2SwitchViaTablesVersion,MTG2SwitchDefault,MTG2Switch
grib_check_key_equals $sample_grib2 tablesVersionMTG2Switch,MTG2SwitchViaTablesVersion,MTG2SwitchDefault,MTG2Switch "33 1 0 0"

# Setup GRIB message with metadata we want
${tools_dir}/grib_set -s \
    discipline=0,parameterCategory=7,parameterNumber=6,typeOfLevel=entireAtmosphere,stepType=max,stepRange=0-6 \
    $sample_grib2 $base_grib2

# Now adapt to various cases
declare -a cases
declare -a expected
## CASE 01: ECMWF OD CY48R1 pre-mtg2 ##
# tablesVersion=33
# backgroundProcess=255
# generatingProcessIdentifier=154
cases+=(temp.$label.case01.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set tablesVersion=33;
    set backgroundProcess=255;
    set generatingProcessIdentifier=154;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=0
# MTG2Switch=0
expected+=("1 0 0")

## CASE 02: ECMWF OD CY50R1 pre-mtg2 ##
# tablesVersion=35
# productionStatusOfProcessedData=0
# backgroundProcess=255
# generatingProcessIdentifier=161
cases+=(temp.$label.case02.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set tablesVersion=35;
    set productionStatusOfProcessedData=0;
    set backgroundProcess=255;
    set generatingProcessIdentifier=161;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=0
expected+=("1 1 0")

## CASE 03: ECMWF OD CY49R2 post-mtg2 ##
# tablesVersion=34
# productionStatusOfProcessedData=0
# backgroundProcess=255
# generatingProcessIdentifier=159
cases+=(temp.$label.case03.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set tablesVersion=34;
    set productionStatusOfProcessedData=0;
    set backgroundProcess=255;
    set generatingProcessIdentifier=159;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("1 1 1")

## CASE 04: ECMWF OD TEST CY50R1 post-mtg2 ##
# tablesVersion=35
# productionStatusOfProcessedData=1
# backgroundProcess=255
# generatingProcessIdentifier=161
cases+=(temp.$label.case04.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set tablesVersion=35;
    set productionStatusOfProcessedData=1;
    set backgroundProcess=255;
    set generatingProcessIdentifier=161;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("1 1 1")

## CASE 05: ECMWF OD CY50R2 post-mtg2 ##
# tablesVersion=35
# productionStatusOfProcessedData=0
# backgroundProcess=255
# generatingProcessIdentifier=162
cases+=(temp.$label.case05.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set tablesVersion=35;
    set productionStatusOfProcessedData=0;
    set backgroundProcess=255;
    set generatingProcessIdentifier=162;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("1 1 1")

## CASE 06: ECMWF OD post-mtg2 ##
# tablesVersion=37
cases+=(temp.$label.case06.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set tablesVersion=37;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("1 1 1")

## CASE 07: ECMWF RD CY50R1 pre-mtg2 ##
# tablesVersion=35
# class=rd
# productionStatusOfProcessedData=2
# backgroundProcess=255
# generatingProcessIdentifier=161
cases+=(temp.$label.case07.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="rd";
    set tablesVersion=35;
    set productionStatusOfProcessedData=2;
    set backgroundProcess=255;
    set generatingProcessIdentifier=161;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=0
expected+=("1 1 0")

## CASE 08: ECMWF RD CY50R2 post-mtg2 ##
# tablesVersion=35
# class=rd
# productionStatusOfProcessedData=2
# backgroundProcess=255
# generatingProcessIdentifier=162
cases+=(temp.$label.case08.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="rd";
    set tablesVersion=35;
    set productionStatusOfProcessedData=2;
    set backgroundProcess=255;
    set generatingProcessIdentifier=162;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("1 1 1")

## CASE 09: ECMWF MC CY49R1 pre-mtg2 ##
# tablesVersion=33
# class=mc
# productDefinitionTemplateNumber=42
# productionStatusOfProcessedData=0
# backgroundProcess=255
# generatingProcessIdentifier=158
cases+=(temp.$label.case09.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set productDefinitionTemplateNumber=42;
    set class="mc";
    set tablesVersion=33;
    set productionStatusOfProcessedData=0;
    set backgroundProcess=255;
    set generatingProcessIdentifier=158;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=0
# MTG2Switch=0
expected+=("1 0 0")

## CASE 10: ECMWF MC CY49R2 post-mtg2 ##
# tablesVersion=34
# class=mc
# productDefinitionTemplateNumber=42
# productionStatusOfProcessedData=3
# backgroundProcess=255
# generatingProcessIdentifier=159
cases+=(temp.$label.case10.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set productDefinitionTemplateNumber=42;
    set class="mc";
    set tablesVersion=34;
    set productionStatusOfProcessedData=3;
    set backgroundProcess=255;
    set generatingProcessIdentifier=159;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=2
# MTG2Switch=2
expected+=("1 2 2")

## CASE 11: ECMWF MC CY50R1 pre-mtg2 ##
# tablesVersion=35
# class=mc
# productDefinitionTemplateNumber=42
# productionStatusOfProcessedData=0
# backgroundProcess=255
# generatingProcessIdentifier=161
cases+=(temp.$label.case11.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set productDefinitionTemplateNumber=42;
    set class="mc";
    set tablesVersion=35;
    set productionStatusOfProcessedData=0;
    set backgroundProcess=255;
    set generatingProcessIdentifier=161;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=2
# MTG2Switch=0
expected+=("1 2 0")

## CASE 12: ECMWF MC TEST CY50R1 post-mtg2 ##
# tablesVersion=35
# class=mc
# productDefinitionTemplateNumber=42
# productionStatusOfProcessedData=1
# backgroundProcess=255
# generatingProcessIdentifier=161
cases+=(temp.$label.case12.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set productDefinitionTemplateNumber=42;
    set class="mc";
    set tablesVersion=35;
    set productionStatusOfProcessedData=1;
    set backgroundProcess=255;
    set generatingProcessIdentifier=161;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=2
# MTG2Switch=2
expected+=("1 2 2")

## CASE 13: ECMWF MC CY50R2 post-mtg2 ##
# tablesVersion=35
# class=mc
# productDefinitionTemplateNumber=42
# productionStatusOfProcessedData=0
# backgroundProcess=255
# generatingProcessIdentifier=162
cases+=(temp.$label.case13.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set productDefinitionTemplateNumber=42;
    set class="mc";
    set tablesVersion=35;
    set productionStatusOfProcessedData=0;
    set backgroundProcess=255;
    set generatingProcessIdentifier=162;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=2
# MTG2Switch=2
expected+=("1 2 2")

## CASE 14: ECMWF MC post-mtg2 ##
# tablesVersion=37
# class=mc
# productDefinitionTemplateNumber=42
cases+=(temp.$label.case14.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set productDefinitionTemplateNumber=42;
    set class="mc";
    set tablesVersion=37;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=2
# MTG2Switch=2
expected+=("1 2 2")

## CASE 15: ECMWF CR CY49R2 post-mtg2 ##
# tablesVersion=34
# class=cr
# productDefinitionTemplateNumber=42
# productionStatusOfProcessedData=2
# backgroundProcess=255
# generatingProcessIdentifier=159
cases+=(temp.$label.case15.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set productDefinitionTemplateNumber=42;
    set class="cr";
    set tablesVersion=34;
    set productionStatusOfProcessedData=2;
    set backgroundProcess=255;
    set generatingProcessIdentifier=159;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=2
# MTG2Switch=2
expected+=("1 2 2")

## CASE 16: ECMWF CR CY50R1 pre-mtg2 ##
# tablesVersion=35
# class=cr
# productDefinitionTemplateNumber=42
# productionStatusOfProcessedData=2
# backgroundProcess=255
# generatingProcessIdentifier=161
cases+=(temp.$label.case16.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set productDefinitionTemplateNumber=42;
    set class="cr";
    set tablesVersion=35;
    set productionStatusOfProcessedData=2;
    set backgroundProcess=255;
    set generatingProcessIdentifier=161;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=2
# MTG2Switch=0
expected+=("1 2 0")

## CASE 17: ECMWF CR CY50R2 post-mtg2 ##
# tablesVersion=35
# class=cr
# productDefinitionTemplateNumber=42
# productionStatusOfProcessedData=2
# backgroundProcess=255
# generatingProcessIdentifier=162
cases+=(temp.$label.case17.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set productDefinitionTemplateNumber=42;
    set class="cr";
    set tablesVersion=35;
    set productionStatusOfProcessedData=2;
    set backgroundProcess=255;
    set generatingProcessIdentifier=162;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=2
# MTG2Switch=2
expected+=("1 2 2")

## CASE 18: ERA6 CY49R2 post-mtg2 ##
# tablesVersion=34
# class=e6
# productionStatusOfProcessedData=3
# backgroundProcess=255
# generatingProcessIdentifier=159
cases+=(temp.$label.case18.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="e6";
    set tablesVersion=34;
    set productionStatusOfProcessedData=3;
    set backgroundProcess=255;
    set generatingProcessIdentifier=159;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=0
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("0 1 1")

## CASE 19: ERA5 pre-mtg2 ##
# tablesVersion=5
# class=ea
cases+=(temp.$label.case19.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="ea";
    set tablesVersion=5;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=0
# MTG2SwitchDefault=1
# MTG2Switch=0
expected+=("0 1 0")

## CASE 20: ERA5 tv37 pre-mtg2 ##
# tablesVersion=37
# class=ea
cases+=(temp.$label.case20.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="ea";
    set tablesVersion=37;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=0
# MTG2SwitchDefault=1
# MTG2Switch=0
expected+=("0 1 0")

## CASE 21: HYDRO pre-mtg2 ##
# tablesVersion=5
# class=ce
cases+=(temp.$label.case21.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="ce";
    set tablesVersion=5;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=0
# MTG2SwitchDefault=1
# MTG2Switch=0
expected+=("0 1 0")

## CASE 22: HYDRO tv37 pre-mtg2 ##
# tablesVersion=37
# class=ce
cases+=(temp.$label.case22.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="ce";
    set tablesVersion=37;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=0
# MTG2SwitchDefault=1
# MTG2Switch=0
expected+=("0 1 0")

## CASE 23: CERISE ECMWF pre-mtg2 ##
# tablesVersion=32
# class=ci
cases+=(temp.$label.case23.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="ci";
    set tablesVersion=32;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=0
# MTG2SwitchDefault=1
# MTG2Switch=0
expected+=("0 1 0")

## CASE 24: CERISE ECMWF tv37 pre-mtg2 ##
# tablesVersion=37
# class=ci
cases+=(temp.$label.case24.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="ci";
    set tablesVersion=37;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=0
# MTG2SwitchDefault=1
# MTG2Switch=0
expected+=("0 1 0")

## CASE 25: CERISE CNMC pre-mtg2 ##
# tablesVersion=32
# class=ci
# centre=cnmc
# subCentre=98
cases+=(temp.$label.case25.grib2)

cat >$tempFilt<<EOF
    set centre="cnmc";
    set subCentre=98;
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="ci";
    set tablesVersion=32;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=0
# MTG2SwitchDefault=1
# MTG2Switch=0
expected+=("0 1 0")

## CASE 26: CERISE CNMC tv37 pre-mtg2 ##
# tablesVersion=37
# class=ci
# centre=cnmc
# subCentre=98
cases+=(temp.$label.case26.grib2)

cat >$tempFilt<<EOF
    set centre="cnmc";
    set subCentre=98;
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="ci";
    set tablesVersion=37;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=0
# MTG2SwitchDefault=1
# MTG2Switch=0
expected+=("0 1 0")

## CASE 27: TIGGE ECMWF pre-mtg2 ##
# tablesVersion=4
# productionStatusOfProcessedData=4
cases+=(temp.$label.case27.grib2)

cat >$tempFilt<<EOF
    set tablesVersion=4;
    set productionStatusOfProcessedData=4;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=0
# MTG2Switch=0
expected+=("1 0 0")

## CASE 28: TIGGE ECMWF post-mtg2 ##
# tablesVersion=37
# productionStatusOfProcessedData=4
cases+=(temp.$label.case28.grib2)

cat >$tempFilt<<EOF
    set tablesVersion=37;
    set productionStatusOfProcessedData=4;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("1 1 1")

## CASE 29: S2S ECMWF pre-mtg2 ##
# tablesVersion=13
# productionStatusOfProcessedData=6
cases+=(temp.$label.case29.grib2)

cat >$tempFilt<<EOF
    set tablesVersion=13;
    set productionStatusOfProcessedData=6;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=0
# MTG2Switch=0
expected+=("1 0 0")

## CASE 30: S2S ECMWF post-mtg2 ##
# tablesVersion=37
# productionStatusOfProcessedData=6
cases+=(temp.$label.case30.grib2)

cat >$tempFilt<<EOF
    set tablesVersion=37;
    set productionStatusOfProcessedData=6;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("1 1 1")

## CASE 31: UERRA ECMWF pre-mtg2 ##
# tablesVersion=17
# productionStatusOfProcessedData=8
cases+=(temp.$label.case31.grib2)

cat >$tempFilt<<EOF
    set tablesVersion=17;
    set productionStatusOfProcessedData=8;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=0
# MTG2Switch=0
expected+=("1 0 0")

## CASE 32: UERRA ECMWF post-mtg2 ##
# tablesVersion=37
# productionStatusOfProcessedData=8
cases+=(temp.$label.case32.grib2)

cat >$tempFilt<<EOF
    set tablesVersion=37;
    set productionStatusOfProcessedData=8;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("1 1 1")

## CASE 33: CARRA/CERRA ECMWF pre-mtg2 ##
# tablesVersion=17
# productionStatusOfProcessedData=10
cases+=(temp.$label.case33.grib2)

cat >$tempFilt<<EOF
    set tablesVersion=17;
    set productionStatusOfProcessedData=10;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=0
# MTG2Switch=0
expected+=("1 0 0")

## CASE 34: CARRA/CERRA ECMWF post-mtg2 ##
# tablesVersion=37
# productionStatusOfProcessedData=10
cases+=(temp.$label.case34.grib2)

cat >$tempFilt<<EOF
    set tablesVersion=37;
    set productionStatusOfProcessedData=10;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("1 1 1")

## CASE 35: Contributing centres (e.g. kwbc) pre-mtg2 ##
# tablesVersion=2
# class=od
# stream=kwbc
# centre=kwbc
# subCentre=98
cases+=(temp.$label.case35.grib2)

cat >$tempFilt<<EOF
    set centre="kwbc";
    set subCentre=98;
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="od";
    set stream="kwbc";
    set tablesVersion=2;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=0
# MTG2Switch=0
expected+=("1 0 0")

## CASE 36: Contributing centres (e.g. kwbc) post-mtg2 ##
# tablesVersion=37
# class=od
# stream=kwbc
# centre=kwbc
# subCentre=98
cases+=(temp.$label.case36.grib2)

cat >$tempFilt<<EOF
    set centre="kwbc";
    set subCentre=98;
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="od";
    set stream="kwbc";
    set tablesVersion=37;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("1 1 1")

## CASE 37: ECMWF A5 CY49R2 post-mtg2 ##
# tablesVersion=34
# class=a5
# productDefinitionTemplateNumber=42
# productionStatusOfProcessedData=3
# backgroundProcess=255
# generatingProcessIdentifier=159
cases+=(temp.$label.case37.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set productDefinitionTemplateNumber=42;
    set class="a5";
    set tablesVersion=34;
    set productionStatusOfProcessedData=3;
    set backgroundProcess=255;
    set generatingProcessIdentifier=159;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=2
# MTG2Switch=2
expected+=("1 2 2")

## CASE 38: DestinE ClimateDT generation 1 pre-mtg2 ##
# tablesVersion=31
# class=d1
# productionStatusOfProcessedData=12
# dataset=climate-dt
# generation=1
cases+=(temp.$label.case38.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="d1";
    set tablesVersion=31;
    set productionStatusOfProcessedData=12;
    set dataset="climate-dt";
    set generation=1;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=0
# MTG2SwitchDefault=1
# MTG2Switch=0
expected+=("0 1 0")

## CASE 39: DestinE ClimateDT generation 1 tv37 pre-mtg2 ##
# tablesVersion=37
# class=d1
# productionStatusOfProcessedData=12
# dataset=climate-dt
# generation=1
cases+=(temp.$label.case39.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="d1";
    set tablesVersion=37;
    set productionStatusOfProcessedData=12;
    set dataset="climate-dt";
    set generation=1;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=0
# MTG2SwitchDefault=1
# MTG2Switch=0
expected+=("0 1 0")

## CASE 40: DestinE ClimateDT generation 2 pre-mtg2 ##
# tablesVersion=31
# class=d1
# productionStatusOfProcessedData=12
# dataset=climate-dt
# generation=2
cases+=(temp.$label.case40.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="d1";
    set tablesVersion=31;
    set productionStatusOfProcessedData=12;
    set dataset="climate-dt";
    set generation=2;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=0
# MTG2Switch=0
expected+=("1 0 0")

## CASE 41: DestinE ClimateDT generation 2 post-mtg2 ##
# tablesVersion=34
# class=d1
# productionStatusOfProcessedData=12
# dataset=climate-dt
# generation=2
cases+=(temp.$label.case41.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="d1";
    set tablesVersion=34;
    set productionStatusOfProcessedData=12;
    set dataset="climate-dt";
    set generation=2;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("1 1 1")

## CASE 42: DestinE ExtremesDT pre-mtg2 ##
# tablesVersion=31
# class=d1
# productionStatusOfProcessedData=12
# dataset=extremes-dt
cases+=(temp.$label.case42.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="d1";
    set tablesVersion=31;
    set productionStatusOfProcessedData=12;
    set dataset="extremes-dt";
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=0
# MTG2Switch=0
expected+=("1 0 0")

## CASE 43: DestinE ExtremesDT post-mtg2 ##
# tablesVersion=34
# class=d1
# productionStatusOfProcessedData=12
# dataset=extremes-dt
cases+=(temp.$label.case43.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="d1";
    set tablesVersion=34;
    set productionStatusOfProcessedData=12;
    set dataset="extremes-dt";
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("1 1 1")

## CASE 44: EERIE pre-mtg2 ##
# tablesVersion=32
# class=ed
cases+=(temp.$label.case44.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="ed";
    set tablesVersion=32;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2
# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=0
# MTG2Switch=0
expected+=("1 0 0")

## CASE 45: EERIE post-mtg2 ##
# tablesVersion=37
# class=ed
cases+=(temp.$label.case45.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="ed";
    set tablesVersion=37;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("1 1 1")

## CASE 46: NextGEMS pre-mtg2 ##
# tablesVersion=31
# class=ng
cases+=(temp.$label.case46.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="ng";
    set tablesVersion=31;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=0
# MTG2Switch=0
expected+=("1 0 0")

## CASE 47: NextGEMS post-mtg2 ##
# tablesVersion=37
# class=ng
cases+=(temp.$label.case47.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="ng";
    set tablesVersion=37;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("1 1 1")

## CASE 48: FIRE pre-mtg2 ##
# tablesVersion=28
# class=gw
cases+=(temp.$label.case48.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="gw";
    set tablesVersion=28;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=0
# MTG2Switch=0
expected+=("1 0 0")

## CASE 49: FIRE post-mtg2 ##
# tablesVersion=37
# class=gw
cases+=(temp.$label.case49.grib2)

cat >$tempFilt<<EOF
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="gw";
    set tablesVersion=37;
    write;
EOF

${tools_dir}/grib_filter -o ${cases[*]: -1} $tempFilt $base_grib2

# Expected result
# MTG2SwitchViaTablesVersion=1
# MTG2SwitchDefault=1
# MTG2Switch=1
expected+=("1 1 1")

# Test different cases

for i in "${!cases[@]}"; do

    # Check the values of the keys are as we expect
    grib_check_key_equals ${cases[i]} MTG2SwitchViaTablesVersion,MTG2SwitchDefault,MTG2Switch "${expected[i]}"

    # Now check parameter match
    # Pull in values of keys
    read MTG2SwitchViaTablesVersion MTG2SwitchDefault MTG2Switch <<< "${expected[i]}"
    # If pre-MTG2 we expect old paramId and to read the pre-MTG2 defs
    if [[ $MTG2Switch -eq 0 ]]; then

        grib_check_key_equals ${cases[i]} paramId           "228035"
        grib_check_key_equals ${cases[i]} shortName         "mxcape6"
        grib_check_key_equals ${cases[i]} paramIdFilename   "paramId.lte33.def"
        grib_check_key_equals ${cases[i]} shortNameFilename "shortName.lte33.def"

    # Otherwise we are post-MTG2 so we expect new paramId and to read the post-MTG2 defs
    else
        grib_check_key_equals ${cases[i]} paramId           "237287"
        grib_check_key_equals ${cases[i]} shortName         "max_cape"
        grib_check_key_equals ${cases[i]} paramIdFilename   "paramId.def"
        grib_check_key_equals ${cases[i]} shortNameFilename "shortName.def"
    fi
    echo

done

# Now test changes done in memory

# Changing tablesVersion should trigger MTG2Switch to go to the appropriate value
cat >$tempFilt<<EOF
    set tablesVersion=37;
    assert (MTG2Switch == 1);
    assert (paramId == 237287);
    write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $base_grib2

cat >$tempFilt<<EOF
    set tablesVersion=37;
    set setLocalDefinition=1;
    set grib2LocalSectionNumber=1;
    set class="mc";
    assert (MTG2Switch == 2);
    assert (paramId == 237287);
    write;
EOF

${tools_dir}/grib_filter -o $tempGrib $tempFilt $base_grib2

# Clean up
rm -f $tempGrib $tempFilt *$label*
