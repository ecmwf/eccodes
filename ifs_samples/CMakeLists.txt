function( install_samples samples destination_install destination_build )
  set( _samples ${${samples}} )
  # To install the samples
  install( FILES  ${_samples}
           DESTINATION ${destination_install}
           PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ )

  # To symlink the samples to the build-dir for developers
  execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${destination_build} )
  foreach( _file ${_samples} )
    get_filename_component(_filename ${_file} NAME)
    execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink 
       ${_file} ${destination_build}/${_filename} )
  endforeach()
endfunction()

add_subdirectory( grib1 )

add_subdirectory( grib1_mlgrib2 )

# add_subdirectory( grib1_mlgrib2_ieee32 )

add_subdirectory( grib1_mlgrib2_ieee64 )

