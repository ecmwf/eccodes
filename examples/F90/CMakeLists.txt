# examples/F90/CMakeLists.txt

# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh  @ONLY )

execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/include.sh ${CMAKE_CURRENT_BINARY_DIR} )

# Build the executables used by test scripts
################################################
list( APPEND tests
      grib_index
      grib_copy_message
      bufr_copy_message
      grib_get_keys
      grib_get_data
      get_pl
      get_pv
      grib_keys_iterator
      grib_multi_write
      grib_multi
      grib_nearest
      grib_precision
      grib_print_data
      grib_set_keys
      grib_set_bitmap
      grib_set_missing
      grib_set_pv
      grib_samples
      grib_count_messages
      grib_count_messages_multi
      read_message
      read_from_file
      get_set_uuid
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
      bufr_read_tropical_cyclone
      bufr_set_keys
      bufr_subset
      get_product_kind
)

foreach( tool ${tests} )

    ecbuild_add_test( TARGET eccodes_f_${tool}
                      SOURCES   ${tool}.f90
                      LINKER_LANGUAGE Fortran
                      LIBS      eccodes_f90 eccodes
                      CONDITION HAVE_FORTRAN
                      RESOURCES bufr_read_scatterometer_f.ref
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${tool}.sh
                      TEST_DEPENDS eccodes_download_gribs eccodes_download_bufrs
    )
endforeach()

# Executables without a shell script
# TODO
ecbuild_add_executable( TARGET    f_new_from_file
                        NOINSTALL
                        SOURCES   new_from_file.f90
                        CONDITION HAVE_FORTRAN
                        LINKER_LANGUAGE Fortran
                        LIBS      eccodes_f90 eccodes
)
ecbuild_add_executable( TARGET    f_copy_namespace
                        NOINSTALL
                        SOURCES   copy_namespace.f90
                        CONDITION HAVE_FORTRAN
                        LINKER_LANGUAGE Fortran
                        LIBS      eccodes_f90 eccodes
)
ecbuild_add_executable( TARGET    eccodes_f_grib_set_gvc
                        NOINSTALL
                        SOURCES   grib_set_gvc.f90
                        CONDITION HAVE_FORTRAN
                        LINKER_LANGUAGE Fortran
                        LIBS      eccodes_f90 eccodes
)
ecbuild_add_executable( TARGET    eccodes_f_grib_print_data_static
                        NOINSTALL
                        SOURCES   grib_print_data_static.f90
                        CONDITION HAVE_FORTRAN
                        LINKER_LANGUAGE Fortran
                        LIBS      eccodes_f90 eccodes
)


# # Note extra dependency
# ecbuild_add_test( TARGET eccodes_f_set_test
#                   TYPE SCRIPT
#                   DEPENDS set_gvc set
#                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/set.sh
# )
