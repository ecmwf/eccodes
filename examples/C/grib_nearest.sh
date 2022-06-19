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
set -u

label="grib_nearest_c"
tempTxt=temp.$label.txt
tempRef=temp.$label.ref

input=${data_dir}/high_level_api.grib2
${examples_dir}/c_grib_nearest $input > $tempTxt

cat > $tempRef <<EOF

ordering by shortName,step:i

5 fields in the fieldset
Msg #1, step=0, shortName=msl
Idx	lat	lon	dist	val
58017	-40.85	15.30	97.627	101524
58016	-40.85	14.40	107.078	101570
57617	-39.95	15.30	26.1625	101655
57616	-39.95	14.40	51.4303	101669

Msg #2, step=6, shortName=msl
Idx	lat	lon	dist	val
58017	-40.85	15.30	97.627	101547
58016	-40.85	14.40	107.078	101605
57617	-39.95	15.30	26.1625	101589
57616	-39.95	14.40	51.4303	101641

Msg #3, step=12, shortName=msl
Idx	lat	lon	dist	val
58017	-40.85	15.30	97.627	101606
58016	-40.85	14.40	107.078	101623
57617	-39.95	15.30	26.1625	101652
57616	-39.95	14.40	51.4303	101678

Msg #4, step=18, shortName=msl
Idx	lat	lon	dist	val
58017	-40.85	15.30	97.627	101507
58016	-40.85	14.40	107.078	101486
57617	-39.95	15.30	26.1625	101573
57616	-39.95	14.40	51.4303	101571

Msg #5, step=24, shortName=msl
Idx	lat	lon	dist	val
58017	-40.85	15.30	97.627	101379
58016	-40.85	14.40	107.078	101372
57617	-39.95	15.30	26.1625	101494
57616	-39.95	14.40	51.4303	101471

EOF

diff $tempRef $tempTxt


rm -f $tempTxt $tempRef
