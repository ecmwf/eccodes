#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.ctest.sh

# Element access to "values" with an index beyond the coded values must fail: second order packing
# (GRIB2) and boustrophedonic ordering (GRIB1)
${test_dir}/grib_element_index_codedvalues ${data_dir}/gfs.complex.mvmu.grib2
${test_dir}/grib_element_index_codedvalues ${data_dir}/boustrophedonic.grib1
