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

label="prod_ECC-1571"  # Change prod to bufr or grib etc
tempGrib1=temp1.$label.grib2
tempGrib2=temp2.$label.grib2
sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

# 1st we create a sample file for UERRA with centre lfpw
${tools_dir}/grib_set -s centre=lfpw,productionStatusOfProcessedData=8 ${sample_grib2} ${tempGrib1}
# Now we use the UERRA sample and add a local section to it
${tools_dir}/grib_set -s setLocalDefinition=1,grib2LocalSectionNumber=0,uerraLocalVersion=1 ${tempGrib1} ${tempGrib2}

# We should have a local section with uerraLocalVersion=1,experimentVersionNumber=0002
grib_check_key_equals ${tempGrib2} uerraLocalVersion,experimentVersionNumber "1 0002"
grib_check_key_equals ${tempGrib2} 'marsExpver,mars.expver' '0002 0002'

# set other expver
${tools_dir}/grib_set -s setLocalDefinition=1,grib2LocalSectionNumber=0,uerraLocalVersion=1,experimentVersionNumber=1234 ${tempGrib1} ${tempGrib2}
grib_check_key_equals ${tempGrib2} uerraLocalVersion,experimentVersionNumber "1 1234"

rm -f ${tempGrib1} ${tempGrib2}





