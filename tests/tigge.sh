#!/bin/sh
# Copyright 2005-2013 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

REDIRECT=/dev/null

#set -eax 

dir="${data_dir}/tigge/"

# check tigge global
for file in `ls ${dir}/tigge_*.grib`
do
	${tigge_dir}tigge_check ${file} 2> $REDIRECT > $REDIRECT
done

# check tigge-lam
for file in `ls ${dir}/tiggelam_*.grib`
do
	${tigge_dir}tigge_check -l ${file} 2> $REDIRECT > $REDIRECT
done


# Now test non-TIGGE files too. We now expect tigge_check to fail!
set +e
# All the grib files in the samples are non-TIGGE
for file in `ls ${GRIB_SAMPLES_PATH}/*.tmpl`
do
	${tigge_dir}tigge_check ${file} 2> $REDIRECT > $REDIRECT
	if [ $? -eq 0 ]; then
		# should have failed and returned a non-zero exit code
		exit 1
	fi
done

