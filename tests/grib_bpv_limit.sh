#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

# Test limitations in number of bits per value we can decode/encode are
# working. ecCodes can currently decode a message with a bpv <= sizeof(long)
# and encode messages with bpv < sizeof(long). Attempt to do otherwise will
# result in an error. This is valid for Simple Packing.

. ./include.ctest.sh


$EXEC ${test_dir}/grib_bpv_limit ${data_dir}/regular_latlon_surface.grib2 2>/dev/null

# TODO: grib1 does not work yet. bpv=64 but decode does not fail as expected
#for file in ${data_dir}/regular_latlon_surface.grib?; do 
#    ./grib_bpv_limit $file
#done
