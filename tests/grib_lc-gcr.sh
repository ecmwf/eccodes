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

grib2_sample=$ECCODES_SAMPLES_PATH/lc-gcr_sttd_grib2.tmpl
label="grib_lc-gcr_test"
temp1=temp1.${label}.grib2
temp2=temp2.${label}.grib2
temp3=temp3.${label}.grib2

tempLog=temp.${label}.log

tablesVersionLatest=$( ${tools_dir}/grib_get -p tablesVersionLatest $grib2_sample )

test_stream_and_type()
{
    expver=$1

    if [ "$expver" = "prod" ]; then
        pspd=14
    elif [ "$expver" = "test" ]; then
        pspd=15
    fi

    label="grib_lcgcr_test"
    tempFilt=temp.${label}.filt

    # create local section and use ccsds compression
    ${tools_dir}/grib_set -s tablesVersion=$tablesVersionLatest \
                           $grib2_sample $temp2

    grib_check_key_equals $temp2 'class,centre,centre:i,model,stream,mars.stream,type,stattype,timespan,param' 'lr ecmf 98 ERA5 sttd sttd fc moav 24h 235055'
    
    # Check mars domain is not set
    key=`${tools_dir}/grib_get -f -p mars.domain $temp2`
    [ "$key" = "not_found" ]

    # change to type=an
    ${tools_dir}/grib_set -s typeOfProcessedData=0 $temp2 $temp3
    grib_check_key_equals $temp3 'class,model,stream,mars.stream,type,stattype,timespan,param' 'lr ERA5 sttd sttd an moav 24h 235055'

    # test LC-GCR model options
    #   https://jira.ecmwf.int/browse/ECC-2297
    ${tools_dir}/grib_set -s centre=babj,backgroundProcess=1,generatingProcessIdentifier=1 $temp2 $temp3
    grib_check_key_equals $temp3 'class,centre,centre:i,model,stream,type,stattype,timespan,param' 'lr babj 38 CRA-40 sttd fc moav 24h 235055'

    ${tools_dir}/grib_set -s centre=rjtd,backgroundProcess=2,generatingProcessIdentifier=1 $temp2 $temp3
    grib_check_key_equals $temp3 'class,centre,centre:i,model,stream,type,stattype,timespan,param' 'lr rjtd 34 JRA-3Q sttd fc moav 24h 235055'

    ${tools_dir}/grib_set -s centre=nasa,backgroundProcess=3,generatingProcessIdentifier=1 $temp2 $temp3
    grib_check_key_equals $temp3 'class,centre,centre:i,model,stream,type,stattype,timespan,param' 'lr nasa 173 MERRA-2 sttd fc moav 24h 235055'

}

test_stream_and_type 'prod'
test_stream_and_type 'test'


# Clean up
#rm -f $temp1 $temp2 $temp3 $tempLog
