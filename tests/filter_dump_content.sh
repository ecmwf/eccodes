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

label="filter_dump_content_test"
temp=temp.$label.txt
grib_sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
bufr_sample=$ECCODES_SAMPLES_PATH/BUFR4.tmpl

${tools_dir}/grib_filter - $grib_sample <<EOF
    print "Dump WMO Start....";
    transient dumpit = dump_content("wmo");
    print "Dump WMO End....";
EOF

${tools_dir}/grib_filter - $grib_sample <<EOF
    print "Dump DEBUG Start....";
    transient dumpit = dump_content("debug");
    print "Dump DEBUG End....";
EOF

${tools_dir}/bufr_filter - $bufr_sample <<EOF
    print "BUFR Dump DEFAULT Start....";
    transient dumpit = dump_content("default");
    print "BUFR Dump DEFAULT End....";
EOF

${tools_dir}/bufr_filter - $bufr_sample <<EOF
    print "BUFR Dump Start....";
    set unpack = 1;
    transient dumpit = dump_content("bufr_simple");
    print "BUFR Dump End....";
EOF


# Illegal cases
# ---------------
${tools_dir}/grib_filter - $grib_sample 2>$temp <<EOF
    transient dumpit = dump_content();
EOF
grep -q "Please provide an argument" $temp


${tools_dir}/grib_filter - $grib_sample 2>$temp <<EOF
    transient dumpit = dump_content("nosuch");
EOF
cat $temp
grep -q "Unknown type.*for dumper" $temp


# Clean up
rm -f $temp
