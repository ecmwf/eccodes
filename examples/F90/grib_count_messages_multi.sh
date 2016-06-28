#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

INPUT="../../data/multi.grib2"

# Without multi-field support
c=`${examples_dir}eccodes_f_grib_count_messages_multi 0 $INPUT | tr -d ' '`
[ "$c" = "30" ]

# With multi-field support (more messages should be counted)
c=`${examples_dir}eccodes_f_grib_count_messages_multi 1 $INPUT | tr -d ' '`
[ "$c" = "56" ]
