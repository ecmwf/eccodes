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

if [ ${HAVE_GEOGRAPHY} -ne 1 ]
then
    echo "$0: This test is disabled when HAVE_GEOGRAPHY=OFF"
    exit 0
fi

if [ ${HAVE_ECKIT_GEO} -ne 1 ]
then
    echo "$0: This test is disabled when HAVE_ECKIT_GEO=OFF"
    exit 0
fi

label="grib_geo_iterator_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt

# eckit::geo as (or better than) eccodes, of messages of each gridType: grid points as the eccodes iterator points, PROJ
# strings as projString (classical, ECCODES_ECKIT_GEO=0). Points are compared in the eccodes iterator order (reordered
# to i+j+ for lambert and mercator, eg. mercator.grib with alternativeRowScanning, and listed west to east and north to
# south for space_view); eccodes iterator points of Lambert grids scanning j- (GRIB-405), and of rotated regular Gaussian
# grids (not unrotated) are not compared.

# Compare messages: grid points (grib_geo_iterator) and PROJ strings (grib_eckit_geo_proj_string)
check() {
    ECCODES_ECKIT_GEO=0 $EXEC ${test_dir}/grib_geo_iterator "$1"
    ECCODES_ECKIT_GEO=0 $EXEC ${test_dir}/grib_eckit_geo_proj_string "$1"
}

