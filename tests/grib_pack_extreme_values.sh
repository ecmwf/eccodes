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

# Packing values whose reference value does not fit into an IEEE float must fail, not abort.
# Run with the default implementations and with SpAec / the new simple packing.
${test_dir}/grib_pack_extreme_values
ECCODES_USE_SPAEC=1 ECCODES_USE_SP=1 ${test_dir}/grib_pack_extreme_values
