#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

label="grib_multi_write_c"
tempGrib=temp.$label.grib
tempText=temp.$label.txt

${examples_dir}/c_grib_multi_write ${data_dir}/sample.grib2 $tempGrib > /dev/null

${tools_dir}/grib_get -p step $tempGrib > $tempText

reference=${data_dir}/multi_step.txt
diff $reference $tempText 

# -M = Turn multi-field support off
step=`${tools_dir}/grib_get -M -p step $tempGrib`
[ $step -eq 12 ]

# Clean up
rm -f $tempGrib $tempText
