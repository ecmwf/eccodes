#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

TEMP=output.grib_local_MeteoFrance.grib
$EXEC ${test_dir}/grib_local_MeteoFrance $TEMP

grib_check_key_equals $TEMP "CLNOMA,INGRIB,INBITS" "SURFTEMPERATURE 120 16"

rm -f $TEMP
