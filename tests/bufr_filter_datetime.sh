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

# Define a common label for all the tmp files
label="bufr_filter_datetime_test"

tempErr=temp.$label.err

cd ${data_dir}/bufr

fLog=${label}".log"
rm -f $fLog
touch $fLog

fBufrTmp=${label}".bufr.tmp"
fRules=${label}.filter


#-----------------------------------------------------------
# Test: DateTime
#-----------------------------------------------------------
cat > $fRules <<EOF
transient myStartYear=2012;
transient myStartMonth=10;
transient myStartDay=29;
transient myStartHour=21;
transient myStartMinute=0;
transient myStartSecond=0;

transient myEndDate=20121030;
transient myEndTime=050000;

meta myStartDateTime julian_date(myStartYear,myStartMonth,myStartDay,myStartHour,myStartMinute,myStartSecond);
meta myEndDateTime julian_date(myEndDate,myEndTime);

if (rdbDateTime > myStartDateTime && rdbDateTime < myEndDateTime) {
  print "match";
} else {
  print "no match";
}
print "rdbtimeDate=[rdbtimeDate] rdbtimeTime=[rdbtimeTime] rdbDateTime=[rdbDateTime%f] mystart=[myEndDateTime] myend=[myEndDateTime]";

print "rdbDateTime=[rdbDateTime:s]";

set myEndDateTime="2017-05-22 12:15:23";
print "myEndDate=[myEndDateTime:s] myEndDateTime=[myEndDateTime] myEndDate=[myEndDate] myEndTime=[myEndTime]";

set myEndDateTime=rdbDateTime;
print "myEndDate=[myEndDateTime:s] myEndDateTime=[myEndDateTime] myEndDate=[myEndDate] myEndTime=[myEndTime]";

set userDateTimeStart="2017/05/23 09-12:12";
print "userDateTimeStart=[userDateTimeStart] userDateStart=[userDateStart] userTimeStart=[userTimeStart] [userDateTimeStart:s]";
set userDateTimeStart="20170523 091212";
print "userDateTimeStart=[userDateTimeStart] userDateStart=[userDateStart] userTimeStart=[userTimeStart] [userDateTimeStart:s]";
set userDateTimeStart="20170523091212";
print "userDateTimeStart=[userDateTimeStart] userDateStart=[userDateStart] userTimeStart=[userTimeStart] [userDateTimeStart:s]";
EOF

f="syno_1.bufr"

echo "Test: Julian Date" >> $fLog
echo "file: $f" >> $fLog

${tools_dir}/codes_bufr_filter $fRules $f  > ${f}.log

cat > ${f}.log.ref <<EOF
match
rdbtimeDate=20121030 rdbtimeTime=001019 rdbDateTime=2456230.507164 mystart=2456230.708333 myend=2456230.708333
rdbDateTime=20121030 001019
myEndDate=2017-05-22 12:15:23 myEndDateTime=2457896.01068 myEndDate=20170522 myEndTime=121523
myEndDate=2012-10-30 10:19:00 myEndDateTime=2456230.50716 myEndDate=20121030 myEndTime=1019
userDateTimeStart=2457896.88347 userDateStart=20170523 userTimeStart=91212 2017/05/23 09-12:12
userDateTimeStart=2457896.88347 userDateStart=20170523 userTimeStart=91212 20170523 091212
userDateTimeStart=2457896.88347 userDateStart=20170523 userTimeStart=91212 20170523091212
EOF

diff ${f}.log.ref ${f}.log
rm -f $f.log ${f}.log.ref


# Clean up
rm -f ${f}.log ${f}.log.ref ${f}.out $fLog $fRules
rm -f $tempErr
