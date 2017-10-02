# examples/C/CMakeLists.txt

# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh  @ONLY )

execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/include.sh ${CMAKE_CURRENT_BINARY_DIR} )

# Build the executables used by test scripts
########################################################################
list( APPEND test_bins
      grib_nearest
      grib_set_bitmap
      grib_iterator
      grib_get_keys
      grib_print_data
      grib_set_keys
      grib_index
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
      grib_list
      grib_get_data
      sections_copy
      grib_iterator_bitmap
      large_grib1
      grib_clone
      grib_copy_message
      grib_ensemble_index
      get_product_kind
      bufr_copy_data
      bufr_attributes
      bufr_clone
      bufr_expanded
      bufr_get_keys
      bufr_get_string_array
      bufr_keys_iterator
      bufr_missing
      bufr_read_header
      bufr_read_scatterometer
      bufr_read_synop
      bufr_read_temp
      bufr_set_keys
      bufr_subset
)
foreach( tool ${test_bins} )
    ecbuild_add_executable( TARGET    c_${tool}
                            NOINSTALL
                            SOURCES   ${tool}.c
                            LIBS      eccodes
    )
endforeach()

# Now add each test (shell scripts)
########################################################################
list( APPEND tests
   grib_iterator
   grib_get_keys
   grib_print_data
   grib_set_keys
   grib_keys_iterator
   grib_multi
   grib_multi_write
   grib_precision
   grib_clone
   grib_copy_message
   grib_ensemble_index
   grib_set_pv
   grib_set_data
   grib_set_bitmap
   grib_list
   grib_get_data
   large_grib1
   set_missing
   sections_copy
   bufr_attributes
   bufr_copy_data
   bufr_clone
   bufr_expanded
   bufr_get_keys
   bufr_get_string_array
   bufr_keys_iterator
   bufr_missing
   bufr_read_header
   bufr_read_scatterometer
   bufr_read_synop
   bufr_read_temp
   bufr_set_keys
   bufr_subset
   get_product_kind
)
foreach( test ${tests} )
    ecbuild_add_test( TARGET eccodes_c_${test}
                      TYPE       SCRIPT
                      COMMAND    ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                      TEST_DEPENDS eccodes_download_gribs eccodes_download_bufrs
    )
endforeach()

########################################################################
# Tests which are conditional
if( ENABLE_EXTRA_TESTS AND HAVE_ECCODES_THREADS )
    # For POSIX threads
    LIST(APPEND pthread_tests grib_pthreads bufr_pthreads)
    foreach( test ${pthread_tests} )
        ecbuild_add_executable( TARGET    c_${test}
                                NOINSTALL
                                SOURCES   ${test}.c
                                LIBS      eccodes
        )
        ecbuild_add_test( TARGET       eccodes_c_${test}
                          TYPE         SCRIPT
                          COMMAND      ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                          TEST_DEPENDS eccodes_download_gribs eccodes_download_bufrs
        )
    endforeach()
endif()


########################################################################
# Tests with no script
ecbuild_add_test(
    TARGET    eccodes_c_new_sample
    SOURCES   new_sample.c
    LIBS      eccodes
    ARGS      "out.grib"
    ENVIRONMENT "ECCODES_SAMPLES_PATH=${PROJECT_SOURCE_DIR}/samples" "ECCODES_DEFINITION_PATH=${PROJECT_SOURCE_DIR}/definitions"
)
