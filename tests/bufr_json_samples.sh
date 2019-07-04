#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh


# Test sample BUFR files
# -------------------------
for file in $ECCODES_SAMPLES_PATH/BUFR*.tmpl; do
  ${tools_dir}/bufr_dump     $file >/dev/null
  ${tools_dir}/bufr_dump -ja $file >/dev/null
  ${tools_dir}/bufr_dump -jf $file >/dev/null
done
