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

label="get_message_offset_p"
fTmp1=${label}.1.tmp
fTmp2=${label}.2.tmp

input=${data_dir}/mixed.grib

$PYTHON $examples_src/grib_get_message_offset.py $input > $fTmp1
${tools_dir}/grib_get -p offset:i $input > $fTmp2
diff $fTmp1 $fTmp2

rm -f $fTmp1 $fTmp2
