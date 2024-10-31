#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.ctest.sh

for f in GRIB1.tmpl GRIB2.tmpl sh_sfc_grib1.tmpl sh_sfc_grib2.tmpl; do
    input=$ECCODES_SAMPLES_PATH/$f
    $EXEC ${test_dir}/codes_dump_action_tree GRIB "$input" > /dev/null
done

input=$ECCODES_SAMPLES_PATH/BUFR4.tmpl
$EXEC ${test_dir}/codes_dump_action_tree BUFR "$input" > /dev/null

input=$data_dir/metar/metar.txt
$EXEC ${test_dir}/codes_dump_action_tree METAR "$input" > /dev/null

input=$data_dir/gts/EGRR20150317121020_00493212.DAT
$EXEC ${test_dir}/codes_dump_action_tree GTS "$input" > /dev/null
