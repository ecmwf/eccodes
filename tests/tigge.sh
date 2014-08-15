#!/bin/sh
# Copyright 2005-2014 ECMWF.
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
for file in ${dir}/tigge_*.grib
do
   ${tigge_dir}tigge_check ${file} 2> $REDIRECT > $REDIRECT
done

# check tigge-lam
for file in ${dir}/tiggelam_*.grib
do
   ${tigge_dir}tigge_check -l ${file} 2> $REDIRECT > $REDIRECT
done


# Now test non-TIGGE files too. We now expect tigge_check to fail!
# All the grib files in the samples are non-TIGGE
for file in ${GRIB_SAMPLES_PATH}/*.tmpl
do
   set +e
   ${tigge_dir}tigge_check ${file} 2> $REDIRECT > $REDIRECT
   status=$?
   set -e
   if [ $status -eq 0 ]; then
      # should have failed and returned a non-zero exit code
      exit 1
   fi
done

# GRIB-531
TEMP=temp.$$.tigge
${tools_dir}grib_get -nparameter ${data_dir}/tigge_pf_ecmwf.grib2 > $TEMP
diff ${data_dir}/tigge_pf_ecmwf.grib2.ref $TEMP

# GRIB-205. Changing productionStatusOfProcessedData should not change
# anything else
input=${dir}/tigge_ecmf_sfc_sd.grib
${tools_dir}grib_set -s productionStatusOfProcessedData=5 $input $TEMP
${tools_dir}grib_compare -bproductionStatusOfProcessedData $input $TEMP

rm -f $TEMP
