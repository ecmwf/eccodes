#! /bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
set -e

tmpfiles="fortran_interface.dat"

trap 'rm -f $tmpfiles' 1 2 3 15

export ECCODES_DEFINITION_PATH=`pwd`"/definitions"
path=../data
grib1=latlon.grib

[ ! -f "$path/$tmpfiles" ] && exit 1


cd tests


DECODE=readvalues
 
./${DECODE} -i  ${path}/${grib1} | head -600 > $tmpfiles

diff $tmpfiles $path/$tmpfiles

result=$?

exit $result

