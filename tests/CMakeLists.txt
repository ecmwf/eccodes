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
      gauss_sub
      grib_util_set_spec
      local_MeteoFrance
      grib_2nd_order_numValues
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
list( APPEND tests1
    definitions
    calendar
    unit_tests
    md5
    uerra
    grib_2nd_order_numValues
)
list( APPEND tests2
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
    ecc-197
    ecc-286
    ecc-288
    gts_get
    gts_ls
    gts_compare
    metar_ls
    metar_get
    metar_dump
    metar_compare
    bufr_set
    ieee
    grib1to2
    grib2to1
    badgrib
    ls
    filter
    multi
    budg
    gridType
    octahedral
    global
    concept
    decimalPrecision
    bitsPerValue
    get_fail
    missing
    local
    step
    set
    iterator
    grib_compare
    level
    index
    bitmap
    list
    second_order
    multi_from_message
    change_scanning
    julian
    statistics
    tigge
    tigge_conversions
    read_any
    padding
    grib_dump
    grib_dump_debug
    grib_util_set_spec
    local_MeteoFrance
    neg_fctime
)
if( HAVE_FORTRAN AND ENABLE_EXTRA_TESTS )
    list(APPEND tests2 bufr_dump_encode_fortran)
    list(APPEND tests2 bufr_dump_decode_fortran)
endif()
if( ENABLE_EXTRA_TESTS )
    list(APPEND tests2 bufr_dump_encode_C)
    list(APPEND tests2 bufr_dump_decode_C)
endif()

# These tests do not require any data downloads
foreach( test ${tests1} )
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
foreach( test ${tests2} )
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
ecbuild_add_test( TARGET eccodes_t_grib_to_netcdf
                  TYPE SCRIPT
                  CONDITION HAVE_NETCDF
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_to_netcdf.sh
                  TEST_DEPENDS eccodes_download_gribs
)

ecbuild_add_test( TARGET eccodes_t_jpeg
                  TYPE SCRIPT
                  CONDITION HAVE_JPEG
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/jpeg.sh
                  TEST_DEPENDS eccodes_download_gribs
)

ecbuild_add_test( TARGET eccodes_t_grib_png
                  TYPE SCRIPT
                  CONDITION HAVE_PNG
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_png.sh
                  TEST_DEPENDS eccodes_download_gribs
)

ecbuild_add_test( TARGET eccodes_t_ccsds
                  TYPE SCRIPT
                  CONDITION HAVE_AEC
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/ccsds.sh
                  TEST_DEPENDS eccodes_download_gribs
)

ecbuild_add_test( TARGET eccodes_t_grib_check_param_concepts
                  TYPE SCRIPT
                  CONDITION ENABLE_EXTRA_TESTS
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_check_param_concepts.sh
)

