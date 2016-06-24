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


#Define a common label for all the tmp files
label="bufr_read_temp_f"

#Define tmp file
fTmp=${label}.tmp.txt
rm -f $fTmp | true

# The path to the BUFR file is hardcoded in the example

REDIRECT=/dev/null

# Run the example
${examples_dir}/eccodes_f_bufr_read_temp > $fTmp

#TODO: check the results

#Clean up
rm -f $fTmp

