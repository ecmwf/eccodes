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

workdir=`pwd`
REDIRECT=/dev/null
label="grib_second_order_test"
tempText=temp.$label.txt

cd ${data_dir}

encoding=1
simple_no_bitmap=simple.grib
simple_bitmap=simple_bitmap.grib
tempFilt=temp.$label.filter

files_no_bitmap="gen_ext.grib \
gen_ext_boust.grib \
gen_ext_spd_2.grib \
gen_ext_spd_3.grib \
gen.grib row.grib "

files_bitmap="constant_width_bitmap.grib \
constant_width_boust_bitmap.grib \
gen_bitmap.grib \
gen_ext_bitmap.grib \
gen_ext_boust_bitmap.grib \
gen_ext_spd_2_bitmap.grib \
gen_ext_spd_2_boust_bitmap.grib \
gen_ext_spd_3_boust_bitmap.grib \
simple_bitmap.grib"

no_packing="gen.grib|row.grib|gen_bitmap.grib|constant_width_bitmap.grib|constant_width_boust_bitmap.grib"

test_data() {
    ${tools_dir}/grib_filter $tempFilt $simple > $simple.data

    for f in $files
    do 
        ${tools_dir}/grib_filter $tempFilt $f > $f.data
        diff $simple.data $f.data > /dev/null
        ${tools_dir}/grib_compare -cvalues $f $simple
        echo $f decoding test passed > $REDIRECT

        exclude=`echo $f | awk " /$no_packing/ {print \"found\";} "`
        if [ -z "$exclude" ] && [ $encoding != 0 ]
        then
            rm -f $f.copied
            ${tools_dir}/grib_copy -r $f $f.copied
            ${tools_dir}/grib_filter $tempFilt $f.copied > $f.copied.data
            diff $simple.data $f.copied.data > /dev/null
            ${tools_dir}/grib_compare -cvalues $f.copied $simple
            echo $f encoding test passed > $REDIRECT
            echo > $REDIRECT
        fi

    done
    rm -f *.data *.copied
}


cat > $tempFilt<<EOF
print "[values!1]!";
EOF

simple=$simple_no_bitmap
files=$files_no_bitmap
test_data

simple=$simple_bitmap
files=$files_bitmap
test_data

# Now make sure grib_dump works on a second-order row-by-row file
# See GRIB-147
${tools_dir}/grib_dump second_ord_rbr.grib1 > $REDIRECT

# Test nearest neighbour on second order with a bitmap
# GRIB-541
sec_ord_bmp=sec_ord_bmp.$$.grib1

# Convert to second order packing
${tools_dir}/grib_set -r -s packingType=grid_second_order gen_bitmap.grib $sec_ord_bmp
grib_check_key_equals $sec_ord_bmp accuracy 4
# Check there are missing values
nums=`${tools_dir}/grib_get -p numberOfDataPoints,numberOfCodedValues,numberOfMissing $sec_ord_bmp`
[ "$nums" = "5969 4 5965" ]

if [ $HAVE_GEOGRAPHY -eq 1 ]; then
    res=`${tools_dir}/grib_get -l 33,88.5 $sec_ord_bmp`
    [ "$res" = "9999 5.51552 9999 9999 " ]

    res=`${tools_dir}/grib_get -l 30,90.0 $sec_ord_bmp`
    [ "$res" = "5.26552 9999 9999 9999 " ]

    res=`${tools_dir}/grib_get -l 28.5,87 $sec_ord_bmp`
    [ "$res" = "9999 2.51552 9999 9999 " ]

    res=`${tools_dir}/grib_get -l 28.5,90 $sec_ord_bmp`
    [ "$res" = "3.51552 9999 5.26552 9999 " ]

    # GRIB-203 nearest on M-F second order boustrophedonic
    res=`${tools_dir}/grib_get -w count=1 -l 0,0,1 lfpw.grib1`
    [ "$res" = "20563.4  " ]
fi

# Unpack/pack test for second order grib1 data
# --------------------------------------------
g1files="lfpw.grib1
   gen_ext_spd_2.grib
   gen_ext_spd_3.grib"
temp1=temp1.$label.grib
temp_stat1=temp.$label.stat1
temp_stat2=temp.$label.stat2

for f1 in $g1files; do
    # This does unpack and repack
    ${tools_dir}/grib_copy -r $f1 $temp1
    ${tools_dir}/grib_get -n statistics $f1    > $temp_stat1
    ${tools_dir}/grib_get -n statistics $temp1 > $temp_stat2
    perl ${test_dir}/number_compare.pl $temp_stat1 $temp_stat2
done

# GRIB-883
# ------------
# Two coded values: Should stay as grid_simple
temp2=temp2.$label.grib
temp3=temp3.$label.grib
cat > $tempFilt<<EOF
 set values={ 2.1, 3.4 };
 write;
