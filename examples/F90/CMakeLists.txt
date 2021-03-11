# examples/F90/CMakeLists.txt

# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh  @ONLY )

execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/include.sh ${CMAKE_CURRENT_BINARY_DIR} )

# Add all the tests
#############################################
if( HAVE_BUILD_TOOLS )
    list( APPEND tests_sanity
          grib_set_pv
          grib_set_data
          grib_ecc-671 )
    list( APPEND tests_extra
          grib_index
          grib_copy_message
          bufr_copy_message
          grib_get_keys
          grib_get_data
          grib_get_pl
          grib_get_pv
          grib_keys_iterator
          grib_multi_write
          grib_multi
          grib_nearest
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
          read_from_file
          grib_get_set_uuid
          grib_clone
          bufr_attributes
          bufr_copy_data
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
          grib_ecc-671 )
    list( APPEND tests_extra
          grib_index
          grib_get_keys
          grib_get_data
          grib_get_pl
          grib_get_pv
          grib_keys_iterator
          grib_multi
          grib_nearest
          grib_precision
          grib_print_data
          grib_set_missing
          grib_samples
          grib_count_messages
          grib_count_messages_multi
          read_from_file
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

foreach( tool ${tests_sanity} )
    ecbuild_add_test( TARGET          eccodes_f_${tool}
                      SOURCES         ${tool}.f90
                      LINKER_LANGUAGE Fortran
                      LIBS            eccodes_f90 eccodes
                      CONDITION       HAVE_FORTRAN
                      LABELS          "sanity"
                      RESOURCES       bufr_read_scatterometer_f.ref
                      COMMAND         ${CMAKE_CURRENT_SOURCE_DIR}/${tool}.sh )
endforeach()
foreach( tool ${tests_extra} )
    ecbuild_add_test( TARGET          eccodes_f_${tool}
                      SOURCES         ${tool}.f90
                      LINKER_LANGUAGE Fortran
                      LIBS            eccodes_f90 eccodes
                      CONDITION       HAVE_FORTRAN AND ENABLE_EXTRA_TESTS
                      RESOURCES       bufr_read_scatterometer_f.ref
                      COMMAND         ${CMAKE_CURRENT_SOURCE_DIR}/${tool}.sh
                      TEST_DEPENDS    eccodes_download_gribs eccodes_download_bufrs )
endforeach()

# Test for CCSDS (AEC) packing
ecbuild_add_test( TARGET          eccodes_f_grib_set_packing
                  SOURCES         grib_set_packing.f90
                  LINKER_LANGUAGE Fortran
                  LIBS            eccodes_f90 eccodes
                  CONDITION       HAVE_AEC AND HAVE_FORTRAN
                  COMMAND         ${CMAKE_CURRENT_SOURCE_DIR}/grib_set_packing.sh )

# Executables without a shell script
# TODO
ecbuild_add_executable( TARGET    eccodes_f_new_from_file
                        NOINSTALL
                        SOURCES   new_from_file.f90
                        CONDITION HAVE_FORTRAN
                        LINKER_LANGUAGE Fortran
                        LIBS      eccodes_f90 eccodes )
ecbuild_add_executable( TARGET    eccodes_f_grib_set_gvc
                        NOINSTALL
                        SOURCES   grib_set_gvc.f90
                        CONDITION HAVE_FORTRAN
                        LINKER_LANGUAGE Fortran
                        LIBS      eccodes_f90 eccodes )
ecbuild_add_executable( TARGET    eccodes_f_grib_print_data_static
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
