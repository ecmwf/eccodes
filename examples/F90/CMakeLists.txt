# examples/F90/CMakeLists.txt

# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh  @ONLY )

list( APPEND test_bins
    bufr_attributes
    bufr_clone
    bufr_copy_data
    bufr_copy_keys
    bufr_copy_message
    bufr_ecc-1019
    bufr_ecc-1284
    bufr_expanded
    bufr_get_keys
    bufr_get_string_array
    bufr_keys_iterator
    bufr_read_header
    bufr_read_scatterometer
    bufr_read_synop
    bufr_read_temp
    bufr_read_tempf
    bufr_read_tropical_cyclone
    bufr_set_keys
    bufr_subset
    codes_datetime_julian
    codes_dump
    codes_ecc-1392
    codes_f90_misc
    codes_load_file
    codes_scan_file
    codes_set_paths
    get_native_type
    get_product_kind
    grib_clone
    grib_get_message
    grib_copy_message
    grib_copy_namespace
    grib_count_messages
    grib_count_messages_multi
    grib_ecc-1316
    grib_ecc-671
    grib_elements
    grib_get_data
    grib_get_data_real4
    grib_get_keys
    grib_get_pl
    grib_get_pv
    grib_get_set_uuid
    grib_index
    grib_keys_iterator
    grib_keys_iterator_skip
    grib_multi
    grib_multi_write
    grib_nearest
    grib_nearest_four_single
    grib_nearest_single
    grib_precision
    grib_print_data
    grib_read_bytes
    grib_read_from_file
    grib_read_message
    grib_samples
    grib_sections_copy
    grib_set_bitmap
    grib_set_data
    grib_set_data_force
    grib_set_keys
    grib_set_missing
    grib_set_pv
    key_is_computed)

foreach( tool ${test_bins} )
    ecbuild_add_executable( TARGET          f_${tool}
                            NOINSTALL
                            SOURCES         ${tool}.f90
                            LINKER_LANGUAGE Fortran
                            CONDITION       HAVE_FORTRAN
                            LIBS            eccodes_f90 eccodes )
endforeach()

# Add all the tests
####################
if( HAVE_BUILD_TOOLS )
    list( APPEND tests_sanity
          codes_ecc-1392
          codes_datetime_julian
          codes_set_paths
          codes_f90_misc
          grib_set_pv
          grib_set_data
          grib_set_data_force
          bufr_ecc-1284
          bufr_ecc-1019
          get_native_type
          key_is_computed
          grib_ecc-671 )
    list( APPEND tests_extra
          grib_index
          codes_dump
          codes_scan_file
          codes_load_file
	  grib_get_message
          grib_copy_message
          grib_sections_copy
          bufr_copy_message
          grib_get_keys
          grib_get_data
          grib_get_data_real4
          grib_get_pl
          grib_get_pv
          grib_keys_iterator
          grib_keys_iterator_skip
          grib_multi_write
          grib_multi
          grib_elements
          grib_nearest
          grib_nearest_single
          grib_nearest_four_single
          grib_precision
          grib_print_data
          grib_set_keys
          grib_set_bitmap
          grib_set_missing
          grib_samples
          grib_count_messages
          grib_count_messages_multi
          grib_copy_namespace
          grib_read_message
          grib_read_from_file
          grib_read_bytes
          grib_get_set_uuid
          grib_clone
          grib_ecc-1316
          bufr_attributes
          bufr_clone
          bufr_expanded
          bufr_get_keys
          bufr_get_string_array
          bufr_keys_iterator
          bufr_read_header
          bufr_read_scatterometer
          bufr_read_synop
          bufr_read_temp
          bufr_read_tempf
          bufr_read_tropical_cyclone
          bufr_set_keys
          bufr_copy_keys
          bufr_subset
          get_product_kind )
else()
    # No command line tools
    list( APPEND tests_sanity
          grib_set_pv
          grib_set_data
          grib_set_data_force
          codes_set_paths
          codes_f90_misc
          get_native_type
          key_is_computed
          grib_ecc-671 )
    list( APPEND tests_extra
          grib_index
          grib_get_keys
          grib_get_data
          grib_get_pl
          grib_get_pv
          grib_keys_iterator
          grib_keys_iterator_skip
          grib_multi
          grib_nearest
          grib_elements
          grib_nearest_single
          grib_nearest_four_single
          grib_precision
          grib_print_data
          grib_set_missing
          grib_samples
          grib_count_messages
          grib_count_messages_multi
          grib_read_from_file
          grib_clone
          bufr_attributes
          bufr_clone
          bufr_expanded
          bufr_get_keys
          bufr_get_string_array
          bufr_keys_iterator
          bufr_read_header
          bufr_read_scatterometer
          bufr_read_synop
          bufr_read_temp
          bufr_read_tempf
          bufr_read_tropical_cyclone
          bufr_set_keys
          bufr_subset
          get_product_kind )
endif()

foreach( atest ${tests_sanity} )
    ecbuild_add_test( TARGET          eccodes_f_${atest}
                      TYPE            SCRIPT
                      CONDITION       HAVE_FORTRAN
                      LABELS          "sanity"
                      RESOURCES       bufr_read_scatterometer_f.ref
                      COMMAND         ${CMAKE_CURRENT_SOURCE_DIR}/${atest}.sh )
endforeach()

if( HAVE_BUILD_TOOLS AND NOT CMAKE_CXX_COMPILER_ID MATCHES NVHPC )
      # Test which fails on Leonardo
      list( APPEND tests_extra bufr_copy_data )
endif()

foreach( atest ${tests_extra} )
    ecbuild_add_test( TARGET          eccodes_f_${atest}
                      CONDITION       HAVE_FORTRAN AND ENABLE_EXTRA_TESTS
                      RESOURCES       bufr_read_scatterometer_f.ref
                      COMMAND         ${CMAKE_CURRENT_SOURCE_DIR}/${atest}.sh
                      TEST_DEPENDS    eccodes_download_gribs eccodes_download_bufrs eccodes_download_bufr_refs )
endforeach()

# Test for CCSDS (AEC) packing
if( HAVE_BUILD_TOOLS AND HAVE_AEC AND HAVE_FORTRAN )
    ecbuild_add_executable( TARGET          f_grib_set_packing
                            NOINSTALL
                            SOURCES         grib_set_packing.f90
                            LINKER_LANGUAGE Fortran
                            LIBS            eccodes_f90 eccodes )

    ecbuild_add_test( TARGET   eccodes_f_grib_set_packing
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/grib_set_packing.sh )
endif()

# Executables without a shell script
#ecbuild_add_executable( TARGET    f_new_from_file
#                        NOINSTALL
#                        SOURCES   new_from_file.f90
#                        CONDITION HAVE_FORTRAN
#                        LINKER_LANGUAGE Fortran
#                        LIBS      eccodes_f90 eccodes )
ecbuild_add_executable( TARGET    f_grib_set_gvc
                        NOINSTALL
                        SOURCES   grib_set_gvc.f90
                        CONDITION HAVE_FORTRAN
                        LINKER_LANGUAGE Fortran
                        LIBS      eccodes_f90 eccodes )
ecbuild_add_executable( TARGET    f_grib_print_data_static
                        NOINSTALL
                        SOURCES   grib_print_data_static.f90
                        CONDITION HAVE_FORTRAN
                        LINKER_LANGUAGE Fortran
                        LIBS      eccodes_f90 eccodes )


# # Note extra dependency
# ecbuild_add_test( TARGET eccodes_f_set_test
#                   TYPE SCRIPT
#                   DEPENDS set_gvc set
#                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/set.sh
# )
