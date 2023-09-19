#!/bin/sh

TEST_DIR=`dirname $0`

if [ $# -ne 1 ]; then
    echo "Usage: $0 title > $TEST_DIR/title.sh"  1>&2
    echo  1>&2
    echo "Example:"  1>&2
    echo "  $0 ECC-6666 > tests/grib_ecc-6666.sh"  1>&2
    echo "  $0 ECC-9999 > tests/bufr_ecc-9999.sh"  1>&2
    echo  1>&2
    exit 1
fi

TEST_TITLE="$1"
TEST_TITLE=$(echo $TEST_TITLE | tr '[A-Z]' '[a-z]')

# Check label is unique
grep -i -q "label=\"$TEST_TITLE\"" $TEST_DIR/*.sh
if [ $? -eq 0 ]; then
  echo "ERROR: Test label \"$TEST_TITLE\" seems to already exist!"
  grep -l "label=\"$TEST_TITLE\"" $TEST_DIR/*.sh # Show results
  exit 1
fi

cat <<EOF
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

REDIRECT=/dev/null

label="prod_${TEST_TITLE}_test"  # Change prod to bufr or grib etc
tempGrib=temp.\$label.grib
tempBufr=temp.\$label.bufr
tempFilt=temp.\$label.filt
tempLog=temp.\$label.log
tempRef=temp.\$label.ref

sample_grib2=\$ECCODES_SAMPLES_PATH/GRIB2.tmpl
sample_bufr4=\$ECCODES_SAMPLES_PATH/BUFR4.tmpl

#...
#infile=\${data_dir}/SOME_FILE
#cat >\$tempFilt<<EOF
#EOF
#\${tools_dir}/grib_filter -o \$tempGrib \$tempFilt \$sample_grib2
#\${tools_dir}/grib_get
#\${tools_dir}/grib_set
#grib_check_key_equals \$temp k1,k2 "v1 v2"

#\${tools_dir}/bufr_get
#\${tools_dir}/bufr_set
#...

rm -f \$tempGrib \$tempBufr \$tempFilt \$tempLog \$tempRef
EOF

echo >&2
echo "Remember to chmod 755 the new test" >&2
echo >&2
