#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-765.
# BUFR decoding memory leaks
# ---------------------------------------------------------
label="bufr_ecc-765-test"
tempLog=temp.${label}.log
tempSupp=temp.${label}.supp

if test "x$ECCODES_TEST_WITH_VALGRIND" = "x"; then
    echo "Environment variable ECCODES_TEST_WITH_VALGRIND not defined. Test disabled"
    exit 0
fi

# The presence of ECCODES_TEST_WITH_VALGRIND environment variable redefines
# tools_dir so we reset it to its original
tools_dir=$build_dir/bin

# ---------------------------------------------------------
# Test 1: Set of BUFR files which have zero leaks.
# Over time this list should increase
# ---------------------------------------------------------
bufr_files_zero_leaks="
    207003.bufr
    PraticaTemp.bufr
    aaen_55.bufr
    aben_55.bufr
    aeolus_wmo_26.bufr
    delayed_repl_01.bufr
    goes16_nm.bufr
    good_j2eo.bufr
    israel_observations_2017041010.bufr
    mhen_55.bufr
    modw_87.bufr
    sentinel1.bufr
    synop_multi_subset.bufr
    tropical_cyclone.bufr
    uegabe.bufr
"

VALGRIND_OPTIONS="--error-exitcode=1 --leak-check=full --log-file=$tempLog"

# bufr_filter has a small leak to do with parsing and action files not related to BUFR decoding,
# So instead we use bufr_ls to do the unpacking
for bf in $bufr_files_zero_leaks; do
    valgrind $VALGRIND_OPTIONS ${tools_dir}/bufr_ls -s unpack=1 ${data_dir}/bufr/$bf >/dev/null
done


# ---------------------------------------------------------
# Test 2: Set of BUFR files which have a small known leak.
# Over time this list should decrease
# ---------------------------------------------------------
bufr_files_known_leaks="
    ahws_139.bufr
    airc_142.bufr
    airc_144.bufr
    airs_57.bufr
    alws_139.bufr
    amda_144.bufr
    amsa_55.bufr
    amsb_55.bufr
    amse_55.bufr
    amsu_55.bufr
    amv2_87.bufr
    amv3_87.bufr
    asbh_139.bufr
    asbl_139.bufr
    asca_139.bufr
    asch_139.bufr
    ascs_139.bufr
    aseh_139.bufr
    asel_139.bufr
    ashs_139.bufr
    atap_55.bufr
    ateu_155.bufr
    atms_201.bufr
    atov_55.bufr
    avhm_87.bufr
    avhn_87.bufr
    avhr_58.bufr
    b002_95.bufr
    b002_96.bufr
    b003_56.bufr
    b004_145.bufr
    b005_87.bufr
    b005_89.bufr
    b006_96.bufr
    b007_31.bufr
    bssh_170.bufr
    bssh_176.bufr
    bssh_178.bufr
    bssh_180.bufr
    btem_109.bufr
    buoy_27.bufr
    cmwi_87.bufr
    cmwn_87.bufr
    cnow_28.bufr
    cori_156.bufr
    crit_202.bufr
    csrh_189.bufr
    emsg_189.bufr
    emsg_87.bufr
    eum_iasi_twt.bufr
    eumetsat_iasi_co.bufr
    euwv_87.bufr
    fy3a_154.bufr
    fy3b_154.bufr
    g2nd_208.bufr
    g2to_206.bufr
    go15_87.bufr
    goee_87.bufr
    goes_87.bufr
    goga_89.bufr
    gosat.bufr
    grst_26.bufr
    gsd1_208.bufr
    gsd2_208.bufr
    gsd3_208.bufr
    gst4_26.bufr
    hirb_55.bufr
    hirs_55.bufr
    ias1_240.bufr
    iasi_241.bufr
    ifco_208.bufr
    ikco_217.bufr
    imssnow.bufr
    itrg_208.bufr
    itwt_233.bufr
    j2eo_216.bufr
    j2nb_216.bufr
    jaso_214.bufr
    kond_209.bufr
    maer_207.bufr
    meta_140.bufr
    metar_with_2_bias.bufr
    mhsa_55.bufr
    mhsb_55.bufr
    mhse_55.bufr
    mloz_206.bufr
    modi_87.bufr
    monw_87.bufr
    new.bufr
    nomi_206.bufr
    nos1_208.bufr
    nos2_208.bufr
    nos3_208.bufr
    nos4_208.bufr
    nos5_208.bufr
    nos6_208.bufr
    nos7_208.bufr
    nos8_208.bufr
    ocea_131.bufr
    ocea_132.bufr
    ocea_133.bufr
    ocea_21.bufr
    pgps_110.bufr
    pilo_91.bufr
    profiler_european.bufr
    rada_250.bufr
    rado_250.bufr
    s4kn_165.bufr
    sb19_206.bufr
    sbu8_206.bufr
    ship_11.bufr
    ship_12.bufr
    ship_13.bufr
    ship_14.bufr
    ship_19.bufr
    ship_9.bufr
    smin_49.bufr
    smis_49.bufr
    smiu_49.bufr
    smos_203.bufr
    sn4k_165.bufr
    soil_7.bufr
    srals3_ecmwf4.bufr
    srals3_eumetsat.bufr
    ssbt_127.bufr
    stuk_7.bufr
    syno_1.bufr
    syno_2.bufr
    syno_3.bufr
    syno_4.bufr
    syno_multi.bufr
    synop.bufr
    temp-land-with-substituted-values.bufr
    temp_101.bufr
    temp_102.bufr
    temp_106.bufr
    tmr7_129.bufr
    tros_31.bufr
    wavb_134.bufr
"

cat > $tempSupp << EOF
{
   eccodes_string_split_suppression
   Memcheck:Leak
   match-leak-kinds: definite
   fun:malloc
   fun:string_split
   fun:load_bufr_elements_table*
   fun:bufr_get_from_table
   fun:accessor_bufr_elements_table_get_descriptor
   fun:grib_bufr_descriptor_new
}
EOF
VALGRIND_OPTIONS="--error-exitcode=1 --leak-check=full --log-file=$tempLog --suppressions=$tempSupp"

# bufr_filter has a small leak to do with parsing and action files not related to BUFR decoding,
# So instead we use bufr_ls to do the unpacking
for bf in $bufr_files_known_leaks; do
    valgrind $VALGRIND_OPTIONS ${tools_dir}/bufr_ls -s unpack=1 ${data_dir}/bufr/$bf >/dev/null
done


# Clean up
rm -f $tempLog $tempSupp
