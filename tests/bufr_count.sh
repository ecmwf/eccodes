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

#set -x

#Enter data dir
cd ${data_dir}/bufr

# counting messages
count=`${tools_dir}bufr_count syno_multi.bufr`
#[ "$count" = "3" ]
count=`${tools_dir}codes_count syno_multi.bufr`
[ "$count" = "3" ]
