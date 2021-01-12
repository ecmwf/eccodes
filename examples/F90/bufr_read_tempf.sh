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

#Define a common label for all the tmp files
label="bufr_read_tempf_f"
tempOut=temp.${label}.txt
tempRef=temp.${label}.ref
rm -f $tempRef $tempOut

# The path to the BUFR file is hard coded in the example
${examples_dir}/eccodes_f_bufr_read_tempf > $tempOut

# Check the results
cat > $tempRef<<EOF
Statid: MISSING
Ob:       1 16 245 20151202 110419   41.670    12.450   35.0   36.0 114   64
WMO list lat, lon, ht:    41.670    12.450    0.0
Radiosonde number/software: J2063451        MW41 2.3.0
level  dtime   dlat   dlon pressure geopotH airTemp  dewPtT windDir  windSp  signif
    2   44.0 -0.000  0.001 100000.0   243.0  286.16    5.60  147.00    0.20   65536
    3  169.0 -0.001  0.000  92500.0   892.0  280.94    2.87   68.00    3.00   65536 tjump
    7  316.0 -0.000 -0.002  85000.0  1593.0  283.60   14.37   60.00    4.00   65536
   14  630.0 -0.003  0.003  70000.0  3187.0  274.99   30.49  296.00    5.40   65536
   24 1117.0 -0.005  0.018  50000.0  5828.0  258.61   11.68  310.00    3.30   65536 tjump
   29 1411.0 -0.011  0.030  40000.0  7482.0  247.70   27.79  273.00    2.20   65536
   32 1761.0 -0.004  0.033  30000.0  9493.0  230.33   13.63  157.00    3.30   65536
   37 1967.0  0.003  0.028  25000.0 10696.0  220.52   16.47  157.00    5.20   65536
   42 2234.0  0.016  0.017  20000.0 12098.0  208.45    9.01  119.00    8.40   65536
   53 2578.0  0.015  0.018  15000.0 13835.0  207.93   17.32  316.00    8.70   65536
   64 3087.0 -0.021  0.036  10000.0 16310.0  208.29   24.46  340.00    9.60   79872
EOF

#diff -w $tempRef $tempOut


# Clean up
rm -f $tempRef $tempOut
