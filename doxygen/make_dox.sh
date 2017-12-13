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
doxygen --version
doxygen grib_api_wiz.cfg

# Do not copy this. Use default generated doxygen.css
# cp doxygen.css ../html/

# Change the links from example code back to confluence
./convert_links.sh

# Note:
#  May have to edit html/interfaceeccodes_1_1codes__set.html
#  to remove the weird */ inserted for subroutine 'codes_set_string'

# Remove temp files
rm -f eccodes.h
rm -f ecCodes.py
echo DONE
