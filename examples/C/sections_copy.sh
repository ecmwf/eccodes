#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

REGUL_GRID_FILE=${proj_dir}/samples/regular_ll_sfc_grib2.tmpl
GAUSS_GRID_FILE=${proj_dir}/samples/reduced_gg_pl_640_grib2.tmpl
OUTPUT=temp.sections.grib

##################
# Copy the GRID section from REGUL_GRID_FILE
${examples_dir}/c_sections_copy $REGUL_GRID_FILE $GAUSS_GRID_FILE g $OUTPUT >/dev/null

# Now the output should have a regular grid
# but its date should be the same as the Gaussian grid sample
grid_tmpl=`${tools_dir}/grib_get -p gridType,date $OUTPUT`
[ "$grid_tmpl" = "regular_ll 20100912" ]


##################
# Copy the GRID section from GAUSS_GRID_FILE
${examples_dir}/c_sections_copy $GAUSS_GRID_FILE $REGUL_GRID_FILE g $OUTPUT >/dev/null

# Now the output should have a gaussian grid
# but its date should be the same as the regular grid
grid_tmpl=`${tools_dir}/grib_get -p gridType,date $OUTPUT`
[ "$grid_tmpl" = "reduced_gg 20070323" ]


##################
# Copy both the PRODUCT and GRID sections from REGUL_GRID_FILE
${examples_dir}/c_sections_copy $REGUL_GRID_FILE $GAUSS_GRID_FILE gp $OUTPUT >/dev/null

# Now the output should have a regular grid
# and its date should be the same as the regular latlon
grid_tmpl=`${tools_dir}/grib_get -p gridType,date $OUTPUT`
[ "$grid_tmpl" = "regular_ll 20070323" ]


##################
# Copy all sections from REGUL_GRID_FILE (use commas)
${examples_dir}/c_sections_copy $REGUL_GRID_FILE $GAUSS_GRID_FILE p,l,g,d,b $OUTPUT >/dev/null

# Now the output should be identical to the regular grib
${tools_dir}/grib_compare $REGUL_GRID_FILE $OUTPUT

rm -f $OUTPUT
