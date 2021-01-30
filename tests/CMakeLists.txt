# tests/CMakeLists.txt
#
# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh @ONLY )

execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/include.sh ${CMAKE_CURRENT_BINARY_DIR} )
execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/utils.sh   ${CMAKE_CURRENT_BINARY_DIR} )
execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/number_compare.pl ${CMAKE_CURRENT_BINARY_DIR} )

# Build the executables used by test scripts
################################################
list(APPEND test_bins
    grib_bpv_limit
    grib_double_cmp
    read_any
    julian
    grib_indexing
    grib_multi_from_message
    grib_read_index
    unit_tests
    bufr_keys_iter
    gauss_sub
    grib_nearest_test
    grib_util_set_spec
    grib_local_MeteoFrance
    grib_2nd_order_numValues
    grib_optimize_scaling
    grib_optimize_scaling_sh
    grib_ecc-386
    bufr_ecc-517
    bufr_get_element
    bufr_extract_headers
    extract_offsets
    bufr_check_descriptors
    grib_sh_ieee64
    ieee
    grib_set_bytes
    grib_packing_order
    grib_sh_imag
    grib_sh_spectral_complex
    grib_lam_bf
    grib_lam_gp)

foreach( tool ${test_bins} )
    # here we use the fact that each tool has only one C file that matches its name
    ecbuild_add_executable( TARGET    ${tool}
                            NOINSTALL
                            SOURCES   ${tool}.c
                            LIBS      eccodes )
endforeach()

