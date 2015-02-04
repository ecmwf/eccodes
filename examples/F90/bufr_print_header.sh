#!/bin/sh
# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh


#-----------------------------------------------------
# Test reading the header only from a BUFR
# file with multiple messages. 
#----------------------------------------------------

#The bufr filename is hardcoded in the python example
f=${data_dir}"/bufr/syno_multi.bufr"

fRef=$f".header.ref"
fRes=$f".header.test.f"
REDIRECT=/dev/null

#Write the values into a file and compare with reference
${examples_dir}/f_bufr_print_header  2> $REDIRECT > $fRes

#We compare output to the reference by ignoring the whitespaces 
diff -w $fRef $fRes >$REDIRECT 2> $REDIRECT

#cat $fRes

rm -f $fRes

