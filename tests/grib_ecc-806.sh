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
set -u
# --------------------------------------------
# This is the test for the JIRA issue ECC-806
# Local definitions precedence order
# --------------------------------------------
label="grib_ecc-806-test"
tempGrb=temp.${label}.grb
tempOut=temp.${label}.txt
tempErr=temp.${label}.err

# This NCEP grib2 file has the keys
#   discipline = 0
#   parameterCategory = 1
#   parameterNumber = 11
# Which is normally matched by 3066 but when overriden by NCEP local definitions
# should become 260056
# This assumes our definitions contain the local kwbc concepts
# See definitions/grib2/localConcepts/kwbc/paramId.def

input=$data_dir/gfs.complex.mvmu.grib2
res=`${tools_dir}/grib_get -p paramId,shortName,units,name $input`
[ "$res" = "3066 sde m Snow depth" ]

# Change the precedence rule
res=`${tools_dir}/grib_get -s preferLocalConcepts=1 -p paramId,shortName,units,name $input`
[ "$res" = "260056 sdwe kg m**-2 Water equivalent of accumulated snow depth (deprecated)" ]

# Clean up
rm -f $tempGrb $tempOut $tempErr
