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

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-2045
# GRIB: cannot clone repeatedly with headers only
# ---------------------------------------------------------

REDIRECT=/dev/null

label="grib_ecc-2045_test"
tempGrib=temp.$label.grib
tempLog=temp.$label.log

infiles="GRIB1.tmpl GRIB2.tmpl sh_ml_grib1.tmpl sh_ml_grib2.tmpl gg_sfc_grib1.tmpl"
for f in $infiles; do
    $EXEC $test_dir/grib_ecc-2045 $ECCODES_SAMPLES_PATH/$f
done


# Clean up
rm -f $tempGrib $tempLog
