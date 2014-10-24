# examples/python/CMakeLists.txt

# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh  @ONLY )


# Build the executables used by test scripts
################################################
list( APPEND test_bins
   keys_iterator
   print_data
   iterator
   count_messages
)
foreach( tool ${test_bins} )
   ecbuild_add_executable( TARGET    p_${tool}
                           NOINSTALL
                           SOURCES   ${tool}.c
                           CONDITION HAVE_PYTHON
                           LIBS      grib_api
   )
   list( APPEND ptools  p_${tool} )
endforeach()


# Now add each test
#################################################
list( APPEND tests
   clone
   count_messages
   get
   index
   iterator
   keys_iterator
   multi_write
   nearest
   print_data
   samples
   set
   set_missing
   binary_message
   set_bitmap
)
foreach( test ${tests} )
    ecbuild_add_test( TARGET p_${test}_test
                  TYPE       SCRIPT
                  DEPENDS    ${ptools}
                  COMMAND    ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                  CONDITION  HAVE_PYTHON
                  RESOURCES  include.sh ${test}.py
                  ENVIRONMENT PYTHON=${PYTHON_EXECUTABLE}
    )
endforeach()
