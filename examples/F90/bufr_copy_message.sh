#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

INPUT="../../data/bufr/syno_1.bufr"
OUTPUT=out.copy.bufr

${examples_dir}eccodes_f_bufr_copy_message > /dev/null
${tools_dir}/bufr_compare -b bufrHeaderSubCentre $INPUT $OUTPUT

rm -f $OUTPUT
