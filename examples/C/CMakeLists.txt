# examples/C/CMakeLists.txt

# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh  @ONLY )


# Build the executables used by test scripts
#############################################
list( APPEND test_bins
      grib_nearest
      grib_set_bitmap
      grib_iterator
      grib_get_keys
      grib_print_data
      grib_set_keys
      grib_index
      grib_set_missing
      grib_keys_iterator
      grib_set_data
      grib_nearest_multiple
      grib_multi
      grib_multi_write
      grib_precision
      grib_set_pv
      grib_list
      grib_get_data
      grib_sections_copy
      grib_copy_keys
      grib_iterator_bitmap
      grib_clone
      grib_copy_message
      grib_ensemble_index
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
      bufr_read_tempf
      bufr_set_keys
      bufr_subset
      mars_param
      grib_values_check
      multi2
      large_grib1
      get_product_kind)

foreach( tool ${test_bins} )
    ecbuild_add_executable( TARGET    c_${tool}
                            NOINSTALL
                            SOURCES   ${tool}.c
                            LIBS      eccodes )
endforeach()

# Now add each test (shell scripts)
####################################
if( HAVE_BUILD_TOOLS )
    # Command line tools are available
    list(APPEND tests_sanity
         grib_set_data
         large_grib1
         grib_sections_copy
         grib_copy_keys
         get_product_kind_samples)
    list(APPEND tests_extra
       grib_iterator
       grib_get_keys
       grib_print_data
       grib_set_keys
       grib_keys_iterator
       grib_multi_write
       grib_precision
       grib_clone
       grib_copy_message
       grib_ensemble_index
       grib_index
       grib_set_pv
       grib_set_bitmap
       grib_list
       grib_get_data
       grib_mars_param
       grib_nearest
       grib_nearest_multiple
       grib_multi
       grib_set_missing
       grib_values_check
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
       bufr_read_tempf
       bufr_set_keys
       bufr_subset
       get_product_kind)
else()
    # No command line tools
    list(APPEND tests_sanity
        grib_set_data
        get_product_kind_samples)
    list(APPEND tests_extra
       grib_iterator
       grib_get_keys
       grib_print_data
       grib_set_keys
       grib_keys_iterator
       grib_precision
       grib_ensemble_index
       grib_set_pv
       grib_list
       grib_get_data
       grib_nearest_multiple
       grib_multi
       grib_set_missing
       bufr_attributes
       bufr_expanded
       bufr_get_keys
       bufr_get_string_array
       bufr_keys_iterator
       bufr_missing
       bufr_read_header
       bufr_read_scatterometer
       bufr_read_synop
       bufr_read_temp
       bufr_read_tempf
       bufr_set_keys
       bufr_subset
       get_product_kind)
endif()

foreach( test ${tests_sanity} )
        ecbuild_add_test( TARGET       eccodes_c_${test}
                          TYPE         SCRIPT
                          LABELS       "sanity"
                          COMMAND      ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh )
endforeach()
foreach( test ${tests_extra} )
        ecbuild_add_test( TARGET       eccodes_c_${test}
                          TYPE         SCRIPT
                          CONDITION    ENABLE_EXTRA_TESTS
                          COMMAND      ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                          TEST_DEPENDS eccodes_download_gribs eccodes_download_bufrs eccodes_download_bufr_refs )
endforeach()

# Tests which are conditional
#############################
if( ENABLE_EXTRA_TESTS AND HAVE_ECCODES_THREADS )
    # For POSIX threads
    list(APPEND pthread_tests grib_pthreads bufr_pthreads)
    foreach( test ${pthread_tests} )
        ecbuild_add_executable( TARGET    c_${test}
                                NOINSTALL
                                SOURCES   ${test}.c
                                LIBS      eccodes ${CMAKE_THREAD_LIBS_INIT} )
        ecbuild_add_test( TARGET       eccodes_c_${test}
                          TYPE         SCRIPT
                          COMMAND      ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                          TEST_DEPENDS eccodes_download_gribs eccodes_download_bufrs )
    endforeach()
endif()

# Tests with no script
######################
ecbuild_add_test(
    TARGET    eccodes_c_new_sample
    SOURCES   new_sample.c
    LIBS      eccodes
    ARGS      "out.grib"
    ENVIRONMENT "ECCODES_SAMPLES_PATH=${PROJECT_SOURCE_DIR}/samples" "ECCODES_DEFINITION_PATH=${PROJECT_SOURCE_DIR}/definitions"
)
