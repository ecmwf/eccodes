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

label="grib_mars_param_c"
fTmp=${label}.tmp
fOut=${label}.tmp.out
fRef=${label}.tmp.ref

${examples_dir}/c_mars_param

rm -f ${fTmp} ${fRef} ${fOut}
