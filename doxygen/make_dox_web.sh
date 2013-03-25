#!/bin/sh

./prepare_pydocs.pl < ../python/eccode.py > eccode.py

cd ../tools
./make_dox.ksh
cd ../doxygen
p4 edit ../html_web/*
rm -f ../html_web/*
doxygen grib_api_site.cfg
cp doxygen.css ../html_web/
./build_gribex.ksh gribex ../html_web/gribexkeys
./make_site.ksh
