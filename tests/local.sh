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
#set -x

REDIRECT=/dev/null

cd ${data_dir}
rm -f local.log | true

# GRIB2 definitions for chemicals and aerosols for MACC
sample_g2=$GRIB_SAMPLES_PATH/GRIB2.tmpl

${tools_dir}grib_set -s paramId=210005 $sample_g2 macc.grb2
result=`${tools_dir}grib_get -p discipline,parameterCategory,parameterNumber,shortName,units,aerosolType,name macc.grb2`
[ "$result" = '0 20 2 aermr05 kg kg**-1 62001 Dust Aerosol (0.55 - 0.9 um) Mixing Ratio' ]
[ `${tools_dir}grib_get -p productDefinitionTemplateNumber macc.grb2` = '44' ]

${tools_dir}grib_set -s paramId=210001 $sample_g2 macc.grb2
result=`${tools_dir}grib_get -p discipline,parameterCategory,parameterNumber,shortName,units,aerosolType,name macc.grb2`
[ "$result" = '0 20 2 aermr01 kg kg**-1 62008 Sea Salt Aerosol (0.03 - 0.5 um) Mixing Ratio' ]

${tools_dir}grib_set -s paramId=210007 $sample_g2 macc.grb2
result=`${tools_dir}grib_get -p discipline,parameterCategory,parameterNumber,shortName,typeOfSizeInterval,aerosolType,name macc.grb2`
[ "$result" = '0 20 2 aermr07 255 62016 Hydrophobic Organic Matter Aerosol Mixing Ratio' ]

${tools_dir}grib_set -s paramId=210061 $sample_g2 macc.grb2
result=`${tools_dir}grib_get -p discipline,parameterCategory,parameterNumber,shortName,productDefinitionTemplateNumber,constituentType,name macc.grb2`
[ "$result" = '0 20 2 co2 40 3 Carbon Dioxide' ]

${tools_dir}grib_set -s paramId=210062 $sample_g2 macc.grb2
result=`${tools_dir}grib_get -p discipline,parameterCategory,parameterNumber,shortName,productDefinitionTemplateNumber,constituentType,name macc.grb2`
[ "$result" = '0 20 2 ch4 40 2 Methane' ]

rm -f macc.grb2 | true


${tools_dir}grib_set -s edition=2,setLocalDefinition=1 reduced_gaussian_model_level.grib1 loc.grib2
${tools_dir}grib_set -s setLocalDefinition=1 reduced_gaussian_model_level.grib1 loc.grib1

#conversion 1->2
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

