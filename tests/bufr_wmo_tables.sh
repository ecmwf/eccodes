#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

# Define a common label for all the tmp files
label="bufr_wmo_tables_test"

# Define tmp bufr file
fTmp=${label}".tmp.bufr"
fRules=${label}".tmp.filt"
fDump=${label}".tmp.dump"

# --------------------------------
# Testing latest WMO tables
# --------------------------------
bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`
REDIRECT=/dev/null

blacklist=masterTablesVersionNumber

# Get the latest BUFR master tables version
sample=$ECCODES_SAMPLES_PATH/BUFR4.tmpl
latest_wmo_version=`${tools_dir}/bufr_get -p masterTablesVersionNumberLatest $sample`

# In version 29, some key names changed. So blacklist them
blacklist=$blacklist",bandwidthCorrectionCoefficient1ForAtovs,bandwidthCorrectionCoefficient2ForAtovs,tovsOrAtovsProductQualifier"
# In version 33, we changed scanLevelQualityFlags
blacklist=$blacklist",scanLevelQualityFlags"

# The units for centre also changed
blacklist=$blacklist",centre,subCentre,generatingApplication"

for file in ${bufr_files}
do
  input=${data_dir}/bufr/$file
  mtv=`${tools_dir}/bufr_get -wcount=1 -p masterTablesVersionNumber $input`
  # Must exclude old versions (before 14):
  #   1. Sequence 312060 changed content from v15 to v16:
  #      021062 backscatter became 021088 wetBackscatter!
  #   2. Descriptor 014030 width went from 16 bits to 20 bits (e.g. bssh_170.bufr)
  # Also exclude version 19:
  #   This has the issue with local tables (eum_iasi_twt.bufr and eumetsat_iasi_co.bufr)
  #   because they only work for v19 as we only have definitions/bufr/tables/0/local/19-1
  if [ $mtv -gt 13 -a $mtv -ne 19 ]; then
    ${tools_dir}/bufr_set -s  masterTablesVersionNumber=$latest_wmo_version $input $fTmp
    ${tools_dir}/bufr_dump $fTmp >/dev/null
    ${tools_dir}/bufr_compare -b $blacklist $input $fTmp
  fi
done

# In version 33 and higher, scanLevelQualityFlags (=033075) is renamed
# This BUFR uses 033075
input=${data_dir}/bufr/crit_202.bufr
${tools_dir}/bufr_get -s masterTablesVersionNumber=$latest_wmo_version,unpack=1 \
                      -p scanLevelDataValidityQualityFlags \
                      $input
# This BUFR uses 033080 which remains unchanged
input=${data_dir}/bufr/atms_201.bufr
${tools_dir}/bufr_get -s masterTablesVersionNumber=$latest_wmo_version,unpack=1 \
                      -p scanLevelQualityFlags \
                      $input

# ---------------
# Duplicate keys
# ---------------
# New keys appeared in v35. Ensure we added a digit suffix to make them unique e.g.
#   015063 = attenuatedBackscatter
#   015073 = attenuatedBackscatter1
cat > $fRules <<EOF
  set masterTablesVersionNumber=$latest_wmo_version;
  set unexpandedDescriptors={
    15063, 15073,  # attenuatedBackscatter
    15065, 15074,  # particleBackscatterCoefficient
    15067, 15075,  # particleExtinctionCoefficient
    15069, 15076,  # particleLidarRatio
    15070, 15077,  # uncertaintyInLidarRatio
    15071, 15078,  # particleDepolarizationRatio
    # 040063 got renamed in v35 and should use the new name in subsequent versions
    # Old name = scalingVectorMultiplyingTheAPrioriCoVectorInOrderToDefineTheRetrievedCoVector
    # New name = scalingVectorMultiplyingTheAPrioriVectorInOrderToDefineTheRetrievedVector
    40063
  };
  write;
EOF
${tools_dir}/codes_bufr_filter -o $fTmp $fRules $sample
${tools_dir}/bufr_dump -p $fTmp > $fDump
grep -q "attenuatedBackscatter=MISSING" $fDump
grep -q "attenuatedBackscatter1=MISSING" $fDump
grep -q "particleBackscatterCoefficient=MISSING" $fDump
grep -q "particleBackscatterCoefficient1=MISSING" $fDump
grep -q "particleExtinctionCoefficient=MISSING" $fDump
grep -q "particleExtinctionCoefficient1=MISSING" $fDump
grep -q "particleLidarRatio=MISSING" $fDump
grep -q "particleLidarRatio1=MISSING" $fDump
grep -q "uncertaintyInLidarRatio=MISSING" $fDump
grep -q "uncertaintyInLidarRatio1=MISSING" $fDump
grep -q "particleDepolarizationRatio=MISSING" $fDump
grep -q "particleDepolarizationRatio1=MISSING" $fDump
grep -q "scalingVectorMultiplyingTheAPrioriVectorInOrderToDefineTheRetrievedVector=MISSING" $fDump
set +e
# The old name should not be there 
grep -q "scalingVectorMultiplyingTheAPrioriCoVectorInOrderToDefineTheRetrievedCoVector" $fDump
status=$?
set -e
[ $status -eq 1 ]


rm -f $fTmp $fRules $fDump
