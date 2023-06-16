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

label="grib_spectral_test"

output=output.$label.grib
input_complex=$ECCODES_SAMPLES_PATH/sh_ml_grib2.tmpl

$EXEC ${test_dir}/grib_spectral $input_complex $output

rm -f $output

# Now try spectral simple
input_simple=$label.simple.grib
${tools_dir}/grib_set  -rs packingType=spectral_simple $input_complex $input_simple
$EXEC ${test_dir}/grib_spectral $input_simple $output

rm -f $output $input_simple
