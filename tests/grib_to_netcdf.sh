#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

if [ $HAVE_GEOGRAPHY -eq 0 ]; then
    exit 0
fi

if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    # m2-bash messes with the system path.
    # %CONDA_PREFIX%\Library\usr\bin is converted to /usr/bin.
    # %CONDA_PREFIX%\Library\bin is converted to /bin.
    # However the contents of /bin and /usr/bin are identical.
    # They both point to %CONDA_PREFIX%\Library\usr\bin!
    # This means we're unable to access important dlls (like netcdf.dll) which live in
    # %CONDA_PREFIX%\Library\bin.
    # It is not obvious why this behaviour exists.
    # We add this directory back to the path manually.
    export PATH=$PATH:$CONDA_PREFIX/Library/bin
fi

label="grib_to_netcdf_test"
tempGrib=temp.${label}.grib
tempNetcdf=temp.${label}.nc
tempText=temp.${label}.txt
tempDir=temp.${label}.dir

have_netcdf4=0

# Do we have ncdump?
NC_DUMPER=""
if command -v "ncdump" >/dev/null 2>&1; then
    NC_DUMPER="ncdump"
fi


echo "Test ECC-1041: One parameter with different expvers ..."
# ------------------------------------------------------------
# This has 5 messages, all 'tp'. Change the first message to have a different expver
input=${data_dir}/tp_ecmwf.grib
${tools_dir}/grib_set -w stepRange=12 -s experimentVersionNumber=0005 $input $tempGrib
${tools_dir}/grib_to_netcdf -o $tempNetcdf $tempGrib
if test "x$NC_DUMPER" != "x"; then
    $NC_DUMPER -h $tempNetcdf > $tempText
    grep -q "short tp_0005" $tempText
    grep -q "short tp_0001" $tempText
fi

echo "Test HDF5 decoding ..."
# ---------------------------
# Note: this is only available in NetCDF-4. So need to check if the command worked with -k3
input=${data_dir}/sample.grib2
set +e
${tools_dir}/grib_to_netcdf -k3 -o $tempNetcdf $input 2>/dev/null
stat=$?
set -e
if [ $stat -eq 0 ]; then
    have_netcdf4=1
    ${tools_dir}/grib_dump -TA -O $tempNetcdf
    res=`${tools_dir}/grib_get -TA -p identifier $tempNetcdf`
    [ "$res" = "HDF5" ]
fi


grib_files="\
 regular_latlon_surface.grib2 \
 regular_latlon_surface.grib1 \
 regular_gaussian_model_level.grib1 \
 regular_gaussian_pressure_level.grib2 \
 regular_gaussian_surface.grib1 \
 missing.grib2"

ncf_types="NC_INT NC_FLOAT NC_DOUBLE"

# Go thru all the specified GRIB files and convert them to NetCDF
for dt in $ncf_types; do
    for f in $grib_files; do
        rm -f $tempNetcdf
        [ -f ${data_dir}/$f ]
        ${tools_dir}/grib_to_netcdf -D $dt -o $tempNetcdf ${data_dir}/$f >/dev/null
        ${tools_dir}/grib_to_netcdf -T -o $tempNetcdf ${data_dir}/$f >/dev/null
    done
done

echo "Test creating different kinds ..."
# ------------------------------------------------------------------
input=${data_dir}/regular_latlon_surface.grib2
${tools_dir}/grib_to_netcdf -k 1 -o $tempNetcdf $input >/dev/null
${tools_dir}/grib_to_netcdf -k 2 -o $tempNetcdf $input >/dev/null
if [ $have_netcdf4 -eq 1 ]; then
    ${tools_dir}/grib_to_netcdf -k 3 -o $tempNetcdf $input >/dev/null
    ${tools_dir}/grib_to_netcdf -k 4 -o $tempNetcdf $input >/dev/null
fi

echo "Test shuffle and deflate ..."
# ---------------------------------
if [ $have_netcdf4 -eq 1 ]; then
    input=${data_dir}/sst_globus0083.grib
    ${tools_dir}/grib_to_netcdf -s -d9 -k4 -o $tempNetcdf $input

    set +e
    ${tools_dir}/grib_to_netcdf -s -o $tempNetcdf $input > $tempText 2>&1
    status=$?
    set -e
    [ $status -ne 0 ]
    grep -q "Invalid shuffle option. Deflate option needed" $tempText

    set +e
    ${tools_dir}/grib_to_netcdf -s -dy -o $tempNetcdf $input > $tempText 2>&1
    status=$?
    set -e
    [ $status -ne 0 ]
    grep -q "Invalid number" $tempText
fi


