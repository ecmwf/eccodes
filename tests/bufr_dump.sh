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

bufr_files=`cat ${data_dir}/bufr/bufr_data_files.txt`
REDIRECT=/dev/null

for file in ${bufr_files}
do
  ${tools_dir}bufr_dump -O ${data_dir}/bufr/$file 2> $REDIRECT > $REDIRECT
done
