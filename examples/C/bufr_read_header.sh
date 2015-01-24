#!/bin/sh
# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh


#Enter data dir
cd ${data_dir}/bufr

#-----------------------------------------------------
# Test reading the header only from a BUFR
# file with multiple messages
#----------------------------------------------------
f="syno_multi.bufr"
fRef=$f".header.ref"
fRes=$f".header.test"
REDIRECT=/dev/null

#Write the values into a file and compare with reference
${examples_dir}/bufr_read_header  ${data_dir}/bufr/$f 2> $REDIRECT > $fRes
diff $fRef $fRes >$REDIRECT 2> $REDIRECT

rm -f $fRes
