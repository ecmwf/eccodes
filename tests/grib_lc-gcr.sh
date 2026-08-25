#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#
# ---------------------------------------------------------
# This is the test for JIRA issues:
#     ECC-2004 MARS: Add new class 'lr' for WMO Lead Centre for Global Climate Re-Analyses (new MARS class support)
#     ECC-2069 GRIB2: Update Code Table 1.3 for WMO Lead Centre for Global Climate Re-Analyses (new WMO codes support)
#     ECC-2270 GRIB2: LC-GCR and WPMIP update (MARS support)
#     ECC-2314 grib: LC-GCR project test case and other updates (ecCodes test etc)
#     ECC-2297 GRIB2: Updates for LC-GCR (Support for the keyword model)
# Parent data governance ticket with all details (Jira epic): DGOV-513
# ---------------------------------------------------------

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
    else
        echo "Abort: expver not prod/test (i.e. productionStatusOfProcessedData not 14/15)!"
        exit -1
    fi

    # change productionStatusOfProcessedData
    ${tools_dir}/grib_set -s tablesVersion=$tablesVersionLatest,productionStatusOfProcessedData=$pspd \
                           $grib2_sample $temp2

    grib_check_key_equals $temp2 'class,expver,centre,centre:i,model,stream,mars.stream,type,stattype,timespan,param' "lr $expver ecmf 98 ERA5 sttd sttd fc moav 24h 235055"
    
    # Check mars domain is not set
    key=`${tools_dir}/grib_get -f -p mars.domain $temp2`
    [ "$key" = "not_found" ]

    # change to type=an
    ${tools_dir}/grib_set -s typeOfProcessedData=0 $temp2 $temp3
    grib_check_key_equals $temp3 'class,expver,model,stream,mars.stream,type,stattype,timespan,param' "lr $expver ERA5 sttd sttd an moav 24h 235055"

    # test LC-GCR model options
    #   https://jira.ecmwf.int/browse/ECC-2297
    ${tools_dir}/grib_set -s centre=babj,backgroundProcess=1,generatingProcessIdentifier=1 $temp2 $temp3
    grib_check_key_equals $temp3 'class,expver,centre,centre:i,model,stream,type,stattype,timespan,param' "lr $expver babj 38 CRA-40 sttd fc moav 24h 235055"

    ${tools_dir}/grib_set -s centre=rjtd,backgroundProcess=2,generatingProcessIdentifier=1 $temp2 $temp3
    grib_check_key_equals $temp3 'class,expver,centre,centre:i,model,stream,type,stattype,timespan,param' "lr $expver rjtd 34 JRA-3Q sttd fc moav 24h 235055"

    ${tools_dir}/grib_set -s centre=nasa,backgroundProcess=3,generatingProcessIdentifier=1 $temp2 $temp3
    grib_check_key_equals $temp3 'class,expver,centre,centre:i,model,stream,type,stattype,timespan,param' "lr $expver nasa 173 MERRA-2 sttd fc moav 24h 235055"

}

test_stream_and_type 'prod'
test_stream_and_type 'test'


# Clean up
rm -f $temp1 $temp2 $temp3 $tempLog
