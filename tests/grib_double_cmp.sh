#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

# --- test on the grid_ieee type of packing
# ---   check if retrieving data through grib_get_double_array and
# ---   grib_get_double_element provides the same result

. ./include.sh

infile=${data_dir}/grid_ieee.grib

# if [ ! -f ${infile} ]
# then
#   echo no data to test
#   exit 1
# fi

${test_dir}/grib_double_cmp ${infile}
