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

# Setting the values of a field of a multi-field message succeeds, with the legacy and the new
# simple packing
${test_dir}/grib_simple_packing_multi_field ${data_dir}/multi_created.grib2
ECCODES_USE_SP=1 ${test_dir}/grib_simple_packing_multi_field ${data_dir}/multi_created.grib2
