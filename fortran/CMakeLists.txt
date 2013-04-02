if( GRB_API_FORTRAN )

  ecbuild_enable_fortran( REQUIRED )

  # add here the building of fortran bindings

  install( FILES grib_api_F77.h DESTINATION include/${PROJECT_NAME} )

endif()
