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


#Define a common label for all the tmp files
label="examples_f_bufr_get_keys"

#Define tmp file
fTmp=${label}.tmp.txt
rm -f ${fTmp}

#We check "syno_multi.bufr". The path is
#hardcoded in the example

REDIRECT=/dev/null

#Write the values into a file and compare with reference
${examples_dir}/f_bufr_get_keys 2> $REDIRECT > $fTmp

#TODO: check the results

#cat $fTmp

#Clean up
rm -f ${fTmp}
