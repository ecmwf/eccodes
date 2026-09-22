#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

# Tests for TAF (Terminal Aerodrome Forecast)

. ./include.ctest.sh

label="taf_test"
tempTaf=temp.$label.taf
tempOut=temp.$label.out
tempTxt=temp.$label.txt
tempRef=temp.$label.ref

cat >$tempTaf<<EOF
TAF 
KXYZ 051730Z 0518/0624 31008KT 3SM -SHRA BKN020
FM052300 30006KT 5SM -SHRA OVC030
PROB30 0604/0606 VRB20G35KT 1SM TSRA BKN015CB
FM060600 25010KT 4SM -SHRA OVC050 TEMPO 0608/0611 2SM -SHRA OVC030
RMK NXT FCST BY 00Z=
EOF

${tools_dir}/grib_dump -TF -O $tempTaf
id=`${tools_dir}/grib_get -TF -p identifier $tempTaf`
[ "$id" = "TAF" ]

# Clean up
rm -f $tempTaf $tempOut $tempRef $tempTxt
