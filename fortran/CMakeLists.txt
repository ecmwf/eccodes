if( GRB_API_FORTRAN )

    message( WARNING "GRIB_API Fortran bindings still not supported with CMake builds" )

  ecbuild_enable_fortran( REQUIRED )

  # add here the building of fortran bindings

  install( FILES grib_api_F77.h DESTINATION include/${PROJECT_NAME} )

endif()
