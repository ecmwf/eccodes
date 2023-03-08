file( GLOB samples_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.tmpl" )

if(HAVE_INSTALL_ECCODES_SAMPLES)
    install( FILES ${samples_files}
             DESTINATION ${ECCODES_SAMPLES_SUFF}
             PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ )

#    # copy script installSamples.sh to samples folder
#    install( PROGRAMS installSamples.sh DESTINATION ${ECCODES_SAMPLES_SUFF} )

    if(NOT HAVE_MEMFS)
      add_custom_target(set_latest_tables_version)
      add_custom_command(TARGET set_latest_tables_version
                         POST_BUILD
                         COMMAND bash -c "cd ${CMAKE_INSTALL_PREFIX}/${ECCODES_SAMPLES_SUFF}; for F in GRIB2.tmpl *_grib2.tmpl ; do grib_set -s tablesVersion=30 $F tmp; mv tmp $F; done"
                         VERBATIM
                         WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${ECCODES_SAMPLES_SUFF}
      )
    endif()

endif()

foreach( tmpl ${samples_files} )
    list( APPEND ECCODES_SAMPLES_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${tmpl} )
endforeach()

set( ECCODES_SAMPLES_DIR   ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE )
set( ECCODES_SAMPLES_FILES ${ECCODES_SAMPLES_FILES}    PARENT_SCOPE )

# link to the samples in the build directory. See GRIB-786
file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${INSTALL_DATA_DIR} )
if( NOT EXISTS "${CMAKE_BINARY_DIR}/${ECCODES_SAMPLES_SUFF}" )
  execute_process( COMMAND "${CMAKE_COMMAND}" "-E" "create_symlink"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_BINARY_DIR}/${ECCODES_SAMPLES_SUFF}" )
endif()
