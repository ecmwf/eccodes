#!/bin/sh

set -e

# Change the links from example code back to confluence
# This is only done the first time before checking in the new html files
# Do not run this on html files which are already converted!
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
# Adding target="_blank" ensures link opens in a new tab/window in browser
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

