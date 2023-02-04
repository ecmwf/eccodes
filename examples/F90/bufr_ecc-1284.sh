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

label="bufr_ecc-1284_test_f"
TEMP_DUMP=temp.$label.dump
TEMP_TEXT=temp.$label.txt
TEMP_REF=temp.$label.ref
TEMP_BUFR=temp.$label.bufr

# Create a BUFR which has some missing entries in the string array stationOrSiteName
${examples_dir}/eccodes_f_bufr_ecc-1284 $TEMP_BUFR

# --------------
# Plain dump
# --------------
${tools_dir}/bufr_dump -p $TEMP_BUFR > $TEMP_DUMP
# From the dump output, grab 10 lines after stationOrSiteName
awk '/stationOrSiteName/{x=NR+10}(NR<=x){print}' $TEMP_DUMP > $TEMP_TEXT

cat > $TEMP_REF << EOF
stationOrSiteName={    "ARD2-LPTR",
    "ETH2-LPTR",
    MISSING,
    "KALT-LPTR",
    "KALT-LPTR",
    "FHBB-LPTR",
    "FHBB-LPTR",
    "LIND-LPTR",
    "LIND-LPTR",
    MISSING
}
EOF

diff $TEMP_REF $TEMP_TEXT

# --------------
# JSON dump
# --------------
${tools_dir}/bufr_dump $TEMP_BUFR > $TEMP_DUMP
awk '/stationOrSiteName/{x=NR+12}(NR<=x){print}' $TEMP_DUMP > $TEMP_TEXT

cat > $TEMP_REF << EOF
          "key" : "stationOrSiteName",
          "value" : 
          [            "ARD2-LPTR",
            "ETH2-LPTR",
            null,
            "KALT-LPTR",
            "KALT-LPTR",
            "FHBB-LPTR",
            "FHBB-LPTR",
            "LIND-LPTR",
            "LIND-LPTR",
            null
          ],
EOF
diff $TEMP_REF $TEMP_TEXT

# Clean up
rm -f $TEMP_BUFR $TEMP_DUMP $TEMP_TEXT $TEMP_REF
