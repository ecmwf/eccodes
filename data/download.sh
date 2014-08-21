#!/bin/sh

usage ()
{
   prog=`basename $0`
   echo "Usage: $prog [-v] data_dir"
   echo
   echo "-v  verbose"
   echo "-h  prints this help message"
   echo
}

VERBOSE=0
while :
do
   case "$1" in
      -h) usage ; exit 0;;
      -v) VERBOSE=1
         echo "Running with verbose setting"
         ;;
      --) shift ; break ;;
      -*) usage ; exit 0;;
      *)  break;;
   esac
   shift
done

DATA_DIR=$1
if [ -z "$DATA_DIR" ]; then
   echo "Error: No directory specified." 2>&1
   usage
   exit 1
fi

# Check if all downloads are already done
if [ -f "${DATA_DIR}/.downloaded" ]; then
   if [ $VERBOSE -eq 1 ]; then
      echo "All downloads are already done. Exiting."
   fi
   exit 0
fi

files="
    bad.grib
    in_copy.grib
    budg
    constant_field.grib1
    constant_width_bitmap.grib
    constant_width_boust_bitmap.grib
    gen.grib
    gen_bitmap.grib
    gen_ext_bitmap.grib
    gen_ext_boust_bitmap.grib
    gen_ext_boust.grib
    gen_ext.grib
    gen_ext_spd_2_bitmap.grib
    gen_ext_spd_2_boust_bitmap.grib
    gen_ext_spd_2.grib
    gen_ext_spd_3_boust_bitmap.grib
    gen_ext_spd_3.grib
    gfs.c255.grib2
    gts.grib
    index.grib
    grid_ieee.grib
    jpeg.grib2
    lfpw.grib1
    missing_field.grib1
    missing.grib2
    mixed.grib
    multi_created.grib2
    multi.grib2
    pad.grib
    reduced_gaussian_lsm.grib1
    reduced_gaussian_model_level.grib1
    reduced_gaussian_model_level.grib2
    reduced_gaussian_pressure_level_constant.grib1
    reduced_gaussian_pressure_level_constant.grib2
    reduced_gaussian_pressure_level.grib1
    reduced_gaussian_pressure_level.grib2
    reduced_gaussian_sub_area.grib1
    reduced_gaussian_sub_area.grib2
    reduced_gaussian_surface.grib1
    reduced_gaussian_surface.grib2
    reduced_gaussian_surface_jpeg.grib2
    reduced_latlon_surface_constant.grib1
    reduced_latlon_surface_constant.grib2
    reduced_latlon_surface.grib1
    reduced_latlon_surface.grib2
    reference_ensemble_mean.grib1
    reference_stdev.grib1
    regular_gaussian_model_level.grib1
    regular_gaussian_model_level.grib2
    regular_gaussian_pressure_level_constant.grib1
    regular_gaussian_pressure_level_constant.grib2
    regular_gaussian_pressure_level.grib1
    regular_gaussian_pressure_level.grib2
    regular_gaussian_surface.grib1
    regular_gaussian_surface.grib2
    regular_latlon_surface_constant.grib1
    regular_latlon_surface_constant.grib2
    regular_latlon_surface.grib1
    regular_latlon_surface.grib2
    row.grib
    sample.grib2
    satellite.grib
    second_ord_rbr.grib1
    simple_bitmap.grib
    simple.grib
    small_ensemble.grib1
    spectral_compex.grib1
    spectral_complex.grib1
    spherical_model_level.grib1
    spherical_pressure_level.grib1
    sst_globus0083.grib
    test.grib1
    test_uuid.grib2
    tigge_af_ecmwf.grib2
    tigge_cf_ecmwf.grib2
    tigge_ecmwf.grib2
    tigge_pf_ecmwf.grib2
    timeRangeIndicator_0.grib
    timeRangeIndicator_10.grib
    timeRangeIndicator_5.grib
    tp_ecmwf.grib
    v.grib2
    tigge/tigge_ammc_pl_gh.grib
    tigge/tigge_ammc_pl_q.grib
    tigge/tigge_ammc_pl_t.grib
    tigge/tigge_ammc_pl_u.grib
    tigge/tigge_ammc_pl_v.grib
    tigge/tigge_ammc_sfc_10u.grib
    tigge/tigge_ammc_sfc_10v.grib
    tigge/tigge_ammc_sfc_2t.grib
    tigge/tigge_ammc_sfc_lsm.grib
    tigge/tigge_ammc_sfc_mn2t6.grib
    tigge/tigge_ammc_sfc_msl.grib
    tigge/tigge_ammc_sfc_mx2t6.grib
    tigge/tigge_ammc_sfc_orog.grib
    tigge/tigge_ammc_sfc_sf.grib
    tigge/tigge_ammc_sfc_sp.grib
    tigge/tigge_ammc_sfc_st.grib
    tigge/tigge_ammc_sfc_tcc.grib
    tigge/tigge_ammc_sfc_tcw.grib
    tigge/tigge_ammc_sfc_tp.grib
    tigge/tigge_babj_pl_gh.grib
    tigge/tigge_babj_pl_q.grib
    tigge/tigge_babj_pl_t.grib
    tigge/tigge_babj_pl_u.grib
    tigge/tigge_babj_pl_v.grib
    tigge/tigge_babj_sfc_10u.grib
    tigge/tigge_babj_sfc_10v.grib
    tigge/tigge_babj_sfc_2d.grib
    tigge/tigge_babj_sfc_2t.grib
    tigge/tigge_babj_sfc_lsm.grib
    tigge/tigge_babj_sfc_mn2t6.grib
    tigge/tigge_babj_sfc_msl.grib
    tigge/tigge_babj_sfc_mx2t6.grib
    tigge/tigge_babj_sfc_orog.grib
    tigge/tigge_babj_sfc_sd.grib
    tigge/tigge_babj_sfc_sf.grib
    tigge/tigge_babj_sfc_slhf.grib
    tigge/tigge_babj_sfc_sp.grib
    tigge/tigge_babj_sfc_sshf.grib
    tigge/tigge_babj_sfc_ssr.grib
    tigge/tigge_babj_sfc_str.grib
    tigge/tigge_babj_sfc_tcc.grib
    tigge/tigge_babj_sfc_tcw.grib
    tigge/tigge_babj_sfc_tp.grib
    tigge/tigge_cwao_pl_gh.grib
    tigge/tigge_cwao_pl_q.grib
    tigge/tigge_cwao_pl_t.grib
    tigge/tigge_cwao_pl_u.grib
    tigge/tigge_cwao_pl_v.grib
    tigge/tigge_cwao_sfc_10u.grib
    tigge/tigge_cwao_sfc_10v.grib
    tigge/tigge_cwao_sfc_2d.grib
    tigge/tigge_cwao_sfc_2t.grib
    tigge/tigge_cwao_sfc_mn2t6.grib
    tigge/tigge_cwao_sfc_msl.grib
    tigge/tigge_cwao_sfc_mx2t6.grib
    tigge/tigge_cwao_sfc_orog.grib
    tigge/tigge_cwao_sfc_sd.grib
    tigge/tigge_cwao_sfc_skt.grib
    tigge/tigge_cwao_sfc_sp.grib
    tigge/tigge_cwao_sfc_st.grib
    tigge/tigge_cwao_sfc_tcc.grib
    tigge/tigge_cwao_sfc_tcw.grib
    tigge/tigge_cwao_sfc_tp.grib
    tigge/tigge_ecmf_pl_gh.grib
    tigge/tigge_ecmf_pl_q.grib
    tigge/tigge_ecmf_pl_t.grib
    tigge/tigge_ecmf_pl_u.grib
    tigge/tigge_ecmf_pl_v.grib
    tigge/tigge_ecmf_pt_pv.grib
    tigge/tigge_ecmf_pv_pt.grib
    tigge/tigge_ecmf_pv_u.grib
    tigge/tigge_ecmf_pv_v.grib
    tigge/tigge_ecmf_sfc_10u.grib
    tigge/tigge_ecmf_sfc_10v.grib
    tigge/tigge_ecmf_sfc_2d.grib
    tigge/tigge_ecmf_sfc_2t.grib
    tigge/tigge_ecmf_sfc_cap.grib
    tigge/tigge_ecmf_sfc_cape.grib
    tigge/tigge_ecmf_sfc_mn2t6.grib
    tigge/tigge_ecmf_sfc_msl.grib
    tigge/tigge_ecmf_sfc_mx2t6.grib
    tigge/tigge_ecmf_sfc_sd.grib
    tigge/tigge_ecmf_sfc_sf.grib
    tigge/tigge_ecmf_sfc_skt.grib
    tigge/tigge_ecmf_sfc_slhf.grib
    tigge/tigge_ecmf_sfc_sm.grib
    tigge/tigge_ecmf_sfc_sp.grib
    tigge/tigge_ecmf_sfc_sshf.grib
    tigge/tigge_ecmf_sfc_ssr.grib
    tigge/tigge_ecmf_sfc_st.grib
    tigge/tigge_ecmf_sfc_str.grib
    tigge/tigge_ecmf_sfc_sund.grib
    tigge/tigge_ecmf_sfc_tcc.grib
    tigge/tigge_ecmf_sfc_tcw.grib
    tigge/tigge_ecmf_sfc_tp.grib
    tigge/tigge_ecmf_sfc_ttr.grib
    tigge/tigge_egrr_pl_gh.grib
    tigge/tigge_egrr_pl_q.grib
    tigge/tigge_egrr_pl_t.grib
    tigge/tigge_egrr_pl_u.grib
    tigge/tigge_egrr_pl_v.grib
    tigge/tigge_egrr_pt_pv.grib
    tigge/tigge_egrr_pv_pt.grib
    tigge/tigge_egrr_pv_u.grib
    tigge/tigge_egrr_pv_v.grib
    tigge/tigge_egrr_sfc_10u.grib
    tigge/tigge_egrr_sfc_10v.grib
    tigge/tigge_egrr_sfc_2d.grib
    tigge/tigge_egrr_sfc_2t.grib
    tigge/tigge_egrr_sfc_mn2t6.grib
    tigge/tigge_egrr_sfc_msl.grib
    tigge/tigge_egrr_sfc_mx2t6.grib
    tigge/tigge_egrr_sfc_sd.grib
    tigge/tigge_egrr_sfc_sf.grib
    tigge/tigge_egrr_sfc_skt.grib
    tigge/tigge_egrr_sfc_slhf.grib
    tigge/tigge_egrr_sfc_sm.grib
    tigge/tigge_egrr_sfc_sp.grib
    tigge/tigge_egrr_sfc_sshf.grib
    tigge/tigge_egrr_sfc_ssr.grib
    tigge/tigge_egrr_sfc_st.grib
    tigge/tigge_egrr_sfc_str.grib
    tigge/tigge_egrr_sfc_tcc.grib
    tigge/tigge_egrr_sfc_tcw.grib
    tigge/tigge_egrr_sfc_tp.grib
    tigge/tigge_egrr_sfc_ttr.grib
    tigge/tigge_kwbc_pl_gh.grib
    tigge/tigge_kwbc_pl_q.grib
    tigge/tigge_kwbc_pl_t.grib
    tigge/tigge_kwbc_pl_u.grib
    tigge/tigge_kwbc_pl_v.grib
    tigge/tigge_kwbc_pt_pv.grib
    tigge/tigge_kwbc_pv_pt.grib
    tigge/tigge_kwbc_pv_u.grib
    tigge/tigge_kwbc_pv_v.grib
    tigge/tigge_kwbc_sfc_10u.grib
    tigge/tigge_kwbc_sfc_10v.grib
    tigge/tigge_kwbc_sfc_2d.grib
    tigge/tigge_kwbc_sfc_2t.grib
    tigge/tigge_kwbc_sfc_cap.grib
    tigge/tigge_kwbc_sfc_cape.grib
    tigge/tigge_kwbc_sfc_ci.grib
    tigge/tigge_kwbc_sfc_lsm.grib
    tigge/tigge_kwbc_sfc_mn2t6.grib
    tigge/tigge_kwbc_sfc_msl.grib
    tigge/tigge_kwbc_sfc_mx2t6.grib
    tigge/tigge_kwbc_sfc_sd.grib
    tigge/tigge_kwbc_sfc_sf.grib
    tigge/tigge_kwbc_sfc_skt.grib
    tigge/tigge_kwbc_sfc_slhf.grib
    tigge/tigge_kwbc_sfc_sm.grib
    tigge/tigge_kwbc_sfc_sp.grib
    tigge/tigge_kwbc_sfc_sshf.grib
    tigge/tigge_kwbc_sfc_ssr.grib
    tigge/tigge_kwbc_sfc_st.grib
    tigge/tigge_kwbc_sfc_str.grib
    tigge/tigge_kwbc_sfc_tcw.grib
    tigge/tigge_kwbc_sfc_tp.grib
    tigge/tigge_kwbc_sfc_ttr.grib
    tigge/tigge_lfpw_pl_gh.grib
    tigge/tigge_lfpw_pl_q.grib
    tigge/tigge_lfpw_pl_t.grib
    tigge/tigge_lfpw_pl_u.grib
    tigge/tigge_lfpw_pl_v.grib
    tigge/tigge_lfpw_pv_pt.grib
    tigge/tigge_lfpw_pv_u.grib
    tigge/tigge_lfpw_pv_v.grib
    tigge/tigge_lfpw_sfc_10u.grib
    tigge/tigge_lfpw_sfc_10v.grib
    tigge/tigge_lfpw_sfc_2d.grib
    tigge/tigge_lfpw_sfc_2t.grib
    tigge/tigge_lfpw_sfc_cap.grib
    tigge/tigge_lfpw_sfc_cape.grib
    tigge/tigge_lfpw_sfc_mn2t6.grib
    tigge/tigge_lfpw_sfc_msl.grib
    tigge/tigge_lfpw_sfc_mx2t6.grib
    tigge/tigge_lfpw_sfc_sd.grib
    tigge/tigge_lfpw_sfc_sf.grib
    tigge/tigge_lfpw_sfc_skt.grib
    tigge/tigge_lfpw_sfc_slhf.grib
    tigge/tigge_lfpw_sfc_sp.grib
    tigge/tigge_lfpw_sfc_sshf.grib
    tigge/tigge_lfpw_sfc_ssr.grib
    tigge/tigge_lfpw_sfc_st.grib
    tigge/tigge_lfpw_sfc_str.grib
    tigge/tigge_lfpw_sfc_tcc.grib
    tigge/tigge_lfpw_sfc_tcw.grib
    tigge/tigge_lfpw_sfc_tp.grib
    tigge/tigge_lfpw_sfc_ttr.grib
    tigge/tigge_rjtd_pl_gh.grib
    tigge/tigge_rjtd_pl_q.grib
    tigge/tigge_rjtd_pl_t.grib
    tigge/tigge_rjtd_pl_u.grib
    tigge/tigge_rjtd_pl_v.grib
    tigge/tigge_rjtd_sfc_10u.grib
    tigge/tigge_rjtd_sfc_10v.grib
    tigge/tigge_rjtd_sfc_2d.grib
    tigge/tigge_rjtd_sfc_2t.grib
    tigge/tigge_rjtd_sfc_mn2t6.grib
    tigge/tigge_rjtd_sfc_msl.grib
    tigge/tigge_rjtd_sfc_mx2t6.grib
    tigge/tigge_rjtd_sfc_sd.grib
    tigge/tigge_rjtd_sfc_skt.grib
    tigge/tigge_rjtd_sfc_slhf.grib
    tigge/tigge_rjtd_sfc_sm.grib
    tigge/tigge_rjtd_sfc_sp.grib
    tigge/tigge_rjtd_sfc_sshf.grib
    tigge/tigge_rjtd_sfc_ssr.grib
    tigge/tigge_rjtd_sfc_str.grib
    tigge/tigge_rjtd_sfc_tcc.grib
    tigge/tigge_rjtd_sfc_tcw.grib
    tigge/tigge_rjtd_sfc_tp.grib
    tigge/tigge_rjtd_sfc_ttr.grib
    tigge/tigge_rksl_pl_gh.grib
    tigge/tigge_rksl_pl_q.grib
    tigge/tigge_rksl_pl_t.grib
    tigge/tigge_rksl_pl_u.grib
    tigge/tigge_rksl_pl_v.grib
    tigge/tigge_rksl_sfc_10u.grib
    tigge/tigge_rksl_sfc_10v.grib
    tigge/tigge_rksl_sfc_2t.grib
    tigge/tigge_rksl_sfc_msl.grib
    tigge/tigge_rksl_sfc_sp.grib
    tigge/tigge_sbsj_pl_gh.grib
    tigge/tigge_sbsj_pl_q.grib
    tigge/tigge_sbsj_pl_t.grib
    tigge/tigge_sbsj_pl_u.grib
    tigge/tigge_sbsj_pl_v.grib
    tigge/tigge_sbsj_sfc_10u.grib
    tigge/tigge_sbsj_sfc_10v.grib
    tigge/tigge_sbsj_sfc_2t.grib
    tigge/tigge_sbsj_sfc_msl.grib
    tigge/tigge_sbsj_sfc_sf.grib
    tigge/tigge_sbsj_sfc_skt.grib
    tigge/tigge_sbsj_sfc_sp.grib
    tigge/tigge_sbsj_sfc_ssr.grib
    tigge/tigge_sbsj_sfc_st.grib
    tigge/tigge_sbsj_sfc_tcc.grib
    tigge/tigge_sbsj_sfc_tcw.grib
    tigge/tigge_sbsj_sfc_tp.grib
    tigge/tiggelam_cnmc_sfc.grib
  "

