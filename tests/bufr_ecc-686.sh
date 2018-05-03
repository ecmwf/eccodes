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

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-686.
# It tests changing the subtype in the ECMWF local section
# ---------------------------------------------------------
label="bufr_ecc-686-test"
tempRules=temp.${label}.filter
tempBufr=temp.${label}.bufr

set_subtype()
{
    oldSubtype=$1
    newSubtype=$2
    input=${data_dir}/bufr/aaen_55.bufr
    cat > $tempRules << EOF
        print " Setting oldSubtype $oldSubtype and newSubtype $newSubtype";
        print " BEFORE: numObs=[localNumberOfObservations] satID=[satelliteID]";
        transient old_localNumberOfObservations = localNumberOfObservations;
        transient old_satelliteID = satelliteID;
        set oldSubtype=$oldSubtype;
        set newSubtype=$newSubtype;
        print " AFTER: numObs=[localNumberOfObservations] satID=[satelliteID]";
        set satelliteID = old_satelliteID;
        set localNumberOfObservations = old_localNumberOfObservations;
        print " RESTORE: numObs=[localNumberOfObservations] satID=[satelliteID]";
        assert(localNumberOfObservations==128);
        assert(satelliteID==209);
        write;
EOF
    ${tools_dir}/codes_bufr_filter -o $tempBufr $tempRules $input
    res=`${tools_dir}/bufr_get -p oldSubtype,newSubtype,localNumberOfObservations,satelliteID $tempBufr`
    [ "$res" = "$oldSubtype $newSubtype 128 209" ]
    rm -f $tempRules $tempBufr
}

# Non-special subType
set_subtype 30  256

# Special range: 31, 121 <= x <= 130
set_subtype 31  256
set_subtype 121 256
set_subtype 130 256

# oldSubtype = maximum 255 and newSubtype takes over
set_subtype 255 256

rm -f $tempRules $tempBufr
