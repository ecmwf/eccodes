# tests/CMakeLists.txt
#
# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh @ONLY )

# Build the executables used by test scripts
################################################
list( APPEND test_bins
      read_any
      julian
      index
      multi_from_message
      read_index
      unit_tests
      gauss_sub
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
ecbuild_add_test( TARGET t_definitions
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/definitions.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_ieee
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/ieee.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_grib1to2
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib1to2.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_grib2to1
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib2to1.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_badgrib
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/badgrib.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_ls
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/ls.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_convert
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/convert.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_filter
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/filter.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_multi
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/multi.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_budg
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/budg.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_gridType
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gridType.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_concept
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/concept.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_decimalPrecision
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/decimalPrecision.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_bitsPerValue
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bitsPerValue.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_get_fail
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/get_fail.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_missing
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/missing.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_local
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/local.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_step
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/step.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_set
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/set.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_iterator
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/iterator.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_compare
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/compare.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_level
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/level.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_index
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/index.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_bitmap
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_list
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/list.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_second_order
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/second_order.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_multi_from_message
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/multi_from_message.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_change_scanning
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/change_scanning.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_julian
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/julian.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_statistics
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/statistics.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_tigge
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tigge.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_tigge_conversions
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tigge_conversions.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET  t_read_any
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/read_any.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_padding
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/padding.sh
                  RESOURCES include.sh
)
ecbuild_add_test( TARGET t_debug
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/debug.sh
                  RESOURCES include.sh
)

ecbuild_add_test( TARGET t_unit_tests
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/unit_tests.sh
                  RESOURCES include.sh
)

###########################################
# Note: the reference file is in the tests dir not data dir!
ecbuild_add_test( TARGET t_lamb_az_eq_area
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/lamb_az_eq_area.sh
                  RESOURCES include.sh lamb_az_eq_area.ref
)

if ( ENABLE_NETCDF )
    ecbuild_add_test( TARGET t_grib_to_netcdf
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_to_netcdf.sh
                  RESOURCES include.sh
    )
endif()

if( ENABLE_JPG )
    ecbuild_add_test( TARGET t_jpeg
                      TYPE SCRIPT
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/jpeg.sh
                      RESOURCES include.sh
    )
endif()
