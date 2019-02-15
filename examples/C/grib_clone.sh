#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

TEMP=cloned.grib

INPUT=${data_dir}/reduced_gaussian_pressure_level.grib1
${examples_dir}/c_grib_clone ${INPUT} ${TEMP}  > /dev/null
${tools_dir}/grib_compare ${INPUT} ${TEMP}

INPUT=${data_dir}/sample.grib2
${examples_dir}/c_grib_clone ${INPUT} ${TEMP}  > /dev/null
${tools_dir}/grib_compare ${INPUT} ${TEMP}

rm -f ${TEMP}
