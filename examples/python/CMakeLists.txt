# examples/python/CMakeLists.txt

# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh  @ONLY )

# execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/include.sh ${CMAKE_CURRENT_BINARY_DIR} )

# Build the executables used by test scripts
################################################
list(APPEND test_bins
   grib_keys_iterator
   grib_print_data
   grib_iterator
   grib_count_messages
)

foreach( tool ${test_bins} )
   ecbuild_add_executable( TARGET    p_${tool}
                           NOINSTALL
                           SOURCES   ${tool}.c
                           CONDITION HAVE_PYTHON2
                           LIBS      eccodes )
   list( APPEND ptools  p_${tool} )
endforeach()


# Now add each test
################################################
if( HAVE_BUILD_TOOLS )
    list(APPEND tests_basic
       grib_set_pv
       grib_read_sample
       bufr_read_sample
       bufr_ecc-869
    )
    list(APPEND tests_extra
       grib_clone
       grib_count_messages
       grib_get_message_offset
       grib_get_keys
       grib_index
       grib_iterator
       grib_keys_iterator
       grib_multi_write
       grib_nearest
       grib_print_data
       grib_samples
       grib_set_missing
       binary_message
       grib_set_bitmap
       bufr_attributes
       bufr_clone
       bufr_copy_data
       bufr_expanded
       bufr_get_keys
       bufr_keys_iterator
       bufr_read_header
       bufr_read_scatterometer
       bufr_read_tropical_cyclone
       bufr_read_synop
       bufr_read_temp
       bufr_read_tempf
       bufr_set_keys
       bufr_subset
       get_product_kind
       gts_get_keys
       metar_get_keys
       bufr_ecc-448
    )
else()
    # No command line tools
    list(APPEND tests_basic
       grib_read_sample
       bufr_read_sample
    )
    list(APPEND tests_extra
       grib_clone
       grib_count_messages
       grib_get_keys
       grib_index
       grib_iterator
       grib_keys_iterator
       grib_multi_write
       grib_nearest
       grib_print_data
       grib_samples
       bufr_attributes
       bufr_clone
       bufr_expanded
       bufr_get_keys
       bufr_keys_iterator
       bufr_read_header
       bufr_read_scatterometer
       bufr_read_tropical_cyclone
       bufr_read_synop
       bufr_read_temp
       bufr_read_tempf
       bufr_set_keys
       bufr_subset
       get_product_kind
       gts_get_keys
       metar_get_keys
       bufr_ecc-448
    )
endif()

# Some tests require new features
# which are only there for python 2.7 onwards
if( HAVE_PYTHON2 AND PYTHON_VERSION_STRING VERSION_GREATER "2.7" )
    list( APPEND tests_extra grib_set_keys ) # Uses OrderedDict
endif()

foreach( test ${tests_basic} )
    ecbuild_add_test( TARGET eccodes_p_${test}_test
                      TYPE       SCRIPT
                      DEPENDS    ${ptools}
                      COMMAND    ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                      CONDITION  HAVE_PYTHON2
                      ENVIRONMENT PYTHON=${PYTHON_EXECUTABLE} )
endforeach()
foreach( test ${tests_extra} )
    ecbuild_add_test( TARGET eccodes_p_${test}_test
                      TYPE       SCRIPT
                      DEPENDS    ${ptools}
                      COMMAND    ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                      CONDITION  HAVE_PYTHON2 AND ENABLE_EXTRA_TESTS
                      ENVIRONMENT PYTHON=${PYTHON_EXECUTABLE}
                      TEST_DEPENDS eccodes_download_gribs eccodes_download_tigge_gribs eccodes_download_bufrs )
endforeach()

# Add test which requires input CSV file
ecbuild_add_test( TARGET eccodes_p_bufr_encode_flight_test
                  TYPE       SCRIPT
                  DEPENDS    ${ptools}
                  COMMAND    ${CMAKE_CURRENT_SOURCE_DIR}/bufr_encode_flight.sh
                  CONDITION  HAVE_PYTHON2 AND ENABLE_EXTRA_TESTS AND HAVE_BUILD_TOOLS
                  ENVIRONMENT PYTHON=${PYTHON_EXECUTABLE}
                  RESOURCES   flight_data.csv
                  TEST_DEPENDS eccodes_download_bufrs )

# Conditional tests
ecbuild_add_test( TARGET eccodes_p_grib_ccsds_test
                  TYPE       SCRIPT
                  DEPENDS    ${ptools}
                  COMMAND    ${CMAKE_CURRENT_SOURCE_DIR}/grib_ccsds.sh
                  CONDITION  HAVE_PYTHON2 AND HAVE_AEC AND ENABLE_EXTRA_TESTS
                  ENVIRONMENT PYTHON=${PYTHON_EXECUTABLE}
                  TEST_DEPENDS eccodes_download_gribs )
