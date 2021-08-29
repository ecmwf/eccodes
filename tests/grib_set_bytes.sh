#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
TEMP_ERR=temp.grib_set_bytes.log
TEMP_OUT=temp.grib_set_bytes.grib

$EXEC ${test_dir}/grib_set_bytes 2>$TEMP_ERR

grep -q "Wrong size.*for uuidOfVGrid. It is 16 bytes long" $TEMP_ERR

uuid=`${tools_dir}/grib_get -p uuidOfVGrid $TEMP_OUT`
[ "$uuid" = "07204051072040510720405207204053" ]

rm -f $TEMP_OUT $TEMP_ERR
