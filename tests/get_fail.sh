#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

REDIRECT=/dev/null

set +e 

${tools_dir}grib_get -p gribname ${data_dir}/regular_latlon_surface.grib1 2> $REDIRECT > $REDIRECT

if [ $? -eq 0 ] 
  then
  exit 1;
fi
