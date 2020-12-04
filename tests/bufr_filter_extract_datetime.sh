#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

set -x
. ./include.sh

cd ${data_dir}/bufr

# Define a common label for all the tmp files
label="bufr_filter_extract_datetime_test"

# Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

# Define tmp bufr file
fBufrTmp=${label}".bufr.tmp"

# Define filter rules file
fRules=${label}.filter

generate_filter()
{
    start_date=$1
    end_date=$2
    num_subsets_expected=$3

    year1=`echo $start_date  | cut -c 1-4`
    year2=`echo $end_date    | cut -c 1-4`
    month1=`echo $start_date | cut -c 5-6`
    month2=`echo $end_date   | cut -c 5-6`
    day1=`echo $start_date   | cut -c 7-8`
    day2=`echo $end_date     | cut -c 7-8`
    hour1=`echo $start_date   | cut -c 9-10`
    hour2=`echo $end_date     | cut -c 9-10`
    minute1=`echo $start_date | cut -c 11-12`
    minute2=`echo $end_date   | cut -c 11-12`
    second1=`echo $start_date | cut -c 13-14`
    second2=`echo $end_date   | cut -c 13-14`

    cat > $fRules <<EOF
     transient originalNumberOfSubsets = numberOfSubsets;
     set unpack=1;

     transient extractDateTimeYearStart=$year1;
     transient extractDateTimeMonthStart=$month1;
     transient extractDateTimeDayStart=$day1;
     transient extractDateTimeHourStart=$hour1;
     transient extractDateTimeMinuteStart=$minute1;
     transient extractDateTimeSecondStart=$second1;

     transient extractDateTimeYearEnd=$year2;
     transient extractDateTimeMonthEnd=$month2;
     transient extractDateTimeDayEnd=$day2;
     transient extractDateTimeHourEnd=$hour2;
     transient extractDateTimeMinuteEnd=$minute2;
     transient extractDateTimeSecondEnd=$second2;

     set doExtractDateTime=1;
     if (extractedDateTimeNumberOfSubsets!=0) {
       write;
     }
     print "extracted [extractedDateTimeNumberOfSubsets] of [originalNumberOfSubsets] subsets";
     assert($num_subsets_expected == extractedDateTimeNumberOfSubsets);
EOF
}

#-----------------------------------------------------------
echo "Test: Datetime extraction ..."
#-----------------------------------------------------------
cat > $fRules <<EOF
 transient originalNumberOfSubsets = numberOfSubsets;
 set unpack=1;

 transient extractDateTimeYearStart=2012;
 transient extractDateTimeMonthStart=10;
 transient extractDateTimeDayStart=31;
 transient extractDateTimeHourStart=0;
 transient extractDateTimeMinuteStart=1;
 transient extractDateTimeSecondStart=31.6;

 transient extractDateTimeYearEnd=2012;
 transient extractDateTimeMonthEnd=10;
 transient extractDateTimeDayEnd=31;
 transient extractDateTimeHourEnd=0;
 transient extractDateTimeMinuteEnd=1;
 transient extractDateTimeSecondEnd=39.6;

 set doExtractDateTime=1;
 if (extractedDateTimeNumberOfSubsets!=0) {
   write;
 }
 print "extracted [extractedDateTimeNumberOfSubsets] of [originalNumberOfSubsets] subsets";
 assert(numberOfSubsets == extractedDateTimeNumberOfSubsets);
EOF

inputBufr="amsa_55.bufr"
outputBufr=${label}.${inputBufr}.out
outputFilt=${label}.txt
outputRef=${label}.ref
rm -f $outputFilt

echo "Test: Datetime extraction" >> $fLog
echo "file: $inputBufr" >> $fLog

${tools_dir}/codes_bufr_filter -o $outputBufr $fRules $inputBufr  > $outputFilt
[ -f $outputBufr ]

cat > $fRules <<EOF
set unpack=1;
print "year=[year!15]";
print "===========";
print "month=[month!15]";
print "===========";
print "day=[day!15]";
print "===========";
print "hour=[hour!15]";
print "===========";
print "minute=[minute!15]";
print "===========";
print "second=[second!15]";
print "===========";
EOF
${tools_dir}/codes_bufr_filter $fRules $inputBufr $outputBufr  >> $outputFilt

cat > $outputRef <<EOF
extracted 30 of 128 subsets
year=2012
===========
month=10
===========
day=31
===========
hour=0
===========
minute=1
===========
second=23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 
23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 
31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 
31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 
39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 
39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 
47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 
47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 
55.54 55.54 55.54 55.54 55.54 55.54 55.54 55.54
===========
year=2012
===========
month=10
===========
day=31
===========
hour=0
===========
minute=1
===========
second=31.54
===========
EOF

diff $outputRef $outputFilt

