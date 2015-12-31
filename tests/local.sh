#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
#set -x

REDIRECT=/dev/null

cd ${data_dir}
rm -f local.log

${tools_dir}grib_set -s edition=2,setLocalDefinition=1 reduced_gaussian_model_level.grib1 loc.grib2
${tools_dir}grib_set -s setLocalDefinition=1           reduced_gaussian_model_level.grib1 loc.grib1

# conversion 1->2
for localDefinitionNumber in 1 15 26 30
do
	${tools_dir}grib_set -s localDefinitionNumber=$localDefinitionNumber,perturbationNumber=2,numberOfForecastsInEnsemble=50 loc.grib1 eps.grib1
	${tools_dir}grib_set -s edition=2 eps.grib1 eps.grib2

	${tools_dir}grib_get -p localDefinitionNumber,perturbationNumber,numberOfForecastsInEnsemble,productDefinitionTemplateNumber eps.grib2 >> local.log
	${tools_dir}grib_compare -e -b param eps.grib1 eps.grib2

	${tools_dir}grib_set -s localDefinitionNumber=$localDefinitionNumber,numberOfForecastsInEnsemble=0 loc.grib1 eps.grib1
	${tools_dir}grib_set -s edition=2 eps.grib1 eps.grib2

	${tools_dir}grib_get -f -p localDefinitionNumber,perturbationNumber,productDefinitionTemplateNumber eps.grib2 >> local.log
	${tools_dir}grib_compare -e -b param eps.grib1 eps.grib2
done

#local -> local
for localStart in 1 7 9 20 25 26 30
do
	${tools_dir}grib_set -s localDefinitionNumber=$localStart loc.grib1 loc1.grib1
	${tools_dir}grib_set -s edition=2 loc1.grib1 loc1.grib2
	${tools_dir}grib_get -p localDefinitionNumber loc1.grib1 >> local.log
	${tools_dir}grib_get -p localDefinitionNumber loc1.grib2 >> local.log
	
	for localEps in 1 15 26 30
	do
		${tools_dir}grib_set -s localDefinitionNumber=$localEps,eps=1,perturbationNumber=2,numberOfForecastsInEnsemble=50 loc1.grib2 eps.grib2
		${tools_dir}grib_get -p localDefinitionNumber,perturbationNumber,numberOfForecastsInEnsemble,productDefinitionTemplateNumber eps.grib2 >> local.log
	done
done

#special types/streams
${tools_dir}grib_set -s localDefinitionNumber=1,numberOfForecastsInEnsemble=0 loc.grib1 loc1.grib1
${tools_dir}grib_set -s edition=2 loc1.grib1 loc1.grib2
${tools_dir}grib_get -f -p localDefinitionNumber,perturbationNumber loc1.grib2 >> local.log
${tools_dir}grib_set -s type=em loc1.grib2 eps.grib2
${tools_dir}grib_get -p localDefinitionNumber,productDefinitionTemplateNumber,derivedForecast eps.grib2 >> local.log
${tools_dir}grib_set -s type=es loc1.grib2 eps.grib2
${tools_dir}grib_get -p localDefinitionNumber,productDefinitionTemplateNumber,derivedForecast eps.grib2 >> local.log
${tools_dir}grib_set -s stream=enda loc1.grib2 eps.grib2 #2> $REDIRECT
${tools_dir}grib_get -p localDefinitionNumber,productDefinitionTemplateNumber eps.grib2 >> local.log

diff local.log local.good.log

rm -f local.log loc.grib1 loc.grib2 loc1.grib1 loc1.grib2 eps.grib1 eps.grib2

