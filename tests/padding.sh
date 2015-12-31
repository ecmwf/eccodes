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

REDIRECT=/dev/null
temp=loc.padding.grib1

${tools_dir}grib_set -s setLocalDefinition=1 ${data_dir}/regular_latlon_surface.grib1 $temp
cat  > $$_f <<EOF
	if (GRIBEXSection1Problem ) {
		print "localDefinitionNumber=[localDefinitionNumber] size(GRIBEX-section1)=[GRIBEXSection1Problem] section1Length=[section1Length]";
		write "problem.grib";
		assert(0);
	}
EOF

# Note: we cannot use -printf "%f\n" as on some unix platforms -printf is not an option
# for find. So instead we use sed to get to the filename without the fullpath
localDefinitions=`find ${def_dir}/grib1/ -name 'local.98.*def' | sed -e 's:.*/::' |\
                awk 'BEGIN {FS=".";} {print $3;}' |\
                grep -v def |\
                sed '/245/d' |\
                sed '/244/d' |\
                sed '/192/d' |\
                xargs`

for l1 in $localDefinitions
do
	${tools_dir}grib_set -s localDefinitionNumber=$l1 $temp locx.grib1
	${tools_dir}grib_filter $$_f locx.grib1
	for l2 in $localDefinitions
	do
		#echo "$l1 -> $l2"
		${tools_dir}grib_set -s localDefinitionNumber=$l2 locx.grib1 locy.grib1
		${tools_dir}grib_filter $$_f locy.grib1
	done
done

rm -f $$_f locx.grib1 locy.grib1 $temp
