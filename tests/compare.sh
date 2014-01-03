#!/bin/sh
# Copyright 2005-2014 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#


. ./include.sh

REDIRECT=/dev/null

infile="${data_dir}/regular_latlon_surface.grib1"
outfile=${infile}.1

rm -f $outfile || true

${tools_dir}grib_set -s shortName=2d $infile $outfile

${tools_dir}grib_compare -b indicatorOfParameter,paramId,shortName $infile $outfile > $REDIRECT

rm -f $outfile || true
