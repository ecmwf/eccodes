# tests/CMakeLists.txt
#
# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh @ONLY )

execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/utils.sh   ${CMAKE_CURRENT_BINARY_DIR} )
execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/number_compare.pl ${CMAKE_CURRENT_BINARY_DIR} )

# Build the executables used by test scripts
################################################
list(APPEND test_c_bins
    list_codetable_flagtable_keys
    grib_bpv_limit
    grib_double_cmp
    read_any
    julian
    grib_indexing
    grib_fieldset
    grib_multi_from_message
    grib_read_index
    unit_tests
    bufr_keys_iter
    grib_keys_iter
    grib_keys_iter_skip
    grib_geo_iter
    gauss_sub
    grib_nearest_test
    grib_util_set_spec
    grib_check_param_concepts
    grib_local_MeteoFrance
    grib_2nd_order_numValues
    grib_optimize_scaling
    grib_optimize_scaling_sh
    grib_set_force
    grib_run_length_packing
    grib_partial_message
    grib_ecc-386
    grib_ecc-1467
    grib_ecc-1431
    grib_ecc-1433
    bufr_ecc-517
    bufr_ecc-1288
    bufr_get_element
    bufr_extract_headers
    extract_offsets
    bufr_check_descriptors
    bufr_coordinate_descriptors
    codes_new_from_samples
    codes_dump_action_tree
    codes_set_samples_path
    codes_compare_keys
    codes_dump_content
    grib_sh_ieee64
    grib_ieee
    grib_set_bytes
    grib_copy_message
    grib_packing_order
    grib_unpack_subarray
    grib_sh_imag
    grib_spectral
    grib_lam_bf
    grib_lam_gp)


foreach( tool ${test_c_bins} )
    # here we use the fact that each tool has only one C file that matches its name
    ecbuild_add_executable( TARGET    ${tool}
                            NOINSTALL
                            SOURCES   ${tool}.cc
                            LIBS      eccodes )
endforeach()


# Now add each test (shell scripts)
#################################################
if( HAVE_BUILD_TOOLS )

    # These tests do not require any data downloads
    # and are generally quick
    list(APPEND tests_basic
        codes_info
        codes_deprecated
        unit_tests
        julian
        grib_dump_samples
        bufr_dump_samples
        bufr_check_descriptors
        definitions
        grib2_version
        grib_accessors
        grib_calendar
        grib_md5
        grib_cfNames
        grib_ifsParam
        grib_packing_order
        filter_substr
        grib_uerra
        grib_ecpoint
        grib_s2s
        grib_fire
        # grib_true_imagery
        grib2_wave_spectra
        grib_element
        grib_suppressed
        grib_2nd_order_numValues
        grib_ecc-136
        grib_ecc-530
        grib_ecc-922
        grib_ecc-966
        grib_ecc-967
        grib_ecc-1150
        grib_ecc-1053
        grib_ecc-1065
        grib_ecc-1167
        grib_ecc-1170
        grib_ecc-1195
        grib_ecc-1212
        grib_ecc-1230
        grib_ecc-1258
        grib_ecc-1260
        grib_ecc-1261
        grib_ecc-1271
        grib_ecc-1315
        grib_ecc-1322
        grib_ecc-1319
        grib_ecc-1406
        grib_ecc-1560
        grib_ecc-1571
        grib_ecc-1654
        bufr_ecc-1028
        bufr_ecc-1195
        bufr_ecc-1259
        bufr_ecc-1290
        bufr_ecc-1304
        bufr_ecc-1347
        bufr_ecc-1395
        bufr_json_samples
        bufr_ecc-359
        bufr_ecc-517
        bufr_rdbSubTypes
        grib_efas
        grib_sh_imag
        grib_spectral
        grib_levtype
        grib_typeOfLevel
        grib_grid_unstructured
        grib_grid_lambert_conformal
        grib_grid_polar_stereographic
        grib_grid_healpix
        grib_g1monthlydate
        grib_g1day_of_the_year_date
        grib_g1fcperiod)

    # These tests require data downloads
    # and/or take much longer
    list(APPEND tests_extra
        grib_data_quality_checks
        grib_mars_keys2)

    # These tests do not require any data downloads
    foreach( test ${tests_basic} )
        ecbuild_add_test( TARGET  eccodes_t_${test}
                          TYPE    SCRIPT
                          LABELS  "sanity"
                          COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh )
    endforeach()
    ecbuild_add_test( TARGET eccodes_t_grib_check_gaussian_grids
                      TYPE SCRIPT
                      CONDITION ECCODES_INSTALL_EXTRA_TOOLS
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_check_gaussian_grids.sh )

##For debugging on windows
ecbuild_add_test( TARGET eccodes_t_grib_to_netcdf
TYPE SCRIPT
CONDITION HAVE_NETCDF
COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_to_netcdf.sh )

    # Note: making the test dependent on the grib files (with DEPENDS)
    #       means they will be downloaded at "make" time
    #       rather than when you do "ctest". Use TEST_DEPENDS instead
    foreach( test ${tests_extra} )
        ecbuild_add_test( TARGET eccodes_t_${test}
                      TYPE SCRIPT
                      CONDITION ENABLE_EXTRA_TESTS
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                      RESOURCES asca_139.t1.ref
                      TEST_DEPENDS eccodes_download_gribs eccodes_download_tigge_gribs
                                   eccodes_download_bufrs eccodes_download_bufr_refs
                                   eccodes_download_metars eccodes_download_gts )
    endforeach()


    ###########################################
    # Note: the reference file is in the tests dir not data dir!
    ecbuild_add_test( TARGET eccodes_t_grib_grid_lamb_az_eq_area
                      TYPE SCRIPT
                      CONDITION ENABLE_EXTRA_TESTS
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_grid_lamb_az_eq_area.sh
                      RESOURCES grib_lamb_az_eq_area.ref
                      TEST_DEPENDS eccodes_download_gribs )

    # These ones are conditional
    ###########################################
    ecbuild_add_test( TARGET eccodes_t_tools_data_from_stdin
                      TYPE SCRIPT
                      CONDITION NOT ECCODES_ON_WINDOWS AND ENABLE_EXTRA_TESTS
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools_data_from_stdin.sh
                      TEST_DEPENDS eccodes_download_bufrs )

    ecbuild_add_test( TARGET eccodes_t_bufr_ecc-197
                      TYPE SCRIPT
                      CONDITION NOT ECCODES_ON_WINDOWS AND ENABLE_EXTRA_TESTS
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bufr_ecc-197.sh
                      TEST_DEPENDS eccodes_download_bufrs )
    ecbuild_add_test( TARGET eccodes_t_bufr_ecc-1288
                      TYPE SCRIPT
                      CONDITION NOT ECCODES_ON_WINDOWS AND ENABLE_EXTRA_TESTS
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bufr_ecc-1288.sh
                      TEST_DEPENDS eccodes_download_bufrs eccodes_download_bufr_refs )

else()
    # No command line tools
    list(APPEND tests_no_tools
        unit_tests
        julian
        bufr_check_descriptors
        grib_sh_imag
        grib_2nd_order_numValues
        grib_sh_ieee64)

    foreach( test ${tests_no_tools} )
    ecbuild_add_test( TARGET  eccodes_t_${test}
                      TYPE    SCRIPT
                      LABELS  "sanity"
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh )
    endforeach()

endif()
