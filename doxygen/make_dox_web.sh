#!/bin/sh
set -e
./prepare_pydocs.pl < ../python/gribapi.py > gribapi.py

cd ../tools
./make_dox.ksh
cd ../doxygen

rm -f ../html_web/*

DOXYGEN_EXE=doxygen
# DOXYGEN_EXE=/tmp/doxygen-1.5.3/bin/doxygen
$DOXYGEN_EXE grib_api_site.cfg

cp doxygen.css ../html_web/
./build_gribex.ksh gribex ../html_web/gribexkeys
./make_site.ksh
