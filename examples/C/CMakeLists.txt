# examples/C/CMakeLists.txt

# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh  @ONLY )


# Build the executables used by test scripts
################################################
list( APPEND test_bins
      nearest
      set_bitmap
      iterator
      get
      print_data
      set
      set_missing
      keys_iterator
      set_data
      mars_param
      values_check
      box
      multi
      multi2
      multi_write
      precision
      set_pv
      list
      get_data
      sections_copy
      iterator_bitmap
      large_grib1
      clone
      check_gaussian_grid
      ensemble_index
)
foreach( tool ${test_bins} )
    ecbuild_add_executable( TARGET    ${tool}
                            NOINSTALL
                            SOURCES   ${tool}.c
                            LIBS      grib_api
    )
endforeach()

# Now add each test
#################################################
list( APPEND tests
   iterator
   get
   print_data
   set
   keys_iterator
   multi
   multi_write
   precision
   list
   large_grib1
   get_data
   set_missing
   clone
   sections_copy
   set_pv
   check_gaussian_grids
)
foreach( test ${tests} )
    ecbuild_add_test( TARGET c_${test}
                  TYPE       SCRIPT
                  COMMAND    ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                  RESOURCES  include.sh
    )
endforeach()


#############################################
# Tests with no script
ecbuild_add_test( TARGET    c_new_sample
                  SOURCES   new_sample.c
                  LIBS      grib_api
                  ARGS      "out.grib"
                  ENVIRONMENT "GRIB_SAMPLES_PATH=${PROJECT_SOURCE_DIR}/samples" "GRIB_DEFINITION_PATH=${PROJECT_SOURCE_DIR}/definitions"
)

