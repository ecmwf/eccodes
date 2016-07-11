#!/bin/sh

# No need to do tools. We use another mechanism. See top-level
# confluence directory
#cd ../tools
#./make_dox.ksh
#cd ../doxygen

set -e

./process_C_header.pl ../src/grib_api.h ../src/eccodes.h > eccodes.h

rm -fr ../html/*
touch ../html/Makefile.am
doxygen grib_api_wiz.cfg

# Do not copy this. Use default generated doxygen.css
# cp doxygen.css ../html/

# Remove temp files
rm -f eccodes.h