# Now add each test (shell scripts)
#################################################
if( HAVE_BUILD_TOOLS )

    # These tests do not require any data downloads
    list(APPEND tests_no_data_reqd
        unit_tests
        julian
        grib_dump_samples
        bufr_dump_samples
        bufr_check_descriptors
        definitions
        grib2_version
        grib_calendar
        grib_md5
        grib_cfNames
        grib_packing_order
        filter_substr
        grib_uerra
        grib_2nd_order_numValues
        grib_ecc-136
        grib_ecc-966
        grib_ecc-967
        grib_ecc-1150
        grib_ecc-1053
        grib_ecc-1065
        grib_ecc-1167
        grib_ecc-1170
        grib_ecc-1195
        bufr_ecc-1195
        bufr_json_samples
        bufr_ecc-359
        bufr_ecc-517
        bufr_rdbSubTypes
        grib_efas
        grib_sh_imag
        grib_sh_spectral_complex
        pseudo_diag
        grib_levtype
        grib_grid_unstructured
        grib_grid_lambert_conformal
        grib_grid_polar_stereographic
        grib_grid_space_view
        grib_g1fcperiod)

    # These tests do require data downloads
    list(APPEND tests_data_reqd
        grib_data_quality_checks
        grib_bpv_limit
        grib_complex
        grib_double_cmp
        grib_change_packing
        bufr_dump_data
        bufr_dump_descriptors
        bufr_dump_subset
        bufr_dump_decode_filter
        bufr_dump_encode_filter
        bufrdc_desc_ref
        bufrdc_ref
        bufr_compare
        bufr_copy
        bufr_count
        bufr_get
        bufr_filter
        bufr_filter_extract_datetime
        bufr_filter_extract_area
        bufr_json_data
        bufr_ls
        bufr_ls_json
        bufr_change_edition
        bufr_keys_iter
        bufr_get_element
        bufr_wmo_tables
        bufr_extract_headers
        extract_offsets
        bufr_ecc-673
        bufr_ecc-428
        bufr_ecc-286
        bufr_ecc-288
        bufr_ecc-313
        bufr_ecc-616
        bufr_ecc-686
        bufr_ecc-690
        bufr_ecc-379
        bufr_ecc-393
        bufr_ecc-433
        bufr_ecc-750
        bufr_ecc-765
        bufr_ecc-875
        bufr_ecc-887
        bufr_ecc-1187
        grib_ecc-490
        grib_ecc-756
        grib_ecc-806
        grib_ecc-873
        grib_ecc-600
        grib_ecc-923
        grib_ecc-979
        grib_ecc-984
        grib_ecc-1000
        grib_ecc-1001
        grib_ecc-1030
        grib_set_bytes
        bufr_ecc-556
        gts_get
        gts_ls
        gts_count
        gts_compare
        metar_ls
        metar_get
        metar_dump
        metar_compare
        bufr_set
        ieee
        grib_sh_ieee64
        grib_optimize_scaling
        grib_optimize_scaling_sh
        grib_lam_bf
        grib_lam_gp
        grib1to2
        grib2to1
        grib1to3
        grib2to3
        grib2_templates
        grib3_templates
        grib_bad
        grib_ls
        grib_ls_json
        grib_filter
        grib_multi
        grib_nearest_test
        pseudo_budg
        grib_gridType
        grib_octahedral
        grib_grid_mercator
        grib_global
        grib_concept
        grib_decimalPrecision
        grib_bitsPerValue
        grib_get_fail
        grib_missing
        grib_local
        grib_step
        grib_set
        grib_iterator
        grib_proj_string
        grib_compare
        grib_copy
        grib_level
        grib_indexing
        grib_bitmap
        grib_list
        grib_second_order
        grib_multi_from_message
        grib_change_scanning
        grib_statistics
        grib_tigge
        read_any
        grib_dump
        grib_dump_debug
        grib_dump_json
        grib_local_MeteoFrance
        grib_neg_fctime
        codes_split_file
        grib_mars_types)

    if( HAVE_FORTRAN AND ENABLE_EXTRA_TESTS )
        list(APPEND tests_data_reqd bufr_dump_encode_fortran)
        list(APPEND tests_data_reqd bufr_dump_decode_fortran)
    endif()
    if( ENABLE_EXTRA_TESTS )
        list(APPEND tests_data_reqd grib_util_set_spec)
        list(APPEND tests_data_reqd grib_padding)
        list(APPEND tests_data_reqd grib_tigge_conversions1)
        list(APPEND tests_data_reqd grib_tigge_conversions2)
        list(APPEND tests_data_reqd bufr_dump_encode_C)
        list(APPEND tests_data_reqd bufr_dump_decode_C)
    endif()

    # These tests do not require any data downloads
    foreach( test ${tests_no_data_reqd} )
        ecbuild_add_test( TARGET  eccodes_t_${test}
                          TYPE    SCRIPT
                          LABELS  "sanity"
                          COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh )
    endforeach()
    ecbuild_add_test( TARGET eccodes_t_grib_check_gaussian_grids
                      TYPE SCRIPT
                      CONDITION ECCODES_INSTALL_EXTRA_TOOLS
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_check_gaussian_grids.sh )


    # Note: making the test dependent on the grib files (with DEPENDS)
    #       means they will be downloaded at "make" time
    #       rather than when you do "ctest". Use TEST_DEPENDS instead
    foreach( test ${tests_data_reqd} )
        ecbuild_add_test( TARGET eccodes_t_${test}
                      TYPE SCRIPT
                      CONDITION ENABLE_EXTRA_TESTS
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                      RESOURCES asca_139.t1.ref
                      TEST_DEPENDS eccodes_download_gribs eccodes_download_tigge_gribs
                                   eccodes_download_bufrs eccodes_download_metars eccodes_download_gts )
    endforeach()

    if( HAVE_FORTRAN AND ENABLE_EXTRA_TESTS )
        set_tests_properties( eccodes_t_bufr_dump_encode_fortran PROPERTIES TIMEOUT 2500 )
    endif()
    if( ENABLE_EXTRA_TESTS )
        set_tests_properties( eccodes_t_bufr_dump_encode_C PROPERTIES TIMEOUT 2500 )
    endif()


    if( HAVE_PYTHON2 AND ENABLE_EXTRA_TESTS )
        list( APPEND ptests bufr_dump_encode_python bufr_dump_decode_python )
        foreach( test ${ptests} )
            ecbuild_add_test( TARGET eccodes_t_${test}
                              TYPE SCRIPT
                              COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                              ENVIRONMENT PYTHON=${PYTHON_EXECUTABLE} PYTHONPATH=${PROJECT_BINARY_DIR}/python
                              TEST_DEPENDS eccodes_download_bufrs )
        endforeach()
    endif()

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

    if( ENABLE_EXTRA_TESTS AND HAVE_ECCODES_THREADS )
        ecbuild_add_executable( TARGET    grib_encode_pthreads
                                NOINSTALL
                                SOURCES   grib_encode_pthreads.c
                                LIBS      eccodes ${CMAKE_THREAD_LIBS_INIT} )
        ecbuild_add_test( TARGET eccodes_t_grib_encode_pthreads
                          TYPE SCRIPT
                          COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_encode_pthreads.sh )

        foreach( test grib_threads_ecc-604 grib_threads_ecc-604-encode bufr_threads_ecc-604 )
            ecbuild_add_executable( TARGET    ${test}
                                    NOINSTALL
                                    SOURCES   ${test}.c
                                    LIBS      eccodes ${CMAKE_THREAD_LIBS_INIT} )
        endforeach()
        ecbuild_add_test( TARGET eccodes_t_grib_threads_ecc-604
                          TYPE SCRIPT
                          TEST_DEPENDS eccodes_download_gribs
                          COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_threads_ecc-604.sh )
        ecbuild_add_test( TARGET eccodes_t_bufr_threads_ecc-604
                          TYPE SCRIPT
                          TEST_DEPENDS eccodes_download_bufrs
                          COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bufr_threads_ecc-604.sh )
    endif()

    ecbuild_add_test( TARGET eccodes_t_grib_to_netcdf
                      TYPE SCRIPT
                      CONDITION HAVE_NETCDF AND ENABLE_EXTRA_TESTS
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_to_netcdf.sh
                      TEST_DEPENDS eccodes_download_gribs )

    ecbuild_add_test( TARGET eccodes_t_grib_jpeg
                      TYPE SCRIPT
                      CONDITION HAVE_JPEG AND ENABLE_EXTRA_TESTS
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_jpeg.sh
                      TEST_DEPENDS eccodes_download_gribs )

    ecbuild_add_test( TARGET eccodes_t_grib_png
                      TYPE SCRIPT
                      CONDITION HAVE_PNG AND ENABLE_EXTRA_TESTS
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_png.sh
                      TEST_DEPENDS eccodes_download_gribs )

    ecbuild_add_test( TARGET eccodes_t_grib_ccsds
                      TYPE SCRIPT
                      CONDITION HAVE_AEC AND ENABLE_EXTRA_TESTS
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_ccsds.sh
                      TEST_DEPENDS eccodes_download_gribs )

    ecbuild_add_test( TARGET eccodes_t_grib_check_param_concepts
                      TYPE SCRIPT
                      CONDITION ENABLE_EXTRA_TESTS
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_check_param_concepts.sh )

    ecbuild_add_test( TARGET eccodes_t_bufr_split_by_rdbSubtype
                      TYPE SCRIPT
                      CONDITION ENABLE_EXTRA_TESTS AND ECCODES_INSTALL_EXTRA_TOOLS
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bufr_split_by_rdbSubtype.sh
                      TEST_DEPENDS eccodes_download_bufrs )

    # Performance test. Must have -DENABLE_TIMER=ON
    ecbuild_add_test( TARGET eccodes_t_grib_ecc-386
                      TYPE SCRIPT
                      CONDITION ENABLE_EXTRA_TESTS AND ENABLE_TIMER
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_ecc-386.sh
                      TEST_DEPENDS eccodes_download_gribs )
else()
    # No command line tools
    list(APPEND tests_no_tools
        unit_tests
        julian
        bufr_check_descriptors
        grib_sh_imag
        grib_sh_spectral_complex
        grib_2nd_order_numValues
        grib_sh_ieee64)

    foreach( test ${tests_no_tools} )
    ecbuild_add_test( TARGET  eccodes_t_${test}
                      TYPE    SCRIPT
                      LABELS  "sanity"
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh )
    endforeach()

endif()
