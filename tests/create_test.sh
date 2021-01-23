#!/bin/sh

TEST_DIR=`dirname $0`

if [ $# -ne 1 ]; then
    echo "Usage: $0 title > $TEST_DIR/title.sh"
    exit 1
fi

TEST_TITLE="$1"

# Check label is unique
grep -q "label=\"$TEST_TITLE\"" $TEST_DIR/*.sh
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

. ./include.sh
set -u
REDIRECT=/dev/null
label="$TEST_TITLE"
temp=temp.\$label
sample_grib1=\$ECCODES_SAMPLES_PATH/GRIB1.tmpl
sample_grib2=\$ECCODES_SAMPLES_PATH/GRIB2.tmpl
sample_bufr3=\$ECCODES_SAMPLES_PATH/BUFR3.tmpl
sample_bufr4=\$ECCODES_SAMPLES_PATH/BUFR4.tmpl

#...
#infile=\${data_dir}/SOME_FILE
#\${tools_dir}/grib_get
#\${tools_dir}/grib_set
#grib_check_key_equals \$temp k1,k2 "v1 v2"
#\${tools_dir}/bufr_get
#\${tools_dir}/bufr_set
#...

rm -f \$temp
EOF

echo >&2
echo "Remember to chmod 755 the new test" >&2
echo >&2
