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
# cm / cr encoding in GRIB2
# ---------------------------------------------------------

REDIRECT=/dev/null

label=`basename $0 | sed -e 's/\.sh/_test/'`

tempGrib=temp.$label.grib
tempFilt=temp.$label.filt

sample_grib2=$ECCODES_SAMPLES_PATH/GRIB2.tmpl

cat >$tempFilt<<EOF
set setLocalDefinition=1;
set tablesVersion=37;
set productDefinitionTemplateNumber = 3;
set grib2LocalSectionNumber = 32;
set dataDate = 20241022;
set dataTime = 0000;
set paramId = 129;
set forecastTime = 72;
set typeOfFirstFixedSurface = 100;
set scaleFactorOfFirstFixedSurface = 0;
set scaledValueOfFirstFixedSurface = 50000;
set scaleFactorOfStandardDeviation=missing();
set scaledValueOfStandardDeviation=missing();
set scaleFactorOfDistanceFromEnsembleMean=missing();
set scaledValueOfDistanceFromEnsembleMean=missing();
set derivedForecast = 6;
set numberOfForecastsInEnsemble = 51;
set clusterIdentifier = 1;
set NH = missing();
set NL = 1;
set totalNumberOfClusters = 4;
set clusteringMethod = 1;
set northernLatitudeOfClusterDomain = 75000000;
set southernLatitudeOfClusterDomain = 30000000;
set easternLongitudeOfClusterDomain = 40000000;
set westernLongitudeOfClusterDomain = 340000000;
set numberOfForecastsInTheCluster = 21;
set ensembleForecastNumbers = {0,2,5,9,12,18,20,23,25,26,27,28,29,33,35,36,37,38,45,46,50};
set marsClass = 1;
set marsType = 14;
set marsStream = 1035;
set experimentVersionNumber = 0001;
set clusteringStartTimeStep = 72;
set clusteringEndTimeStep = 96;
set representativeMember = 5;
set climatologicalRegime = 2;
write;
EOF

result=$(${tools_dir}/grib_get -p mars.number $tempGrib)
[ "$result" = "1" ]

result=$(${tools_dir}/grib_get -p mars.domain $tempGrib)
[ "$result" = "h" ]

result=$(${tools_dir}/grib_get -p mars.step $tempGrib)
[ "$result" = "72" ]

result=$(${tools_dir}/grib_get -p mars.levelist $tempGrib)
[ "$result" = "500" ]

# Clean up
rm -f $tempGrib $tempFilt
