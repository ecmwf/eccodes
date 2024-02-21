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

label="grib_padding_test"
REDIRECT=/dev/null
tempGrib=temp.local.$label.grib1
tempFilt=temp.local.$label.filt

if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi

${tools_dir}/grib_set -s setLocalDefinition=1 ${data_dir}/regular_latlon_surface.grib1 $tempGrib
cat  > $tempFilt <<EOF
    if (GRIBEXSection1Problem ) {
        print "localDefinitionNumber=[localDefinitionNumber] size(GRIBEX-section1)=[GRIBEXSection1Problem] section1Length=[section1Length]";
        write "problem.grib";
        assert(0);
    }
EOF

# Note: we cannot use -printf "%f\n" as on some unix platforms -printf is not an option
# for find. So instead we use sed to get to the filename without the fullpath
localDefinitions=`find ${def_dir}/grib1/ -name 'local.98.*.def' | sed -e 's:.*/::' |\
                awk 'BEGIN {FS=".";} {print $3;}' |\
                grep -v def |\
                sed '/245/d' |\
                sed '/12/d' |\
                sed '/50/d' |\
                sed '/244/d' |\
                sed '/190/d' |\
                sed '/191/d' |\
                sed '/192/d' |\
                xargs`

count=0
for l1 in $localDefinitions
do
    ${tools_dir}/grib_set -M -s localDefinitionNumber=$l1 $tempGrib locx.grib1
    ${tools_dir}/grib_filter -M $tempFilt locx.grib1
    for l2 in $localDefinitions
    do
        if [ $l1 -ne $l2 ]; then
            #echo "$l1 -> $l2"
            ${tools_dir}/grib_set -M -s localDefinitionNumber=$l2 locx.grib1 locy.grib1
            ${tools_dir}/grib_filter -M $tempFilt locy.grib1
            count=$((count+1))
        fi
    done
done
echo Did $count iterations
rm -f locx.grib1 locy.grib1 $tempGrib $tempFilt
