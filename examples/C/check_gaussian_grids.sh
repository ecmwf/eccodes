#!/bin/sh
# Copyright 2005-2014 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

# Check all sample GRIBs with a Gaussian grid
samples_dir=${proj_dir}/samples
for gg in ${samples_dir}/reduced_gg_* ${samples_dir}/regular_gg_*; do
   ${examples_dir}check_gaussian_grid $gg >/dev/null
done
