set( _basedir grib1_mlgrib2_ieee64 )

list( APPEND ifs_samples_${_basedir}
      gg_ml.tmpl
      gg_sfc.tmpl
      sh_ml.tmpl
      sh_sfc.tmpl
)

install( FILES ${GRIB_API_SAMPLES_FILES} ${ifs_samples_${_basedir}}  
         DESTINATION share/${PROJECT_NAME}/ifs_samples/${_basedir}
         PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ )
