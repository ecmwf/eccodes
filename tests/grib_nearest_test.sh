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

label="grib_nearest_test"
temp=$label.temp
tempRef=$label.ref
input_grb=${data_dir}/reduced_gaussian_pressure_level.grib1

# Nearest with decoding the data values
# --------------------------------------
$EXEC ${test_dir}/grib_nearest_test $input_grb > $temp
cat > $tempRef <<EOF
ordering by param,step
1 fields in the fieldset
n,step,param
1 0 t  
Idx	lat	lon	dist	val
4839	-40.46	18.00	259.679	285.527
4838	-40.46	15.00	51.5268	284.074
4719	-37.67	18.00	366.445	289.621
4718	-37.67	15.00	258.597	289.027

EOF
diff $tempRef $temp

# Nearest without decoding the data values
# ----------------------------------------
$EXEC ${test_dir}/grib_nearest_test -n $input_grb > $temp
cat > $tempRef <<EOF
ordering by param,step
1 fields in the fieldset
n,step,param
1 0 t  
Idx	lat	lon	dist
4839	-40.46	18.00	259.679
4838	-40.46	15.00	51.5268
4719	-37.67	18.00	366.445
4718	-37.67	15.00	258.597

EOF
diff $tempRef $temp

# Clean up
rm -f $temp $tempRef
