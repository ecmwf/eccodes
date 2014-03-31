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
)
foreach( tool ${test_bins} )
    # here we use the fact that each tool has only one C file that matches its name
    ecbuild_add_executable( TARGET    ${tool}
                            NOINSTALL
                            SOURCES   ${tool}.c
                            LIBS      grib_api
    )
endforeach()

# Now add each test
#################################################
ecbuild_add_test( TARGET c_iterator
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/iterator.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET c_get
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/get.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET c_print_data
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/print_data.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET c_set
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/set.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET c_keys_iterator
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/keys_iterator.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET c_multi
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/multi.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET c_multi_write
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/multi_write.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET c_precision
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/precision.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET c_list
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/list.sh
                  RESOURCES include.sh
)

ecbuild_add_test( TARGET c_large_grib1
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/large_grib1.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET c_get_data
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/get_data.sh
                  RESOURCES include.sh
)

ecbuild_add_test( TARGET c_set_missing
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/set_missing.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET c_clone
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/clone.sh
                  RESOURCES include.sh
)

ecbuild_add_test( TARGET c_sections_copy
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/sections_copy.sh
                  RESOURCES include.sh
)


#############################################
# Tests with no script
ecbuild_add_test( TARGET    c_new_sample
                  SOURCES   new_sample.c
                  LIBS      grib_api
                  ARGS      "out.grib"
                  ENVIRONMENT "GRIB_SAMPLES_PATH=${PROJECT_SOURCE_DIR}/samples" "GRIB_DEFINITION_PATH=${PROJECT_SOURCE_DIR}/definitions" )

