# examples/F90/CMakeLists.txt

# Configure the file which all CMake tests will include
configure_file( include.ctest.sh.in  include.ctest.sh  @ONLY )


# Build the executables used by test scripts
################################################
list( APPEND tests
      index
      copy_message
      get
      get_data
      get_pl
      get_pv
      keys_iterator
      multi_write
      multi
      nearest
      precision
      print_data
      set
      set_bitmap
      set_missing
      set_pv
      samples
      count_messages
      read_message
      read_from_file
      get_set_uuid
      clone
      bufr_attributes
      bufr_clone
      bufr_expanded
      bufr_get_keys
      bufr_keys_iterator
      bufr_print_header 
      bufr_print_data
      bufr_set_keys
      bufr_subset
)

# Simplify tests: no need to build executable and then test. All in one
# TODO
# ecbuild_add_test( TARGET test_f90_samples
#   LINKER_LANGUAGE Fortran
#   SOURCES samples.f90
#   LIBS    eccodes_f90 eccodes
#   COMMAND  ${CMAKE_CURRENT_SOURCE_DIR}/samples.sh
#   CONDITION  EC_HAVE_FORTRAN
# )

foreach( tool ${tests} )

    # Build the Fortran executable
    ecbuild_add_executable( TARGET    f_${tool}
                            NOINSTALL
                            SOURCES   ${tool}.f90
                            CONDITION EC_HAVE_FORTRAN
                            LINKER_LANGUAGE Fortran
                            LIBS      eccodes_f90 eccodes
    )
    # Add the test which depends on the above executable
    ecbuild_add_test( TARGET f_${tool}_test
                      TYPE SCRIPT
                      DEPENDS f_${tool}
                      CONDITION EC_HAVE_FORTRAN
                      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${tool}.sh
                      RESOURCES include.sh
    )
endforeach()

# Executables without a shell script
# TODO
ecbuild_add_executable( TARGET    f_new_from_file
                        NOINSTALL
                        SOURCES   new_from_file.f90
                        CONDITION EC_HAVE_FORTRAN
                        LINKER_LANGUAGE Fortran
                        LIBS      eccodes_f90 eccodes
)
ecbuild_add_executable( TARGET    f_copy_namespace
                        NOINSTALL
                        SOURCES   copy_namespace.f90
                        CONDITION EC_HAVE_FORTRAN
                        LINKER_LANGUAGE Fortran
                        LIBS      eccodes_f90 eccodes
)
ecbuild_add_executable( TARGET    f_set_gvc
                        NOINSTALL
                        SOURCES   set_gvc.f90
                        CONDITION EC_HAVE_FORTRAN
                        LINKER_LANGUAGE Fortran
                        LIBS      eccodes_f90 eccodes
)


# # Note extra dependency
# ecbuild_add_test( TARGET f_set_test
#                   TYPE SCRIPT
#                   DEPENDS set_gvc set
#                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/set.sh
#                   RESOURCES include.sh
# )
# 
