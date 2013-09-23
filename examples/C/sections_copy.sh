#!/bin/sh
# Copyright 2005-2013 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

REGUL_GRID_FILE=../../samples/regular_ll_sfc_grib2.tmpl
GAUSS_GRID_FILE=../../samples/reduced_gg_pl_640_grib2.tmpl
OUTPUT=temp.sections.grib

${examples_dir}sections_copy $REGUL_GRID_FILE $GAUSS_GRID_FILE $OUTPUT
# Now the output should have a regular grid i.e. gridDefinitionTemplateNumber==0
# but its date should be the same as the Gaussian grid sample
grid_tmpl=`${tools_dir}grib_get -p gridDefinitionTemplateNumber,date $OUTPUT`
[ "$grid_tmpl" = "0 20100912" ]


${examples_dir}sections_copy $GAUSS_GRID_FILE $REGUL_GRID_FILE $OUTPUT
# Now the output should have a reduced gaussian grid i.e. gridDefinitionTemplateNumber==40
# but its date should be the same as the Regular grid sample
grid_tmpl=`${tools_dir}grib_get -p gridDefinitionTemplateNumber,date $OUTPUT`
[ "$grid_tmpl" = "40 20070323" ]

rm -f $OUTPUT
