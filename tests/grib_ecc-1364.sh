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


# ECC-1364: GRIB: Geoiterator for Lambert Conformal in the southern hemisphere

label="grib_ecc-1364_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempRef=temp.$label.ref

sample_grib1=$ECCODES_SAMPLES_PATH/GRIB1.tmpl

# Create a GRIB with a similar grid to the one in the JIRA issue
cat >$tempFilt<<EOF
  set dataRepresentationType = 3; # lambert conformal conic
  set resolutionAndComponentFlags = 136;
  set Nx = 999;
  set Ny = 1249;
  set latitudeOfFirstGridPoint = -54387;
  set longitudeOfFirstGridPoint = 265669;
  set LoV = 295000;
  set DxInMetres = 4000;
  set DyInMetres = 4000;
  set projectionCentreFlag = 128;
  set Latin1 = -35000;
  set Latin2 = -35000;
  set latitudeOfSouthernPole = -90000;
  set scanningMode = 64; # +i +j
  write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib1

${tools_dir}/grib_get_data $tempGrib > $tempLog

${tools_dir}/grib_ls -l -11.6277,-47.9583,1 $tempGrib > $tempLog
grep -q "Grid Point chosen #1 index=1247750 " $tempLog
grep -q "index=1247750 .* distance=0.01 " $tempLog


# Clean up
rm -f $tempGrib $tempFilt $tempLog $tempRef
