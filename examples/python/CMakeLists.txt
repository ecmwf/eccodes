# examples/python/CMakeLists.txt

# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh  @ONLY )


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
   index
   grib_iterator
   grib_keys_iterator
   multi_write
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
   bufr_read_synop 
   bufr_read_temp
   bufr_set_keys
   bufr_subset
   get_product_kind
)
foreach( test ${tests} )
    ecbuild_add_test( TARGET p_${test}_test
                  TYPE       SCRIPT
                  DEPENDS    ${ptools}
                  COMMAND    ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                  CONDITION  HAVE_PYTHON
                  RESOURCES  include.sh ${test}.py
                  ENVIRONMENT PYTHON=${PYTHON_EXECUTABLE}
                  TEST_DEPENDS get_gribs get_tigge_gribs get_bufrs
    )
endforeach()
