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

# ---------------------------------------------------------
# This is the test for JIRA issue ECC-XXXX
# < Add issue summary here >
# ---------------------------------------------------------

REDIRECT=/dev/null

label=`basename $0 | sed -e 's/\.sh/_test/'`

tempGrib=temp.$label.grib
tempFilt=temp.$label.filt

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

#####################
# test eest type pd #
#####################
cat >$tempFilt<<EOF
set productDefinitionTemplateNumber=112;
set tablesVersion=37;
set setLocalDefinition=1;
set class="od";
set stream="eest";
set type="pd";
set probabilityType=10;
set scaleFactorOfLowerLimit = 0;
set scaledValueOfLowerLimit = 1;
set scaleFactorOfUpperLimit = 0;
set scaledValueOfUpperLimit = 3;
set typeOfStatisticalProcessing=0;
set lengthOfTimeRange=168;
set paramId=131167;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2

result=$(${tools_dir}/grib_get -p mars.quantile $tempGrib)
[ "$result" = "1:3" ]

result=$(${tools_dir}/grib_get -p mars.stattype $tempGrib)
[ "$result" = "7dav" ]

result=$(${tools_dir}/grib_get -p mars.timespan $tempGrib)
[ "$result" = "none" ]

#####################
# test eest type fc #
#####################
cat >$tempFilt<<EOF
set productDefinitionTemplateNumber=11;
set tablesVersion=37;
set setLocalDefinition=1;
set class="od";
set stream="eest";
set type="fc";
set perturbationNumber=1;
set typeOfStatisticalProcessing=0;
set lengthOfTimeRange=168;
set paramId=171167;
write;
EOF
${tools_dir}/grib_filter -o $tempGrib $tempFilt $sample_grib2

result=$(${tools_dir}/grib_get -p mars.number $tempGrib)
[ "$result" = "1" ]

result=$(${tools_dir}/grib_get -p mars.stattype $tempGrib)
[ "$result" = "7dav" ]

result=$(${tools_dir}/grib_get -p mars.timespan $tempGrib)
[ "$result" = "none" ]


# Clean up
#rm -f $tempGrib $tempFilt
