if( SWIG_FOUND )
  message( STATUS " SWIG command      : [${SWIG_EXECUTABLE}]" )
endif()

if(CMATH_FOUND)
  message( STATUS " CMath libs        : [${CMATH_LIBRARIES}]" )
endif()

if( PNG_FOUND )
  message( STATUS " PNG libs          : [${PNG_LIBRARIES}]" )
  message( STATUS "      includes     : [${PNG_INCLUDE_DIR}]" )
endif()

if( JASPER_FOUND )
  message( STATUS " Jasper libs       : [${JASPER_LIBRARIES}]" )
  message( STATUS "        includes   : [${JASPER_INCLUDE_DIR}]" )
endif()

if( OpenJPEG_FOUND )
  message( STATUS " OpenJPEG libs     : [${OPENJPEG_LIBRARIES}]" )
  message( STATUS "          includes : [${OPENJPEG_INCLUDE_DIR}]" )
endif()

if( NETCDF_FOUND )
  message( STATUS " NetCDF   libs     : [${NETCDF_LIBRARIES}]" )
  message( STATUS "          includes : [${NETCDF_INCLUDE_DIR}]" )
endif()



