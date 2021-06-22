#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
set -u

label="grib_ecc-1255-test"
tempGrib=temp.$label.grib
tempFilt1=temp.1.$label.filt
tempFilt2=temp.2.$label.filt
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# Non-EFAS GRIB
# -----------------
cat > $tempFilt1 <<EOF
 print "is_efas=[is_efas]";
EOF
result=`${tools_dir}/grib_filter $tempFilt1 $sample_grib2`
[ "$result" = "is_efas=0" ]


# EFAS GRIB
# -----------
cat > $tempFilt2 <<EOF
 set setLocalDefinition = 1;
 set localDefinitionNumber = 41; # sets is_efas to 1
 write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt2 $sample_grib2

result=`${tools_dir}/grib_filter $tempFilt1 $tempGrib`
[ "$result" = "is_efas=1" ]


# Clean up
rm -f $tempGrib $tempFilt1 $tempFilt2
