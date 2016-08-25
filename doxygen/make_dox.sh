#!/bin/sh

# No need to do tools. We use another mechanism. See top-level
# confluence directory
#cd ../tools
#./make_dox.ksh
#cd ../doxygen

set -e

./process_C_header.pl ../src/grib_api.h ../src/eccodes.h > eccodes.h

# Python doxygen module will be "ecCodes" to distinguish it from "eccodes" for F90
./process_python.pl ../python/gribapi.py > ecCodes.py

rm -fr ../html/*
touch ../html/Makefile.am
doxygen grib_api_wiz.cfg

# Do not copy this. Use default generated doxygen.css
# cp doxygen.css ../html/

# Remove temp files
rm -f eccodes.h
rm -f ecCodes.py

# Change the links from any example code back to confluence
fnames="
bufr_get_keys
grib_index
grib_get_keys
grib_set_keys
grib_get_data
grib_clone
grib_print_data
grib_copy_message
count_messages
grib_nearest
grib_samples
grib_set_missing
"

for fn in $fnames; do
  perl -p -i -e "s|$fn\.f90|<a href=\"https://software.ecmwf.int/wiki/display/ECC/$fn\" target=\"_blank\">$fn.f90</a>|" ../html/classeccodes.html
  # Do all F90 interface files too
  perl -p -i -e "s|$fn\.f90|<a href=\"https://software.ecmwf.int/wiki/display/ECC/$fn\" target=\"_blank\">$fn.f90</a>|" ../html/interfaceeccodes*.html
done

pnames="
grib_get_keys
grib_set_keys
grib_samples
grib_set_missing
grib_nearest
grib_index
grib_print_data
grib_multi_write
grib_iterator
grib_clone
grib_keys_iterator
bufr_get_keys
"
for fn in $pnames; do
  perl -p -i -e "s|$fn\.py|<a href=\"https://software.ecmwf.int/wiki/display/ECC/$fn\" target=\"_blank\">$fn.py</a>|" ../html/namespaceec_codes.html
done

echo DONE
