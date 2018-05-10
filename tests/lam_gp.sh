#!/bin/sh
# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

${test_dir}/lam_gp

${tools_dir}/grib_dump -O lam_gp_lambert_lam.grib
${tools_dir}/grib_dump -O lam_gp_mercator_lam.grib
${tools_dir}/grib_dump -O lam_gp_polar_stereographic_lam.grib
