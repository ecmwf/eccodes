#!/bin/bash

set -e

## This script executes different perl script to create the eccodes definition files
## There are three sets of eccodes definitions files for GRIB2

# create GRIB1 defs
./create_defs.grib1.pl

# create defs for is_mtg2_switch_0 (table grib_encoding)
./create_defs.grib2.lte33.pl

# create defs for is_mtg2_switch_1 (table grib_encoding)
./create_defs.grib2.mtg2.pl

# create defs for is_mtg2_switch_2 (table grib_encoding)
./create_defs.grib2.chemsplit.pl