EOF
${tools_dir}/grib_filter -o $temp2 $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl
ECCODES_DEBUG=1 ${tools_dir}/grib_set -r -s packingType=grid_second_order $temp2 $temp3 2>$tempText
grib_check_key_equals $temp3 packingType,accuracy 'grid_simple 24'
grep -q "Packing not changed" $tempText

# Three coded values: Now we can change to 2nd order
cat > $tempFilt<<EOF
 set values={ 2.1, 3.4, 8.9 };
 write;
EOF
${tools_dir}/grib_filter -o $temp2 $tempFilt $ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -r -s packingType=grid_second_order $temp2 $temp3
grib_check_key_equals $temp3 packingType,accuracy 'grid_second_order 24'


# ECC-1219: packingType conversion from grid_ieee to grid_second_order
# ---------------------------------------------------------------------
ALLOWED_ERROR=3e-06
if [ $ECCODES_ON_WINDOWS -eq 0 ]; then
    infile=grid_ieee.grib
    ${tools_dir}/grib_set -r -s packingType=grid_second_order $infile $temp1
    grib_check_key_equals $temp1 packingType grid_second_order
    ${tools_dir}/grib_compare -cdata:n -R all=$ALLOWED_ERROR $infile $temp1
fi

# Second order doesn't have a proper representation for constant fields
# So we change the packing type to grid_simple
# ---------------------------------------------------------------------
infile=gen_ext_spd_2.grib
grib_check_key_equals $infile packingType grid_second_order
${tools_dir}/grib_set -d1 $infile $temp1
grib_check_key_equals $temp1 packingType grid_simple

infile=$ECCODES_SAMPLES_PATH/GRIB1.tmpl
${tools_dir}/grib_set -r -s packingType=grid_second_order $infile $temp1
grib_check_key_equals $temp1 packingType grid_simple

infile=$ECCODES_SAMPLES_PATH/GRIB2.tmpl
${tools_dir}/grib_set -r -s packingType=grid_second_order $infile $temp1
grib_check_key_equals $temp1 packingType grid_simple

export ECCODES_GRIBEX_BOUSTROPHEDONIC=1
${tools_dir}/grib_get -n statistics boustrophedonic.grib1
${tools_dir}/grib_set -s scaleValuesBy=1.1 boustrophedonic.grib1 $temp1
unset ECCODES_GRIBEX_BOUSTROPHEDONIC


# data_g1second_order_constant_width_packing
# ------------------------------------------
input=second_ord_rbr.grib1
${tools_dir}/grib_set -s Ni=2,Nj=74,secondOrderOfDifferentWidth=0,secondaryBitmapPresent=1 $input $temp1
grib_check_key_equals $temp1 packingType grid_second_order_constant_width
${tools_dir}/grib_dump -O $temp1 > $REDIRECT
set +e
${tools_dir}/grib_set -s scaleValuesBy=2 $temp1 $temp2 > $tempText 2>&1
status=$?
set -e
[ $status -ne 0 ]
grep -q "Not implemented" $tempText

if [ $HAVE_GEOGRAPHY -eq 1 ]; then
    ${tools_dir}/grib_get_data $temp1 > $REDIRECT
    ${tools_dir}/grib_ls -l46,1 $temp1 > $REDIRECT
    ${tools_dir}/grib_ls -j -l46,1,1 $temp1 > $REDIRECT
fi

# Encoding
input=second_ord_rbr.grib1
${tools_dir}/grib_set -s scaleValuesBy=1.01 $input $temp1
${tools_dir}/grib_dump $temp1
grib_check_key_equals $temp1 packingType grid_second_order

# ECC-1986: GRIB1: Setting field values in second order packing
cat > $tempFilt<<EOF
  if (count==1) {
    # min = 19074.9, max = 20717.6
    assert( min < 20000 );
    set offsetValuesBy = 1000.0;
    assert( min > 20000 );
  }
EOF
input=lfpw.grib1
${tools_dir}/grib_filter $tempFilt $input

# Local entries in table 5.0
# ---------------------------
latestOfficial=$( ${tools_dir}/grib_get -p tablesVersionLatestOfficial $ECCODES_SAMPLES_PATH/GRIB2.tmpl )
for drtn in 50001 50002; do
    ${tools_dir}/grib_set -r -s tablesVersion=$latestOfficial,dataRepresentationTemplateNumber=$drtn sample.grib2 $temp1
    ${tools_dir}/grib_dump -O -p section_5 $temp1 > $temp2
    grep -q "dataRepresentationTemplateNumber = $drtn .*Second order packing.*/5.0.table" $temp2
done

# ${tools_dir}/grib_set -r -s tablesVersion=$latestOfficial,dataRepresentationTemplateNumber=50001 sample.grib2 $temp1
# ${tools_dir}/grib_dump -O -p section_5 $temp1 > $temp2
# grep -q "dataRepresentationTemplateNumber = 50001 .*Second order packing.*/5.0.table" $temp2

# Clean up
rm -f $temp_stat1 $temp_stat2
rm -f $temp1 $temp2 $temp3 $sec_ord_bmp
rm -f $tempFilt $tempText
