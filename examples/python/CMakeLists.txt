# examples/python/CMakeLists.txt

# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh  @ONLY )

execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/include.sh ${CMAKE_CURRENT_BINARY_DIR} )

# Build the executables used by test scripts
################################################
list( APPEND test_bins
   grib_keys_iterator
   grib_print_data
   grib_iterator
   grib_count_messages
)

foreach( tool ${test_bins} )
   ecbuild_add_executable( TARGET    p_${tool}
                           NOINSTALL
                           SOURCES   ${tool}.c
                           CONDITION HAVE_PYTHON
                           LIBS      eccodes
   )
   list( APPEND ptools  p_${tool} )
endforeach()


# Now add each test
#################################################
list( APPEND tests
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
   grib_set_pv
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
   bufr_set_keys
   bufr_subset
   get_product_kind
   gts_get_keys
   metar_get_keys
   bufr_ecc-448
)

# The high level python test requires new features in the unittest
# which are only there for python 2.7 onwards
if( HAVE_PYTHON AND PYTHON_VERSION_STRING VERSION_GREATER "2.7" )
    ecbuild_info("Python examples: Adding test for High-level Pythonic Interface")
    list( APPEND tests high_level_api )
    list( APPEND tests grib_set_keys ) # Uses OrderedDict
endif()

foreach( test ${tests} )
    ecbuild_add_test( TARGET eccodes_p_${test}_test
                      TYPE       SCRIPT
                      DEPENDS    ${ptools}
                      COMMAND    ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                      CONDITION  HAVE_PYTHON
                      ENVIRONMENT PYTHON=${PYTHON_EXECUTABLE}
                      TEST_DEPENDS eccodes_download_gribs eccodes_download_tigge_gribs eccodes_download_bufrs
    )
endforeach()

# Add test which requires input CSV file
ecbuild_add_test( TARGET eccodes_p_bufr_encode_flight_test
                  TYPE       SCRIPT
                  DEPENDS    ${ptools}
                  COMMAND    ${CMAKE_CURRENT_SOURCE_DIR}/bufr_encode_flight.sh
                  CONDITION  HAVE_PYTHON
                  ENVIRONMENT PYTHON=${PYTHON_EXECUTABLE}
                  RESOURCES   flight_data.csv
                  TEST_DEPENDS eccodes_download_bufrs
)

# Conditional tests
ecbuild_add_test( TARGET eccodes_p_grib_ccsds_test
                  TYPE       SCRIPT
                  DEPENDS    ${ptools}
                  COMMAND    ${CMAKE_CURRENT_SOURCE_DIR}/grib_ccsds.sh
                  CONDITION  HAVE_PYTHON AND HAVE_AEC
                  ENVIRONMENT PYTHON=${PYTHON_EXECUTABLE}
                  TEST_DEPENDS eccodes_download_gribs
)