[ -d "${DATA_DIR}/tigge" ] || mkdir "${DATA_DIR}/tigge"

# Decide what tool to use to download data
DNLD_PROG=""
if command -v wget >/dev/null 2>&1; then
   PROG=wget
   OPTIONS="--tries=1 --timeout=3 -nv -q -O"
   if [ $VERBOSE -eq 1 ]; then
      OPTIONS="--tries=1 --timeout=3 -nv -O"
   fi
   DNLD_PROG="$PROG $OPTIONS"
fi
if command -v curl >/dev/null 2>&1; then
   PROG=curl
   OPTIONS="--silent --show-error --fail --output"
   if [ $VERBOSE -eq 1 ]; then
      OPTIONS="--show-error --fail --output"
   fi
   DNLD_PROG="$PROG $OPTIONS"
fi
if test "x$DNLD_PROG" = "x"; then
   echo "Cannot find tool to transfer data from download server. Aborting." 1>&2
   exit 1
fi

download_URL="http://download.ecmwf.org"
cd ${DATA_DIR}
echo "Downloading data files for testing..."
for f in $files; do
   # If we haven't already got the file, download it
   if [ ! -f "$f" ]; then
      if [ $VERBOSE -eq 1 ]; then
         echo "$DNLD_PROG $f ${download_URL}/test-data/grib_api/data/$f"
      fi
      $DNLD_PROG $f ${download_URL}/test-data/grib_api/data/$f
      if [ $? -ne 0 ]; then
          echo "Failed to download file: $f"
          exit 1
      fi

      if [ $VERBOSE -eq 1 ]; then
          echo "Downloaded $f ..."
      fi
   fi
done

# Add a file to indicate we've done the download
touch .downloaded
echo "Downloads completed."
