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
   count_messages
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
   count_messages
   grib_get_keys
   grib_index
   grib_iterator
   grib_keys_iterator
   grib_multi_write
   grib_nearest
   grib_print_data
   samples
   grib_set_keys
   set_missing
   binary_message
   grib_set_bitmap
   bufr_attributes
   bufr_clone
   bufr_expanded
   bufr_get_keys
   bufr_keys_iterator
   bufr_read_header
   bufr_read_scatterometer
   bufr_read_synop 
   bufr_read_temp
   bufr_set_keys
   bufr_subset
   bufr_read_tropical_cyclone
   get_product_kind
   gts_get_keys
   metar_get_keys
)
foreach( test ${tests} )
    ecbuild_add_test( TARGET eccodes_p_${test}_test
                  TYPE       SCRIPT
                  DEPENDS    ${ptools}
                  COMMAND    ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                  CONDITION  HAVE_PYTHON
                  RESOURCES  ${test}.py
                  ENVIRONMENT PYTHON=${PYTHON_EXECUTABLE}
                  TEST_DEPENDS eccodes_get_gribs eccodes_get_tigge_gribs eccodes_get_bufrs
    )
endforeach()
