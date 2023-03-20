#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

tempGrib="out_surface_level.grib2"

# Input and output GRIB files are hard coded in the example
${examples_dir}/c_grib_set_missing

# Check the keys have been set to MISSING
if [ -f "${tools_dir}/grib_get" ]; then
    sf=`${tools_dir}/grib_get -p scaleFactorOfFirstFixedSurface $tempGrib`
    [ "$sf" = "MISSING" ]
    sf=`${tools_dir}/grib_get -p scaledValueOfFirstFixedSurface $tempGrib`
    [ "$sf" = "MISSING" ]
fi

rm -f $tempGrib
