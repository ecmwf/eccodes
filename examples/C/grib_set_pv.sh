#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

GRIB1=${data_dir}/regular_latlon_surface.grib1
GRIB2=${proj_dir}/samples/regular_ll_sfc_grib2.tmpl
OUT=temp.$$.out

${examples_dir}/c_grib_set_pv $GRIB1 $OUT > /dev/null
${examples_dir}/c_grib_set_pv $GRIB2 $OUT > /dev/null

rm -f $OUT
