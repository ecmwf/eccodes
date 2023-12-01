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

# Define a common label for all the tmp files
label="bufr_ecc-389_test"

fLog=${label}".log"
tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt
tempRef=temp.$label.ref
tempOut=temp.$label.out

cat > $tempFilt <<EOF
 set numberOfSubsets=2;
 set unexpandedDescriptors={310008};
 set #14#brightnessTemperature={266.53, 266.53000000001};
 set pack=1;
 write;
EOF

input="${data_dir}/bufr/amsu_55.bufr"
${tools_dir}/codes_bufr_filter -o $tempBufr $tempFilt $input

cat > $tempFilt <<EOF
 set unpack=1;
 print "[#14#brightnessTemperature]";
EOF

${tools_dir}/codes_bufr_filter $tempFilt $tempBufr > $tempOut

cat > $tempRef <<EOF
266.53
EOF
diff $tempRef $tempOut


# Clean up
rm -f $tempFilt $tempBufr $tempRef $tempOut
