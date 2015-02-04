#!/bin/sh
# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

#These files are hardcoded in the f90 example
f=${data_dir}/bufr/syno_multi.bufr #input
fTmp=out.clone.f.bufr #output

REDIRECT=/dev/null

#Write the values into a file and compare with reference
${examples_dir}/f_bufr_clone >$REDIRECT 2> $REDIRECT 

#Check if clone is different
set +e
${tools_dir}/bufr_compare $f $fTmp >$REDIRECT 2> $REDIRECT 

if [ $? -eq 0 ]; then
   echo "cloning produced identical files " >&2
   exit 1
fi

set -e

#Check if clone has the same number of messages
[ `${tools_dir}/bufr_count $f` = `${tools_dir}/bufr_count $fTmp` ]

#Clean up
rm -f $fTmp
