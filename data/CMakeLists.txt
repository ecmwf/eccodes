# data/CMakeLists.txt
add_subdirectory(tigge)



# Download all the binary grib data from website
#
set(files_to_download
    bad.grib
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
)

foreach (f ${files_to_download})
  set(tgt data_${f})
  # Download the file and create a dummy target for it
  ecbuild_get_test_data(TARGET "${tgt}" NAME "${f}" DIRNAME ${PROJECT_NAME}/data NOCHECK)

  # Add to list of targets
  LIST(APPEND tgts "${tgt}")
endforeach()

# Copy other files - e.g. reference data, text files etc from the source data dir
LIST(APPEND other_files
    60_model_levels
    bitmap.diff
    ieee_test.good
    index.ok
    index_f90.ok
    julian.out.good
    list_points
    local.good.log
    ls.log
    multi.ok
    multi_step.txt
    no_bitmap.diff
    read_any.ok
    scan_x_regular_gg_5_7_good.dump
    scan_x_regular_ll_5_4_good.dump
    scan_x_regular_ll_5_7_good.dump
    scan_x_regular_ll_8_4_good.dump
    scan_x_regular_ll_8_7_good.dump
    scan_x_rotated_ll_5_4_good.dump
    scan_x_rotated_ll_5_7_good.dump
    scan_x_rotated_ll_8_4_good.dump
    scan_x_rotated_ll_8_7_good.dump
    scan_y_regular_ll_5_4_good.dump
    scan_y_regular_ll_5_7_good.dump
    scan_y_regular_ll_8_4_good.dump
    scan_y_regular_ll_8_7_good.dump
    scan_y_rotated_ll_5_4_good.dump
    scan_y_rotated_ll_5_7_good.dump
    scan_y_rotated_ll_8_4_good.dump
    scan_y_rotated_ll_8_7_good.dump
    spherical_model_level.grib1.good
    spherical_model_level.grib1_32.good
    statistics.out.good
    step.log
    step_grib1.filter
    step_grib1.log
    typeOfProcessedData.ok
    tigge_pf_ecmwf.grib2.ref
)

foreach( file ${other_files} )
   execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${file} ${CMAKE_CURRENT_BINARY_DIR} )
endforeach()

# debug_var(tgts)
# Another dummy target which depends on all previous targets
#add_custom_target( get_gribs ALL DEPENDS ${tgts} )
add_custom_target( get_gribs DEPENDS ${tgts} )
add_dependencies( check get_gribs )
