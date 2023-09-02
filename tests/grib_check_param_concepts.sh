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

label="grib_check_param_concepts_test"
#
# Do various checks on the concepts files
#

# First check the GRIB2 paramId.def and shortName.def
# ----------------------------------------------------
$EXEC ${test_dir}/grib_check_param_concepts paramId $ECCODES_DEFINITION_PATH/grib2/paramId.def
$EXEC ${test_dir}/grib_check_param_concepts paramId $ECCODES_DEFINITION_PATH/grib2/localConcepts/ecmf/paramId.def
$EXEC ${test_dir}/grib_check_param_concepts paramId $ECCODES_DEFINITION_PATH/grib2/localConcepts/uerra/paramId.def
$EXEC ${test_dir}/grib_check_param_concepts paramId $ECCODES_DEFINITION_PATH/grib2/localConcepts/hydro/paramId.def
$EXEC ${test_dir}/grib_check_param_concepts paramId $ECCODES_DEFINITION_PATH/grib2/localConcepts/tigge/paramId.def
$EXEC ${test_dir}/grib_check_param_concepts paramId $ECCODES_DEFINITION_PATH/grib2/localConcepts/s2s/paramId.def
$EXEC ${test_dir}/grib_check_param_concepts paramId $ECCODES_DEFINITION_PATH/grib2/localConcepts/era6/paramId.def

$EXEC ${test_dir}/grib_check_param_concepts shortName $ECCODES_DEFINITION_PATH/grib2/shortName.def
$EXEC ${test_dir}/grib_check_param_concepts shortName $ECCODES_DEFINITION_PATH/grib2/localConcepts/ecmf/shortName.def
$EXEC ${test_dir}/grib_check_param_concepts shortName $ECCODES_DEFINITION_PATH/grib2/localConcepts/uerra/shortName.def
$EXEC ${test_dir}/grib_check_param_concepts shortName $ECCODES_DEFINITION_PATH/grib2/localConcepts/hydro/shortName.def
$EXEC ${test_dir}/grib_check_param_concepts shortName $ECCODES_DEFINITION_PATH/grib2/localConcepts/tigge/shortName.def
$EXEC ${test_dir}/grib_check_param_concepts shortName $ECCODES_DEFINITION_PATH/grib2/localConcepts/s2s/shortName.def
$EXEC ${test_dir}/grib_check_param_concepts shortName $ECCODES_DEFINITION_PATH/grib2/localConcepts/era6/shortName.def

# Check the group: name.def paramId.def shortName.def units.def cfVarName.def
# ----------------------------------------------------------------------------
# Check whether the Test::More Perl module is available
set +e
perl -e 'use Test::More;'
status=$?
set -e
if [ $status -ne 0 ]; then
  echo "Perl Test::More not installed. Test will be skipped"
  exit 0
fi

CHECK_DEFS=$ECCODES_DEFINITION_PATH/check_grib_defs.pl

defs_dirs="
 $ECCODES_DEFINITION_PATH/grib1
 $ECCODES_DEFINITION_PATH/grib2
 $ECCODES_DEFINITION_PATH/grib1/localConcepts/ecmf
 $ECCODES_DEFINITION_PATH/grib2/localConcepts/ecmf

 $ECCODES_DEFINITION_PATH/grib1/localConcepts/ammc
 $ECCODES_DEFINITION_PATH/grib1/localConcepts/eidb
 $ECCODES_DEFINITION_PATH/grib1/localConcepts/ekmi
 $ECCODES_DEFINITION_PATH/grib1/localConcepts/enmi
 $ECCODES_DEFINITION_PATH/grib1/localConcepts/eswi
 $ECCODES_DEFINITION_PATH/grib1/localConcepts/lfpw
 $ECCODES_DEFINITION_PATH/grib1/localConcepts/lowm
 $ECCODES_DEFINITION_PATH/grib1/localConcepts/rjtd

 $ECCODES_DEFINITION_PATH/grib2/localConcepts/uerra
 $ECCODES_DEFINITION_PATH/grib2/localConcepts/hydro
 $ECCODES_DEFINITION_PATH/grib2/localConcepts/tigge
 $ECCODES_DEFINITION_PATH/grib2/localConcepts/s2s

 $ECCODES_DEFINITION_PATH/grib2/localConcepts/egrr
 $ECCODES_DEFINITION_PATH/grib2/localConcepts/ekmi
 $ECCODES_DEFINITION_PATH/grib2/localConcepts/eswi
 $ECCODES_DEFINITION_PATH/grib2/localConcepts/lfpw
"

for dir in $defs_dirs; do
  cd $dir
  $CHECK_DEFS
done

cd $test_dir

# -------------------------------
echo "WMO legacy parameters..."
# -------------------------------
ECMF_DIR=$ECCODES_DEFINITION_PATH/grib2

tempDir=temp.${label}.dir
rm -fr $tempDir
mkdir -p $tempDir
cd $tempDir
#cp $ECMF_DIR/cfName.legacy.def    cfName.def
#cp $ECMF_DIR/cfVarName.legacy.def cfVarName.def
cp $ECMF_DIR/name.legacy.def      name.def
cp $ECMF_DIR/paramId.legacy.def   paramId.def
cp $ECMF_DIR/shortName.legacy.def shortName.def
cp $ECMF_DIR/units.legacy.def     units.def
$CHECK_DEFS
cd $test_dir
rm -fr $tempDir

# -------------------------------
echo "ECMWF legacy parameters..."
# -------------------------------
ECMF_DIR=$ECCODES_DEFINITION_PATH/grib2/localConcepts/ecmf

tempDir=temp.${label}.dir
rm -fr $tempDir
mkdir -p $tempDir
cd $tempDir
cp $ECMF_DIR/cfName.legacy.def    cfName.def
cp $ECMF_DIR/cfVarName.legacy.def cfVarName.def
cp $ECMF_DIR/name.legacy.def      name.def
cp $ECMF_DIR/paramId.legacy.def   paramId.def
cp $ECMF_DIR/shortName.legacy.def shortName.def
cp $ECMF_DIR/units.legacy.def     units.def
$CHECK_DEFS
cd $test_dir
rm -fr $tempDir

# -------------------------------
echo "Check duplicates"
# -------------------------------
paramIdFile=$ECCODES_DEFINITION_PATH/grib2/paramId.def
pids=$(grep "^'" $paramIdFile | awk -F"'" '{printf "%s\n", $2}')
set +e
for p in $pids; do
    # For each paramId found in the top-level WMO file, check if it also exists
    # in the ECMWF local one
    grep "'$p'"  $ECCODES_DEFINITION_PATH/grib2/localConcepts/ecmf/paramId.def
    if [ $? -ne 1 ]; then
      echo "ERROR: check paramId $p. Is it duplicated?"
      exit 1
    fi
done
set -e


cd $test_dir
rm -fr $tempDir
