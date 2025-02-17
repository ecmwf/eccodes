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

REDIRECT=/dev/null

label="grib_ecc-2005_test"
tempGrib=temp.$label.grib
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempOut=temp.$label.txt
tempRef=temp.$label.ref


# Try with a GRIB1 sample with mars keys
infile=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
$EXEC $test_dir/grib_ecc-2005 $infile

# Try with a GRIB2 sample without a local section (no mars keys)
infile=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
$EXEC $test_dir/grib_ecc-2005 $infile

# Try with a GRIB2 sample with a local section (has mars keys)
infile=$ECCODES_SAMPLES_PATH/destine_grib2.tmpl
grib_check_key_equals $infile stream,type 'oper an'
$EXEC $test_dir/grib_ecc-2005 $infile


# Clean up
rm -f $tempGrib $tempFilt $tempLog $tempOut $tempRef
