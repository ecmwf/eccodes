#!/bin/sh

cd ../tools
./make_dox.ksh
cd ../doxygen
p4 edit ../html/*
rm -f ../html/*
touch ../html/Makefile.am
doxygen grib_api_wiz.cfg
cp doxygen.css ../html/
