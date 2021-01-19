#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

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

# Disable if autotools being used
src_config=${src_dir}/config.h
if [ -f ${src_config} ]; then
    exit 0
fi

label="grib_to_netcdf_test"
tempGrib=temp.${label}.grib
tempNetcdf=temp.${label}.nc
tempText=temp.${label}.txt

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


grib_files="\
 regular_latlon_surface.grib2 \
 regular_latlon_surface.grib1 \
 regular_gaussian_model_level.grib1 \
 regular_gaussian_pressure_level.grib2 \
 regular_gaussian_surface.grib1 \
 missing.grib2"

ncf_types="NC_SHORT NC_INT NC_FLOAT NC_DOUBLE"

# Go thru all the specified GRIB files and convert them to NetCDF
for dt in $ncf_types; do
    for f in $grib_files; do
        rm -f $tempNetcdf
        [ -f ${data_dir}/$f ]
        ${tools_dir}/grib_to_netcdf -D $dt -o $tempNetcdf ${data_dir}/$f >/dev/null
        ${tools_dir}/grib_to_netcdf -T -o $tempNetcdf ${data_dir}/$f >/dev/null
    done
done

echo "Test creating different kinds; netcdf3 classic and large ..."
# ------------------------------------------------------------------
# TODO: enable tests for netcdf4 formats too
input=${data_dir}/regular_latlon_surface.grib2
${tools_dir}/grib_to_netcdf -k 1 -o $tempNetcdf $input >/dev/null
${tools_dir}/grib_to_netcdf -k 2 -o $tempNetcdf $input >/dev/null
#${tools_dir}/grib_to_netcdf -k 3 -o $tempNetcdf $input >/dev/null
#${tools_dir}/grib_to_netcdf -k 4 -o $tempNetcdf $input >/dev/null

echo "Test ECC-1060 ..."
# ----------------------
sample2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -s productDefinitionTemplateNumber=30 $sample2 $tempGrib
${tools_dir}/grib_to_netcdf -o $tempNetcdf $tempGrib
${tools_dir}/grib_set -s productDefinitionTemplateNumber=31 $sample2 $tempGrib
${tools_dir}/grib_to_netcdf -o $tempNetcdf $tempGrib

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

# Clean up
rm -f $tempNetcdf $tempGrib $tempText
