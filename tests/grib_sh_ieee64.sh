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


# Note: grib_ieee64_to_long/grib_long_to_ieee64 function needs the system
#       sizeof(long) == sizeof(double).
#       So on Windows and 32bit Linux, this test is disabled.


if [ $ECCODES_ON_WINDOWS -eq 1 ]; then
    echo "$0: This test is currently disabled on Windows"
    exit 0
fi
if [ $ECCODES_ON_LINUX_32BIT -eq 1 ]; then
    echo "$0: This test is currently disabled on Linux 32 bit system"
    exit 0
fi

$EXEC $test_dir/grib_sh_ieee64
