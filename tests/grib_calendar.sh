#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

REDIRECT=/dev/null

temp1=temp1.calendar.$$
temp2=temp2.calendar.$$
sample=$ECCODES_SAMPLES_PATH/GRIB2.tmpl


${tools_dir}/grib_set -s tablesVersion=13,setCalendarId=1,typeOfCalendar=0 $sample $temp1
grib_check_key_equals $temp1 calendarIdPresent 1
grib_check_key_equals $temp1 section1Length    24
grib_check_key_equals $temp1 calendarIdentificationTemplateNumber 0
grib_check_key_equals $temp1 typeOfCalendar    0

${tools_dir}/grib_set -s tablesVersion=13,setCalendarId=1,calendarIdentificationTemplateNumber=1 $sample $temp1
grib_check_key_equals $temp1 calendarIdPresent 1
grib_check_key_equals $temp1 section1Length    25
grib_check_key_equals $temp1 calendarIdentificationTemplateNumber 1
grib_check_key_equals $temp1 numberOfTensOfThousandsOfYearsOfOffset MISSING

${tools_dir}/grib_set -s tablesVersion=13,setCalendarId=1,calendarIdentificationTemplateNumber=2 $sample $temp1
grib_check_key_equals $temp1 calendarIdPresent 1
grib_check_key_equals $temp1 section1Length    26
grib_check_key_equals $temp1 calendarIdentificationTemplateNumber 2
grib_check_key_equals $temp1 typeOfCalendar    255
grib_check_key_equals $temp1 numberOfTensOfThousandsOfYearsOfOffset MISSING

# Remove the calendar section
${tools_dir}/grib_set -s deleteCalendarId=1 $temp1 $temp2
grib_check_key_equals $temp2 section1Length    21
grib_check_key_equals $temp2 calendarIdPresent 0

rm -f $temp1 $temp2
