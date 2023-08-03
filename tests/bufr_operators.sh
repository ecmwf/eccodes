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
label="bufr_operators_test"

tempBufr=temp.$label.bufr
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempRef=temp.$label.ref

#-----------------------------------------------------------
# Test: change width using operator 201YYY
#-----------------------------------------------------------
HIGH_TEMPERATURE=10000
cat > $tempFilt <<EOF
 set unpack=1;
 set airTemperature=$HIGH_TEMPERATURE;
 set pack=1;
 write;
EOF
input="${data_dir}/bufr/bssh_176.bufr"

 # This should fail. Out of Range
set +e
${tools_dir}/codes_bufr_filter -o $tempBufr $tempFilt $input 2>/dev/null
status=$?
set -e
[ $status -ne 0 ]

# Now change the width of airTemperature to allow high value
cat > $tempFilt <<EOF
 set unpack=1;
 set edition=4;
 set unexpandedDescriptors={301022,12023,201138,12101,201000,12023};
 set airTemperature=$HIGH_TEMPERATURE;
 set pack=1;
 print "airTemperature=[airTemperature], width=[airTemperature->width]";
 write;
EOF
${tools_dir}/codes_bufr_filter -o $tempBufr $tempFilt $input > $tempLog

cat > $tempRef <<EOF
airTemperature=$HIGH_TEMPERATURE, width=26
EOF
diff $tempRef $tempLog


#-----------------------------------------------------------
# Test: change reference val using operator 203YYY
#-----------------------------------------------------------
# Normally min. temperature = -99
#          max. nonlinearInverseSpectralWidth = 655.35
#          min. shortestOceanWavelengthOnSpectralResolution = 0
f="$ECCODES_SAMPLES_PATH/BUFR4.tmpl"
cat > $tempFilt <<EOF
 set masterTablesVersionNumber = 37;
 set inputOverriddenReferenceValues = { -100, 10000, -100 };
 set unexpandedDescriptors = { 
        203015,   12023, 42008, 42007,   203255, 
                  12023, 42008, 42007,
        203000 };
 # Now setting out-of-range values will work
 set temperature = -100;  # code 012023
 set nonlinearInverseSpectralWidth = 755; # code 042008
 set shortestOceanWavelengthOnSpectralResolution = -1; # code 042007
 set pack=1;
 write;
EOF

${tools_dir}/codes_bufr_filter -o $tempBufr $tempFilt $f
${tools_dir}/bufr_get -s unpack=1 \
   -p temperature,nonlinearInverseSpectralWidth,shortestOceanWavelengthOnSpectralResolution \
   $tempBufr > $tempLog

cat > $tempRef <<EOF
-100 755 -1
EOF
diff $tempRef $tempLog

# Try with one element
cat > $tempFilt <<EOF
 set masterTablesVersionNumber = 37;
 set inputOverriddenReferenceValues = { -150 };
 set unexpandedDescriptors = { 
        203015, 12023,  203255, 12023, 203000 };
 set temperature = -101;
 set pack=1;
 write;
EOF
${tools_dir}/codes_bufr_filter -o $tempBufr $tempFilt $f
res=`${tools_dir}/bufr_get -s unpack=1 -p temperature $tempBufr`
[ "$res" = "-101" ]


# Clean up
rm -f $tempBufr $tempRef $tempLog $tempFilt
