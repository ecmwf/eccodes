#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

# -----------------------------------------------------------------
# This is the test for the JIRA issue ECC-1927
# grid_second_order hangs when encoding fields with infinite values
# -----------------------------------------------------------------

. ./include.ctest.sh

for encoding in grid_simple grid_second_order; do
  for infinity in 0 1; do

    if [ $infinity -eq 1 ]; then
      set +e
    fi

    ${examples_dir}/f_grib_infinity_grid_second_order $encoding $infinity
    c=$?

    if [ $infinity -eq 1 ]; then
      set -e
      if [ $c -eq 0 ]; then
        echo "Encoding infinite numbers should have failed"
        exit 1
      fi
    fi

  done
done
