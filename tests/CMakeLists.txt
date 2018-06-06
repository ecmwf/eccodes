# tests/CMakeLists.txt
#
# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh @ONLY )

execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/include.sh ${CMAKE_CURRENT_BINARY_DIR} )
execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/utils.sh   ${CMAKE_CURRENT_BINARY_DIR} )
execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/number_compare.pl ${CMAKE_CURRENT_BINARY_DIR} )

# Build the executables used by test scripts
################################################
list( APPEND test_bins
      grib_double_cmp
      read_any
      julian
      index
      multi_from_message
      read_index
      unit_tests
      bufr_keys_iter
      gauss_sub
      grib_util_set_spec
      local_MeteoFrance
      grib_2nd_order_numValues
      optimize_scaling
      optimize_scaling_sh
      grib_ecc-386
      bufr_ecc-517
      bufr_get_element
      sh_ieee64
      ieee
)

foreach( tool ${test_bins} )
    # here we use the fact that each tool has only one C file that matches its name
    ecbuild_add_executable( TARGET    ${tool}
                            NOINSTALL
                            SOURCES   ${tool}.c
                            LIBS      eccodes
    )
endforeach()

# Now add each test (shell scripts)
#################################################
# These tests do not require any data downloads
list( APPEND tests_no_data_reqd
    definitions
    grib_calendar
    unit_tests
    md5
    uerra
    grib_2nd_order_numValues
    julian
    bufr_ecc-359
    bufr_ecc-517
    bufr_subTypes
    grib_efas
)
# These tests do require data downloads
list( APPEND tests_data_reqd
    grib_double_cmp
    bufr_dump
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
    bufr_json
    bufr_ls
    bufr_change_edition
    bufr_keys_iter
    bufr_get_element
    bufr_wmo_tables
    bufr_ecc-197
    bufr_ecc-286
    bufr_ecc-288
    bufr_ecc-313
    bufr_ecc-616
    bufr_ecc-686
    bufr_ecc-690
    bufr_ecc-379
    bufr_ecc-393
    bufr_ecc-433
    grib_ecc-490
    bufr_ecc-556
    gts_get
    gts_ls
    gts_compare
    metar_ls
    metar_get
    metar_dump
    metar_compare
    bufr_set
    ieee
    sh_ieee64
    optimize_scaling
    optimize_scaling_sh
    grib1to2
    grib2to1
    grib1to3
    grib2to3
    grib3_templates
    badgrib
    grib_ls
    grib_filter
    grib_multi
    budg
    grib_gridType
    grib_octahedral
    grib_global
    grib_concept
    decimalPrecision
    grib_bitsPerValue
    get_fail
    missing
    grib_local
    grib_step
    grib_set
    grib_iterator
    grib_compare
    grib_level
    index
    grib_bitmap
    list
    grib_second_order
    multi_from_message
    grib_change_scanning
    grib_statistics
    tigge
    read_any
    grib_dump
    grib_dump_debug
    local_MeteoFrance
    grib_neg_fctime
    codes_split_file
)
if( HAVE_FORTRAN AND ENABLE_EXTRA_TESTS )
    list(APPEND tests_data_reqd bufr_dump_encode_fortran)
    list(APPEND tests_data_reqd bufr_dump_decode_fortran)
endif()
if( ENABLE_EXTRA_TESTS )
    list(APPEND tests_data_reqd grib_util_set_spec)
    list(APPEND tests_data_reqd grib_padding)
    list(APPEND tests_data_reqd tigge_conversions)
    list(APPEND tests_data_reqd bufr_dump_encode_C)
    list(APPEND tests_data_reqd bufr_dump_decode_C)
endif()

# These tests do not require any data downloads
foreach( test ${tests_no_data_reqd} )
    ecbuild_add_test( TARGET eccodes_t_${test}
                      TYPE SCRIPT
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
    )
