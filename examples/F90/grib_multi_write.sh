#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

${examples_dir}eccodes_f_grib_multi_write > /dev/null

${tools_dir}/grib_compare ${data_dir}/multi_created.grib2 multi_created.grib2

rm -f multi_created.grib2

