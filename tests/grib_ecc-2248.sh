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

# ---------------------------------------------------------

#if [ ! -d "$ECCODES_DEFINITION_PATH" ]; then
#    echo "Test $0 disabled. No definitions directory"
#    exit 0
#fi

# find all optical templates, exclude lines with #
no_optical=$(grep -lE '^[[:space:]]*[^#].*template.4.optical.def' ${def_dir}/grib2/templates/template.4.[0-9]*.def | wc -l)

# check how many of the optical templates include mars.wavelength.def, exclude lines with #
no_marswavelength=$(grep -lE '^[[:space:]]*[^#].*mars.wavelength.def' $(grep -l template.4.optical.def ${def_dir}/grib2/templates/template.4.[0-9]*.def) | wc -l)

# print the ones which doesn't
grep -LE '^[[:space:]]*[^#].*mars.wavelength.def' $(grep -lE '^[[:space:]]*[^#].*template.4.optical.def' ${def_dir}/grib2/templates/template.4.[0-9]*.def)

# if not all optical contain mars.wavelenght -> error
if [ $no_optical -ne $no_marswavelength ] ; then
 echo "Not all optical templates include mars.wavelength.def"
 exit 1
fi