echo "Test ECC-1060 ..."
# ----------------------
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -s productDefinitionTemplateNumber=30 $sample2 $tempGrib
${tools_dir}/grib_to_netcdf -o $tempNetcdf $tempGrib
${tools_dir}/grib_set -s productDefinitionTemplateNumber=31 $sample2 $tempGrib
${tools_dir}/grib_to_netcdf -o $tempNetcdf $tempGrib

ECCODES_DEBUG=-1 ${tools_dir}/grib_to_netcdf -o $tempNetcdf $tempGrib


echo "Test -u option ..."
# ----------------------
input=${data_dir}/sample.grib2
${tools_dir}/grib_to_netcdf -u time -o $tempNetcdf $input


echo "Test different resolutions ..."
# ------------------------------------
# This should fail as messages have different resolutions
tempGrib2=temp.${label}.2.grib
${tools_dir}/grib_set -s Ni=17,Nj=32,step=12 $ECCODES_SAMPLES_PATH/regular_ll_pl_grib2.tmpl $tempGrib
cat $ECCODES_SAMPLES_PATH/regular_ll_pl_grib2.tmpl $tempGrib > $tempGrib2
set +e
${tools_dir}/grib_to_netcdf -o $tempNetcdf $tempGrib2 2>$tempText
status=$?
set -e
[ $status = 1 ]
grep -q "GRIB message 2 has different resolution" $tempText

rm -f $tempGrib2

echo "Test directory traversal ..."
# ------------------------------------
rm -f $tempNetcdf
mkdir -p $tempDir/subdir
cp ${data_dir}/regular_latlon_surface.grib2 $tempDir/subdir
${tools_dir}/grib_to_netcdf -o $tempNetcdf $tempDir > $tempText
[ -f "$tempNetcdf" ]
grep -q "Processing input file .*/subdir/regular_latlon_surface.grib2" $tempText
rm -rf $tempDir

echo "Test non-numeric dates ..."
# ------------------------------------
sample1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
${tools_dir}/grib_set -s class=ei,type=em,yearOfCentury=255,month=3,day=20 $sample1 $tempGrib
grib_check_key_equals $tempGrib 'dataDate:s' 'mar-20'
${tools_dir}/grib_to_netcdf -o $tempNetcdf $tempGrib


echo "Test -S option..."
# ------------------------------------
input=${data_dir}/high_level_api.grib2
${tools_dir}/grib_to_netcdf -o $tempNetcdf -S param $input

echo "Test -I option..."
# ------------------------------------
input=${data_dir}/high_level_api.grib2
${tools_dir}/grib_to_netcdf -o $tempNetcdf -I method $input


echo "Enable/Disable Checks ..."
# ---------------------------------
rm -f $tempNetcdf
input=${data_dir}/regular_latlon_surface.grib2
cat $input $input > $tempGrib
# By default checks are enabled. So this should fail
set +e
${tools_dir}/grib_to_netcdf -o $tempNetcdf $tempGrib > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Wrong number of fields" $tempText


echo "Not a regular grid ..."
# --------------------------
input=${data_dir}/reduced_gaussian_pressure_level.grib2
set +e
${tools_dir}/grib_to_netcdf -o $tempNetcdf $input > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "not on a regular lat/lon grid or on a regular Gaussian grid" $tempText


echo "No output ..."
# --------------------------
set +e
${tools_dir}/grib_to_netcdf $input > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "No output file" $tempText


# ECC-1783: No error message when input file has invalid fields
input=$data_dir/bad.grib
set +e
${tools_dir}/grib_to_netcdf -o $tempNetcdf $input > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Wrong message length" $tempText


# Non-GRIB input
input=$data_dir/bufr/aaen_55.bufr
set +e
${tools_dir}/grib_to_netcdf -o $tempNetcdf $input > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Input does not contain any field" $tempText


# Bad reference date
input=$data_dir/sample.grib2
set +e
${tools_dir}/grib_to_netcdf -Rxxx -o $tempNetcdf $input > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid reference date" $tempText

# Bad -k option
input=$data_dir/sample.grib2
set +e
${tools_dir}/grib_to_netcdf -k hallo -o $tempNetcdf $input > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid value" $tempText

# Bad -d option
input=$data_dir/sample.grib2
set +e
${tools_dir}/grib_to_netcdf -d 999 -o $tempNetcdf $input > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Invalid deflate option" $tempText


# Validity time check
export GRIB_TO_NETCDF_CHECKVALIDTIME=0
${tools_dir}/grib_to_netcdf -o $tempNetcdf $tempGrib
[ -f "$tempNetcdf" ]
unset GRIB_TO_NETCDF_CHECKVALIDTIME


# Clean up
rm -f $tempNetcdf $tempGrib $tempText
