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

label="grib_ecc-1433_test"
temp=temp.$label

# Reminder to remove the temporary hack for https://jira.ecmwf.int/browse/ECC-1455
# See file
#   definitions/grib2/template.4.horizontal.def
#
echo "Undo the change for ECC-1455!!!!"
exit 1

$EXEC $test_dir/grib_ecc-1433

rm -f $temp
