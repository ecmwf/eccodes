#!/bin/sh

cd ../tools
./make_dox.ksh
cd ../doxygen

rm -f ../html/*
touch ../html/Makefile.am
doxygen grib_api_wiz.cfg
cp doxygen.css ../html/
