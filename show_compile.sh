#!/bin/sh

# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

if [ $# -ne 1 ]; then
    echo "Usage: `basename $0` /path/to/ecCodes/installation"
    exit 1
fi

set -e
PACKAGE=eccodes_f90
ECCODES_INSTALL_PATH=$1
ECCODES_PKGCONF_PATH=$ECCODES_INSTALL_PATH/lib/pkgconfig

if [ ! -d "$ECCODES_INSTALL_PATH" ]; then
   echo "Error: Installation path does not exist!" 2>&1
   exit 1
fi
if [ ! -d "$ECCODES_PKGCONF_PATH" ]; then
   echo "Error: Could not find pkgconfig directory in installation path!" 2>&1
   echo "       Expected path: $ECCODES_PKGCONF_PATH" 2>&1
   exit 1
fi

if ! command -v pkg-config >/dev/null 2>&1; then
   echo "Error: cannot find pkg-config command!" 2>&1
   exit 1
fi

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$ECCODES_PKGCONF_PATH
LIBS=`pkg-config $PACKAGE --libs`
INCL=`pkg-config $PACKAGE --cflags`
FCMP=`pkg-config $PACKAGE --variable=FC`
CCMP=`pkg-config $PACKAGE --variable=CC`

echo
echo "Compilation for Fortran 90 ..."
echo "$FCMP myprog.f90 $LIBS $INCL"
echo
echo "Compilation for C ..."
echo "$CCMP myprog.c $LIBS $INCL"
echo
