#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

set -e

# create tarfiles.txt with the list of parameter files
./create_def.pl > /dev/null

# create the tarball with these parameter files
version=`grep parametersVersion parameters_version.def | awk 'BEGIN {FS="=";}{ sub(/ *;$/,"");print $2; }'`
cat tarfiles.txt | xargs tar zcf grib_api_parameters-v$version.tar.gz

# get the current html download page for GRIB API
dlpage=grib_api.html
rm -f $dlpage || true
cadaver http://wedit.ecmwf.int:81/products/data/software/download << EOF
get $dlpage
EOF

./inject_download_page.pl $dlpage > temp.html
mv temp.html $dlpage

# upload the updated download page
cadaver http://wedit.ecmwf.int:81/products/data/software/download << EOF
put $dlpage
cd software_files
put grib_api_parameters-v$version.tar.gz
EOF
