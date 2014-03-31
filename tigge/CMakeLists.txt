# tigge
ecbuild_add_executable(TARGET   tigge_check
                       SOURCES  tigge_check.c
                       INCLUDES ${GRIB_API_EXTRA_INCLUDE_DIRS}
                       LIBS     grib_api
                       )


ecbuild_add_executable(TARGET   tigge_name 
                       SOURCES  tigge_name.c
                       INCLUDES ${GRIB_API_EXTRA_INCLUDE_DIRS}
                       LIBS     grib_api
                       )

ecbuild_add_executable(TARGET   tigge_accumulations
                       SOURCES  tigge_accumulations.c
                       INCLUDES ${GRIB_API_EXTRA_INCLUDE_DIRS}
                       LIBS     grib_api
                       )

ecbuild_add_executable(TARGET   tigge_split
                       SOURCES  tigge_split.c
                       INCLUDES ${GRIB_API_EXTRA_INCLUDE_DIRS}
                       LIBS     grib_api
                       )
