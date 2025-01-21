#!/bin/bash

mkdir /tmp/eccodes/auditwheel
auditwheel repair -w /tmp/eccodes/auditwheel /tmp/eccodes/build/wheel/*whl
rm /tmp/eccodes/build/wheel/*whl
mv /tmp/eccodes/auditwheel/*whl /tmp/eccodes/build/wheel/
