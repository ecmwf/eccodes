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

# Default mode and single-precision mode (ECC-1579)
${examples_dir}/f_codes_real4_get_set
ECCODES_SINGLE_PRECISION=1 ${examples_dir}/f_codes_real4_get_set
