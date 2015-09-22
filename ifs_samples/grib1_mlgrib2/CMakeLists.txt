set( _basedir grib1_mlgrib2 )

list( APPEND ifs_samples_${_basedir}
      gg_ml.tmpl
      gg_sfc.tmpl
      sh_ml.tmpl
      sh_sfc.tmpl
)

install( FILES ${ECCODES_SAMPLES_FILES} ${ifs_samples_${_basedir}}  
         DESTINATION ${CMAKE_INSTALL_PREFIX}/${ECCODES_IFS_SAMPLES_SUFF}/${_basedir}
         PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ )

file( COPY ${ifs_samples_${_basedir}}
      DESTINATION ${CMAKE_BINARY_DIR}/${ECCODES_IFS_SAMPLES_SUFF}/${_basedir} )
