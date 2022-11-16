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
label="bufr_ecc-1476_test"
tempFilt=temp.$label.filt
tempLog=temp.$label.log

set +u
if test "x$ECCODES_TEST_WITH_VALGRIND" = "x"; then
  echo "Environment variable ECCODES_TEST_WITH_VALGRIND not defined. Test disabled"
  exit 0
fi
set -u

# The presence of ECCODES_TEST_WITH_VALGRIND environment variable redefines
# tools_dir so we reset it to its original
tools_dir=$build_dir/bin

files="
aaen_55.bufr
aben_55.bufr
ahws_139.bufr
airs_57.bufr
alws_139.bufr
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
b005_87.bufr
b005_89.bufr
b006_96.bufr
cmwi_87.bufr
cmwn_87.bufr
cori_156.bufr
crit_202.bufr
csrh_189.bufr
ecc738.msg2.bufr
emsg_189.bufr
emsg_87.bufr
eumetsat_iasi_co.bufr
eum_iasi_twt.bufr
euwv_87.bufr
fy3a_154.bufr
fy3b_154.bufr
g2to_206.bufr
go15_87.bufr
goee_87.bufr
goes_87.bufr
goga_89.bufr
hirb_55.bufr
hirs_55.bufr
iasi_241.bufr
imssnow.bufr
itwt_233.bufr
jaso_214.bufr
maer_207.bufr
mhen_55.bufr
mhsa_55.bufr
mhsb_55.bufr
mhse_55.bufr
mloz_206.bufr
modi_87.bufr
modw_87.bufr
monw_87.bufr
new.bufr
nomi_206.bufr
ocea_21.bufr
pilo_91.bufr
profiler_european.bufr
rada_250.bufr
rado_250.bufr
s4kn_165.bufr
sb19_206.bufr
sbu8_206.bufr
smin_49.bufr
smis_49.bufr
smiu_49.bufr
smos_203.bufr
sn4k_165.bufr
soil_7.bufr
ssbt_127.bufr
stuk_7.bufr
syno_1.bufr
syno_3.bufr
syno_4.bufr
temp_101.bufr
temp-land-with-substituted-values.bufr
tmr7_129.bufr
"

VALGRIND_OPTIONS="--error-exitcode=1 --leak-check=full --log-file=$tempLog"
for f in $files; do
    bf=${data_dir}/bufr/$f
    ${tools_dir}/bufr_dump -Efilter $bf > $tempFilt #>& /dev/null
    valgrind $VALGRIND_OPTIONS ${tools_dir}/codes_bufr_filter $tempFilt $bf >/dev/null
    rm -f $tempFilt
done

rm -f $tempLog $tempFilt

