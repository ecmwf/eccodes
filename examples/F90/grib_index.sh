#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

# if an index file does not exist then
# create index and save to disk
${examples_dir}/eccodes_f_grib_index > index_f90.out
diff index_f90.out ${data_dir}/index_f90.ok

# if an index file exists then load the index from it
${examples_dir}/eccodes_f_grib_index > index_f90.out
diff index_f90.out ${data_dir}/index_f90.ok

rm -f index.idx index_f90.out
