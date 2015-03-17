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
                            LIBS      eccodes
    )
endforeach()

# Now add each test (shell scripts)
#################################################
list( APPEND tests1
    definitions
    calendar
    unit_tests
)
list( APPEND tests2
    bufr_dump
    bufrdc_desc_ref
    bufrdc_ref
    bufr_compare
    bufr_copy
    bufr_count
    bufr_get
    bufr_filter
    bufr_json
    bufr_ls
    gts_ls
    bufr_set
    ieee
    grib1to2
    grib2to1
    badgrib
    ls
    filter
    multi
    budg
    gridType
    concept
    decimalPrecision
    bitsPerValue
    get_fail
    missing
    local
    step
    set
    iterator
    compare
    level
    index
    bitmap
    list
    second_order
    multi_from_message
    change_scanning
    julian
    statistics
    tigge
    tigge_conversions
    read_any
    padding
    debug
)

# These tests do not require any data downloads
foreach( test ${tests1} )
    ecbuild_add_test( TARGET t_${test}
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                  RESOURCES include.sh number_compare.pl
    )
endforeach()

# Note: making the test dependent on the grib files (with DEPENDS)
#       means they will be downloaded at "make" time
#       rather than when you do "make test". Use TEST_DEPENDS instead
foreach( test ${tests2} )
    ecbuild_add_test( TARGET t_${test}
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${test}.sh
                  RESOURCES include.sh number_compare.pl
                  TEST_DEPENDS get_gribs get_tigge_gribs get_bufrs
    )
endforeach()

###########################################
# Note: the reference file is in the tests dir not data dir!
ecbuild_add_test( TARGET t_lamb_az_eq_area
                  TYPE SCRIPT
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/lamb_az_eq_area.sh
                  RESOURCES include.sh lamb_az_eq_area.ref
                  TEST_DEPENDS get_gribs
)

# These ones are conditional
ecbuild_add_test( TARGET t_grib_to_netcdf
                  TYPE SCRIPT
                  CONDITION HAVE_NETCDF
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_to_netcdf.sh
                  RESOURCES include.sh
                  TEST_DEPENDS get_gribs
)

ecbuild_add_test( TARGET t_jpeg
                  TYPE SCRIPT
                  CONDITION HAVE_JPEG
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/jpeg.sh
                  RESOURCES include.sh
                  TEST_DEPENDS get_gribs
)

ecbuild_add_test( TARGET t_ccsds
                  TYPE SCRIPT
                  CONDITION HAVE_AEC
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/ccsds.sh
                  RESOURCES include.sh
                  TEST_DEPENDS get_gribs
)