# Compare messages of a file: all, or the messages given (grib_copy -w count=...), or windows of them (Nx Ny Xo Yo)
compare() {
    if [ $# -eq 1 ]; then
        check "$1"
        return
    fi

    ${tools_dir}/grib_copy -w count=$2 "$1" $tempGrib.in
    if [ $# -eq 6 ]; then
        if [ $(${tools_dir}/grib_get -p edition $tempGrib.in) -eq 1 ]; then
            echo "set Nx=$3; set Ny=$4; set Xo=$5; set Yo=$6; set values={1}; write;" > $tempFilt
        else
            n=$(($3 * $4))
            echo "set Nx=$3; set Ny=$4; set Xo=$5; set Yo=$6; set values={1}; set numberOfDataPoints=$n; set numberOfValues=$n; write;" > $tempFilt
        fi
        ${tools_dir}/grib_filter -o $tempGrib $tempFilt $tempGrib.in
    else
        mv $tempGrib.in $tempGrib
    fi

    check $tempGrib
    rm -f $tempGrib $tempGrib.in
}

# Messages from GRIB2.tmpl: Lambert (j+, as lambert_lam.grib2, Belgium), albers (oblate, not supported by the eccodes
# iterator)
for keys in \
    'gridType="lambert"; set Nx=49; set Ny=69; set values={1}; set numberOfDataPoints=3381; set numberOfValues=3381; set shapeOfTheEarth=6; set latitudeOfFirstGridPointInDegrees=51.199433; set longitudeOfFirstGridPointInDegrees=4.038959; set LaDInDegrees=51.967; set LoVInDegrees=4.9; set Latin1InDegrees=51.967; set Latin2InDegrees=51.967; set DxInMetres=2500; set DyInMetres=2500; set jScansPositively=1' \
    'gridType="albers"; set Nx=31; set Ny=29; set values={1}; set numberOfDataPoints=899; set numberOfValues=899; set shapeOfTheEarth=4; set latitudeOfFirstGridPointInDegrees=23; set longitudeOfFirstGridPointInDegrees=264; set LaDInDegrees=23; set LoVInDegrees=264; set Latin1InDegrees=29.5; set Latin2InDegrees=45.5; set DxInMetres=50000; set DyInMetres=50000; set jScansPositively=1'; do
    echo "set $keys; write;" > $tempFilt
    ${tools_dir}/grib_filter -o $tempGrib $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl
    check $tempGrib
    rm -f $tempGrib
done

# A single point, rotated (as an example): its point as the eccodes iterator, and its spec without increments
echo 'set gridType="rotated_ll"; set latitudeOfSouthernPoleInDegrees=-40; set longitudeOfSouthernPoleInDegrees=10; set Ni=1; set Nj=1; set latitudeOfFirstGridPointInDegrees=45; set longitudeOfFirstGridPointInDegrees=10; set latitudeOfLastGridPointInDegrees=45; set longitudeOfLastGridPointInDegrees=10; set iDirectionIncrementInDegrees=0.5; set jDirectionIncrementInDegrees=0.5; set values={1}; write;' > $tempFilt
${tools_dir}/grib_filter -o $tempGrib $tempFilt $ECCODES_SAMPLES_PATH/regular_ll_sfc_grib2.tmpl
check $tempGrib
for l in 1 2; do
    [ "$(ECCODES_ECKIT_GEO=$l ${tools_dir}/grib_get -p gridSpec $tempGrib)" = '{"area":[45,10,45,10],"grid":[0,0],"projection":{"south_pole":[10,-40],"type":"rotation"}}' ]
done
rm -f $tempGrib

# alternativeRowScanning, of each gridType (compared to the eccodes iterator, of the message without alternativeRowScanning
# if the eccodes iterator does not support it), and scanning j- (latitudes increasing from the first grid point by the
# eccodes iterator, GRIB-405: not compared)
v12="set values={1,2,3,4,5,6,7,8,9,10,11,12}; set numberOfDataPoints=12; set numberOfValues=12"
ll="set Nj=3; set latitudeOfFirstGridPointInDegrees=60; set latitudeOfLastGridPointInDegrees=58; set Ni=4; set longitudeOfFirstGridPointInDegrees=0; set longitudeOfLastGridPointInDegrees=3; set DjInDegrees=1; set DiInDegrees=1"
polar='set gridType="polar_stereographic"; set Nx=4; set Ny=3; set shapeOfTheEarth=6; set latitudeOfFirstGridPointInDegrees=38.7017; set longitudeOfFirstGridPointInDegrees=268.656832; set LaDInDegrees=60; set orientationOfTheGridInDegrees=280; set DxInMetres=50000; set DyInMetres=50000'
mercator='set gridType="mercator"; set Ni=4; set Nj=3; set shapeOfTheEarth=6; set longitudeOfFirstGridPointInDegrees=262.036499; set longitudeOfLastGridPointInDegrees=263.287713; set LaDInDegrees=14; set DiInMetres=45000; set DjInMetres=45000'
while IFS='|' read -r sample keys; do
    echo "$keys; write;" > $tempFilt
    ${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample
    check $tempGrib
    rm -f $tempGrib
done <<EOF
$ECCODES_SAMPLES_PATH/GRIB2.tmpl|$ll; $v12; set alternativeRowScanning=1
$ECCODES_SAMPLES_PATH/GRIB2.tmpl|$ll; $v12; set alternativeRowScanning=1; set jPointsAreConsecutive=1
$ECCODES_SAMPLES_PATH/GRIB2.tmpl|set gridType="rotated_ll"; set latitudeOfSouthernPoleInDegrees=-40; set longitudeOfSouthernPoleInDegrees=10; $ll; $v12; set alternativeRowScanning=1
$ECCODES_SAMPLES_PATH/GRIB2.tmpl|set gridType="lambert_azimuthal_equal_area"; set Nx=4; set Ny=3; $v12; set shapeOfTheEarth=4; set latitudeOfFirstGridPointInDegrees=66.982143; set longitudeOfFirstGridPointInDegrees=324.965976; set standardParallelInDegrees=52; set centralLongitudeInDegrees=10; set xDirectionGridLengthInMetres=50000; set yDirectionGridLengthInMetres=50000; set jScansPositively=0; set alternativeRowScanning=1
$ECCODES_SAMPLES_PATH/GRIB2.tmpl|set gridType="lambert"; set Nx=4; set Ny=3; $v12; set shapeOfTheEarth=6; set latitudeOfFirstGridPointInDegrees=51.199433; set longitudeOfFirstGridPointInDegrees=4.038959; set LaDInDegrees=51.967; set LoVInDegrees=4.9; set Latin1InDegrees=51.967; set Latin2InDegrees=51.967; set DxInMetres=2500; set DyInMetres=2500; set jScansPositively=1; set alternativeRowScanning=1
$ECCODES_SAMPLES_PATH/GRIB2.tmpl|$mercator; $v12; set latitudeOfFirstGridPointInDegrees=14.736453; set latitudeOfLastGridPointInDegrees=15.541634; set jScansPositively=1; set alternativeRowScanning=1
$ECCODES_SAMPLES_PATH/GRIB2.tmpl|$mercator; $v12; set latitudeOfFirstGridPointInDegrees=15.541634; set latitudeOfLastGridPointInDegrees=14.736453; set jScansPositively=0
$ECCODES_SAMPLES_PATH/GRIB2.tmpl|$polar; $v12; set jScansPositively=1; set alternativeRowScanning=1
$ECCODES_SAMPLES_PATH/GRIB2.tmpl|$polar; $v12; set jScansPositively=0
EOF

# space_view window, alternativeRowScanning
${tools_dir}/grib_copy -w count=1 ${data_dir}/gridspec/gridType=space_view.grib $tempGrib.in
echo "set Nx=16; set Ny=12; set Xo=700; set Yo=1700; set values={1}; set numberOfDataPoints=192; set numberOfValues=192; set alternativeRowScanning=1; write;" > $tempFilt
${tools_dir}/grib_filter -o $tempGrib $tempFilt $tempGrib.in
check $tempGrib
rm -f $tempGrib $tempGrib.in

compare ${data_dir}/regular_latlon_surface.grib1
compare ${data_dir}/regular_latlon_surface.grib2
compare "${data_dir}/gridspec/gridType=regular_ll,scanningMode=96.grib"
compare ${data_dir}/constant_field.grib2
compare ${data_dir}/gridspec/gridType=rotated_ll.grib
compare ${data_dir}/reduced_latlon_surface.grib2
compare ${data_dir}/gridspec/gridType=reduced_ll.grib 2/4
compare "${data_dir}/gridspec/gridType=reduced_gg,N320.area.grib"
compare ${data_dir}/gridspec/gridType=healpix.grib 1/2/3/4
compare ${data_dir}/gridspec/gridType=lambert.grib 1/2/3/4/5/7/8/9/10/11
compare ${data_dir}/gridspec/gridType=lambert_azimuthal_equal_area.grib 1/2
compare ${data_dir}/gridspec/gridType=mercator.grib
compare ${data_dir}/mercator.grib2
compare ${data_dir}/gridspec/gridType=polar_stereographic.grib

# space_view windows: disc centre, west limb (scanning i- j+), north limb (GRIB1)
compare ${data_dir}/gridspec/gridType=space_view.grib 1 64 64 700 1700
compare ${data_dir}/gridspec/gridType=space_view.grib 6 64 16 0 1848
compare ${data_dir}/gridspec/gridType=space_view.grib 5 64 16 1830 40
compare ${data_dir}/gridspec/gridType=space_view.grib 8 64 16 0 2742

rm -f $tempGrib $tempGrib.in $tempFilt