endforeach()
ecbuild_add_test( TARGET eccodes_t_check_gaussian_grids
                  TYPE SCRIPT
                  CONDITION  ECCODES_INSTALL_EXTRA_TOOLS
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/check_gaussian_grids.sh
)


# Note: making the test dependent on the grib files (with DEPENDS)
#       means they will be downloaded at "make" time
#       rather than when you do "ctest". Use TEST_DEPENDS instead
foreach( test ${tests_data_reqd} )
    ecbuild_add_test( TARGET eccodes_t_${test}
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                  RESOURCES asca_139.t1.ref
                  TEST_DEPENDS eccodes_download_gribs eccodes_download_tigge_gribs
                               eccodes_download_bufrs eccodes_download_metars eccodes_download_gts
    )
endforeach()

if( HAVE_FORTRAN AND ENABLE_EXTRA_TESTS )
    set_tests_properties ( eccodes_t_bufr_dump_encode_fortran PROPERTIES TIMEOUT 2500)
endif()
if( ENABLE_EXTRA_TESTS )
    set_tests_properties ( eccodes_t_bufr_dump_encode_C PROPERTIES TIMEOUT 2500)
endif()


if( HAVE_PYTHON AND ENABLE_EXTRA_TESTS )
    list( APPEND ptests bufr_dump_encode_python bufr_dump_decode_python )
    foreach( test ${ptests} )
        ecbuild_add_test( TARGET eccodes_t_${test}
                          TYPE SCRIPT
                          COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                          ENVIRONMENT PYTHON=${PYTHON_EXECUTABLE} PYTHONPATH=${PROJECT_BINARY_DIR}/python
                          TEST_DEPENDS eccodes_download_bufrs
        )
    endforeach()
endif()

###########################################
# Note: the reference file is in the tests dir not data dir!
ecbuild_add_test( TARGET eccodes_t_lamb_az_eq_area
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/lamb_az_eq_area.sh
                  RESOURCES lamb_az_eq_area.ref
                  TEST_DEPENDS eccodes_download_gribs
)

# These ones are conditional
###########################################
if( ENABLE_EXTRA_TESTS AND HAVE_ECCODES_THREADS )
    ecbuild_add_executable( TARGET    grib_encode_pthreads
                            NOINSTALL
                            SOURCES   grib_encode_pthreads.c
                            LIBS      eccodes
    )
    ecbuild_add_test( TARGET eccodes_t_grib_encode_pthreads
                      TYPE SCRIPT
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_encode_pthreads.sh
    )
endif()

ecbuild_add_test( TARGET eccodes_t_grib_to_netcdf
                  TYPE SCRIPT
                  CONDITION HAVE_NETCDF
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_to_netcdf.sh
                  TEST_DEPENDS eccodes_download_gribs
)

ecbuild_add_test( TARGET eccodes_t_grib_jpeg
                  TYPE SCRIPT
                  CONDITION HAVE_JPEG
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_jpeg.sh
                  TEST_DEPENDS eccodes_download_gribs
)

ecbuild_add_test( TARGET eccodes_t_grib_png
                  TYPE SCRIPT
                  CONDITION HAVE_PNG
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_png.sh
                  TEST_DEPENDS eccodes_download_gribs
)

ecbuild_add_test( TARGET eccodes_t_grib_ccsds
                  TYPE SCRIPT
                  CONDITION HAVE_AEC
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_ccsds.sh
                  TEST_DEPENDS eccodes_download_gribs
)

ecbuild_add_test( TARGET eccodes_t_grib_check_param_concepts
                  TYPE SCRIPT
                  CONDITION ENABLE_EXTRA_TESTS
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_check_param_concepts.sh
)
# Performance test. Must have -DENABLE_GRIB_TIMER=ON
ecbuild_add_test( TARGET eccodes_t_grib_ecc-386
                  TYPE SCRIPT
                  CONDITION ENABLE_EXTRA_TESTS AND ENABLE_GRIB_TIMER
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_ecc-386.sh
                  TEST_DEPENDS eccodes_download_gribs
)
