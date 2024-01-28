#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.ctest.sh

FROM_FILE=$ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl
TO_FILE=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

${examples_dir}/c_grib_copy_keys $FROM_FILE $TO_FILE
