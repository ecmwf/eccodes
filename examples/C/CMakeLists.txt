# examples/C/CMakeLists.txt

# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh  @ONLY )


# Build the executables used by test scripts
################################################
list( APPEND test_bins
      grib_nearest
      grib_set_bitmap
      grib_iterator
      grib_get_keys
      grib_print_data
      grib_set_keys
      set_missing
      grib_keys_iterator
      grib_set_data
      mars_param
      values_check
      box
      grib_multi
      multi2
      grib_multi_write
      grib_precision
      grib_set_pv
      list
      get_data
      sections_copy
      grib_iterator_bitmap
      large_grib1
      grib_clone
      check_gaussian_grid
      grib_ensemble_index
      get_product_kind
      bufr_attributes
      bufr_clone
      bufr_expanded
      bufr_get_keys
      bufr_keys_iterator
      bufr_missing
      bufr_read_header
      bufr_read_synop
      bufr_read_temp
      bufr_set_keys
      bufr_subset
)
foreach( tool ${test_bins} )
    ecbuild_add_executable( TARGET    ${tool}
                            NOINSTALL
                            SOURCES   ${tool}.c
                            LIBS      eccodes
    )
endforeach()

# Now add each test (shell scripts)
#################################################
list( APPEND tests
   grib_iterator
   grib_get_keys
   grib_print_data
   grib_set_keys
   grib_keys_iterator
   grib_multi
   grib_multi_write
   grib_precision
   list
   large_grib1
   get_data
   set_missing
   grib_clone
   sections_copy
   grib_set_pv
   check_gaussian_grids
   bufr_attributes
   bufr_clone
   bufr_expanded
   bufr_get_keys
   bufr_keys_iterator
   bufr_missing
   bufr_read_header
   bufr_read_synop
   bufr_read_temp
   bufr_set_keys
   bufr_subset
   get_product_kind
)
foreach( test ${tests} )
    ecbuild_add_test( TARGET c_${test}
                  TYPE       SCRIPT
                  COMMAND    ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                  RESOURCES  include.sh
                  TEST_DEPENDS get_gribs get_bufrs
    )
endforeach()


#############################################
# Tests with no script
ecbuild_add_test( TARGET    c_new_sample
                  SOURCES   new_sample.c
                  LIBS      eccodes
                  ARGS      "out.grib"
                  ENVIRONMENT "ECCODES_SAMPLES_PATH=${PROJECT_SOURCE_DIR}/samples" "ECCODES_DEFINITION_PATH=${PROJECT_SOURCE_DIR}/definitions"
)

