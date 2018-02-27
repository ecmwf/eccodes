#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

# Test limitations in number of bits per value we can decode/encode are
# working. Grib API can currently decode message with a bpv <= sizeof(long)
# and encode messages with bpv < sizeof(long). Attempt to do otherwise will
# result in an error. Packing type this is valid for is simple packing.

. ./include.sh

REDIRECT=/dev/null

for file in ${data_dir}/regular_latlon_surface.grib?; do 
    ./bpv_limit $file 2> $REDIRECT > $REDIRECT
done
