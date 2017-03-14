set( _basedir grib1_mlgrib2 )
set( _destination_install ${ECCODES_IFS_SAMPLES_SUFF}/${_basedir} )
set( _destination_build   ${CMAKE_BINARY_DIR}/${ECCODES_IFS_SAMPLES_SUFF}/${_basedir} )

list( APPEND ifs_samples_${_basedir}
      ${CMAKE_CURRENT_SOURCE_DIR}/gg_ml.tmpl
      ${CMAKE_CURRENT_SOURCE_DIR}/gg_sfc.tmpl
      ${CMAKE_CURRENT_SOURCE_DIR}/sh_ml.tmpl
      ${CMAKE_CURRENT_SOURCE_DIR}/sh_sfc.tmpl
      ${ECCODES_SAMPLES_FILES}
)

install_samples( ifs_samples_${_basedir} ${_destination_install} ${_destination_build} )