#-----------------------------------------------------------
echo "Test invalid date ..."
#-----------------------------------------------------------
cat > $fRules <<EOF
 transient originalNumberOfSubsets=numberOfSubsets;
 set unpack=1;
 transient extractDateTimeYearStart=2012;
 transient extractDateTimeMonthStart=19; # Bad month
 transient extractDateTimeDayStart=31;
 transient extractDateTimeHourStart=0;
 transient extractDateTimeMinuteStart=1;
 transient extractDateTimeSecondStart=31.6;

 transient extractDateTimeYearEnd=2012;
 transient extractDateTimeMonthEnd=10;
 transient extractDateTimeDayEnd=31;
 transient extractDateTimeHourEnd=0;
 transient extractDateTimeMinuteEnd=1;
 transient extractDateTimeSecondEnd=39.6;

 set doExtractDateTime=1;
EOF

inputBufr="amsa_55.bufr"
outputBufr=${label}.${inputBufr}.out

set +e
${tools_dir}/codes_bufr_filter -o $outputBufr $fRules $inputBufr
status=$?
set -e
if [ $status -eq 0 ]; then
   echo "Bad start date: bufr_filter should have failed!"
   exit 1
fi

#-----------------------------------------------------------
echo "Test end date < start date ..."
#-----------------------------------------------------------
cat > $fRules <<EOF
 transient originalNumberOfSubsets=numberOfSubsets;
 set unpack=1;
 transient extractDateTimeYearStart=2012;
 transient extractDateTimeMonthStart=10;
 transient extractDateTimeDayStart=31;
 transient extractDateTimeHourStart=0;
 transient extractDateTimeMinuteStart=1;
 transient extractDateTimeSecondStart=31.6;

 transient extractDateTimeYearEnd=2012;
 transient extractDateTimeMonthEnd=10;
 transient extractDateTimeDayEnd=30; #Error
 transient extractDateTimeHourEnd=0;
 transient extractDateTimeMinuteEnd=1;
 transient extractDateTimeSecondEnd=39.6;

 set doExtractDateTime=1;
EOF

inputBufr="amsa_55.bufr"
outputBufr=${label}.${inputBufr}.out

set +e
${tools_dir}/codes_bufr_filter -o $outputBufr $fRules $inputBufr
status=$?
set -e
if [ $status -eq 0 ]; then
   echo "End date before start date: bufr_filter should have failed!"
   exit 1
fi

#-----------------------------------------------------------
echo "Test boundary case ..."
#-----------------------------------------------------------
cat > $fRules <<EOF
 transient originalNumberOfSubsets=numberOfSubsets;
 set unpack=1;
 transient extractDateTimeYearStart  =2012;
 transient extractDateTimeMonthStart =10;
 transient extractDateTimeDayStart   =31;
 transient extractDateTimeHourStart  =0;
 transient extractDateTimeMinuteStart=1;
 transient extractDateTimeSecondStart=24;

 transient extractDateTimeYearEnd    =2012;
 transient extractDateTimeMonthEnd   =10;
 transient extractDateTimeDayEnd     =31;
 transient extractDateTimeHourEnd    =0;
 transient extractDateTimeMinuteEnd  =1;
 transient extractDateTimeSecondEnd  =31;#25;

 set doExtractDateTime=1;
 print "extracted [extractedDateTimeNumberOfSubsets] of [originalNumberOfSubsets] subsets";
 assert(extractedDateTimeNumberOfSubsets==0);
EOF
inputBufr="amsa_55.bufr"
outputBufr=${label}.${inputBufr}.out
${tools_dir}/codes_bufr_filter -o $outputBufr $fRules $inputBufr
rm -f $outputBufr

#-----------------------------------------------------------
echo "Test uncompressed BUFR ..."
#-----------------------------------------------------------
inputBufr="delayed_repl_01.bufr"
outputBufr=${label}.${inputBufr}.out

# The generate_filter function creates $fRules
# Arguments: start_date end_date num_subsets_expected

generate_filter 20171102112500 20171102125959 14
${tools_dir}/codes_bufr_filter -o $outputBufr $fRules $inputBufr

generate_filter 20171102114000 20171102125959 12
${tools_dir}/codes_bufr_filter -o $outputBufr $fRules $inputBufr

generate_filter 20171102120000 20171102125959 8
${tools_dir}/codes_bufr_filter -o $outputBufr $fRules $inputBufr

#-----------------------------------------------------------
echo "Test ECC-1174 ..."
#-----------------------------------------------------------
inputBufr="smin_49.bufr"
temp=temp.${label}.bufr
errlog=temp.${label}.err
outputBufr=${label}.${inputBufr}.out
miss='-1e+100'
${tools_dir}/bufr_set -s unpack=1,second=$miss,pack=1 $inputBufr $temp
generate_filter 20121031000000 20121031000001 60
${tools_dir}/codes_bufr_filter -o $outputBufr $fRules $temp 2>$errlog
grep -q "WARNING.*Key '#1#second' is missing" $errlog
rm -f $temp $errlog

# Clean up
rm -f $outputRef $outputFilt $outputBufr $fLog $fRules
