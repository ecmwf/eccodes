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

label="codes_to_double_test"
tempFilt=temp.$label.filt
tempLog=temp.$label.log
tempRef=temp.$label.ref

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

cat >$tempFilt<<EOF
 transient aa = 156;
 meta aad to_double(aa,0,3,0);
 print "aa=[aad]";

 transient xx = "45.5e3";
 meta xxd to_double(xx,0,6,0);
 print "xx=[xxd:d]";

 transient yy = "45.9";
 meta yyd to_double(yy,0,2,0);
 print "yy=[yyd:s]";
EOF
${tools_dir}/grib_filter $tempFilt $sample_grib2 > $tempLog

cat >$tempRef<<EOF
aa=156
xx=45500
yy=45
EOF
diff $tempRef $tempLog


# Clean up
rm -f $tempFilt $tempLog $tempRef
