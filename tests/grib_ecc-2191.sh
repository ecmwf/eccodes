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

# ---------------------------------------------------------
# ECC-2191: In DataG22OrderPacking.cc, hardcoded macros
# UNDEFINED_LOW (9998.9) and UNDEFINED_HIGH (9999.1) treat
# any value in [9998.9, 9999.1] as missing during complex
# packing. This corrupts legitimate data values near 9999.
# ---------------------------------------------------------

# Run the C test which covers the core scenarios
$EXEC ${test_dir}/grib_ecc-2191
