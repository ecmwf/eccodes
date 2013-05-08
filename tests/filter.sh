#!/bin/sh
# Copyright 2005-2013 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

REDIRECT=/dev/null

if [ -f ${data_dir}/geavg.t12z.pgrbaf108 ]
then
	tmpdata=grib_api.grib
	rm -f $tmpdata || true
	${tools_dir}grib_filter ${data_dir}/filter_rules ${data_dir}/geavg.t12z.pgrbaf108 > $REDIRECT
	rm -f $tmpdata
fi

cat >${data_dir}/f.rules <<EOF
write "${data_dir}/split/[centre]_[date]_[dataType]_[gridType]_[levelType]_[level]_[short_name]_[packingType].grib[editionNumber]";
EOF

[ -d ${data_dir}/split ] || mkdir -p ${data_dir}/split 

${tools_dir}grib_filter ${data_dir}/f.rules ${data_dir}/tigge_pf_ecmwf.grib2 2> $REDIRECT > $REDIRECT

rm -f ${data_dir}/split/*

# Test with nonexistent keys. Note spelling of centre!
#######################################################
cat >${data_dir}/nonexkey.rules <<EOF
set center="john";
EOF
# Invoke without -f i.e. should fail if error encountered
set +e
${tools_dir}grib_filter ${data_dir}/nonexkey.rules ${data_dir}/tigge_pf_ecmwf.grib2 2> $REDIRECT > $REDIRECT
if [ $? -eq 0 ]; then
   echo "grib_filter should have failed if key not found" >&2
   exit 1
fi
set -e
# Now repeat with -f option (do not exit on error)
${tools_dir}grib_filter -f ${data_dir}/nonexkey.rules ${data_dir}/tigge_pf_ecmwf.grib2 2> $REDIRECT > $REDIRECT

rm -f ${data_dir}/nonexkey.rules

# GRIB-308 format specifier for integer keys
##############################################
cat > ${data_dir}/formatint.rules <<EOF
# Pad edition with leading zeroes and level with blanks
print "edition=[edition%.3d], level=[level%5ld]";
EOF

result=`${tools_dir}/grib_filter  ${data_dir}/formatint.rules  $GRIB_SAMPLES_PATH/GRIB1.tmpl`
[ "$result" = "edition=001, level=  500" ]

# Convert from grib1 to grib2 "Generalized vertical height coordinates"
cat >temp.filt <<EOF
set edition=2;
set PVPresent=1;
set NV=6;
set typeOfFirstFixedSurface=150;
set nlev=41.0;
write;
EOF

${tools_dir}/grib_filter -o temp.grib2 temp.filt $GRIB_SAMPLES_PATH/GRIB1.tmpl
result=`${tools_dir}/grib_get -p typeOfFirstFixedSurface,NV,nlev temp.grib2`
[ "$result" = "150 6 41" ]

rm -f temp.grib2 temp.filt
rm -f ${data_dir}/formatint.rules

