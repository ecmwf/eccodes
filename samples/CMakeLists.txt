file( GLOB samples_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.tmpl" )

if(HAVE_INSTALL_ECCODES_SAMPLES)
    install( FILES ${samples_files}
             DESTINATION ${ECCODES_SAMPLES_SUFF}
             PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ )
    # set to the newest GRIB2 tablesVersion
    if(NOT HAVE_MEMFS)
      set(GRB2TABVERS '30')
      add_custom_target(set_grib2_latest_tables_version)
      add_custom_command(TARGET set_grib2_latest_tables_version
                         POST_BUILD
                         COMMAND bash -c "cd ${CMAKE_INSTALL_PREFIX}/${ECCODES_SAMPLES_SUFF}; for F in GRIB2.tmpl *_grib2.tmpl ; do grib_set -s tablesVersion=${GRB2TABVERS} $F tmp; mv tmp $F; done"
                         VERBATIM
                         WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${ECCODES_SAMPLES_SUFF}
      )
      add_custom_command(TARGET set_grib2_latest_tables_version
                         POST_BUILD
                         COMMAND bash -c "cd ${CMAKE_INSTALL_PREFIX}/share/eccodes/ifs_samples/grib1_mlgrib2; for F in *_ml.tmpl ; do grib_set -s tablesVersion=${GRB2TABVERS} $F tmp; mv tmp $F; done"
                         VERBATIM
                         WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${ECCODES_SAMPLES_SUFF}
      )
      add_custom_command(TARGET set_grib2_latest_tables_version
                         POST_BUILD
                         COMMAND bash -c "cd ${CMAKE_INSTALL_PREFIX}/share/eccodes/ifs_samples/grib1_mlgrib2_ieee64; for F in *_ml.tmpl ; do grib_set -s tablesVersion=${GRB2TABVERS} $F tmp; mv tmp $F; done"
                         VERBATIM
                         WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${ECCODES_SAMPLES_SUFF}
      )
      if(HAVE_AEC)
        add_custom_command(TARGET set_grib2_latest_tables_version
                           POST_BUILD
                           COMMAND bash -c "cd ${CMAKE_INSTALL_PREFIX}/share/eccodes/ifs_samples/grib1_mlgrib2_ccsds; for F in *_ml.tmpl gg_sfc_grib2.tmpl ; do grib_set -s tablesVersion=${GRB2TABVERS} $F tmp; mv tmp $F; done"
                           VERBATIM
                           WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${ECCODES_SAMPLES_SUFF}
        )
     endif()
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
