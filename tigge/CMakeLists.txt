# tigge
ecbuild_add_executable(TARGET   tigge_check
                       SOURCES  tigge_check.c
                       INCLUDES ${ECCODES_EXTRA_INCLUDE_DIRS}
                       LIBS     eccodes
                       )


ecbuild_add_executable(TARGET   tigge_name 
                       SOURCES  tigge_name.c
                       INCLUDES ${ECCODES_EXTRA_INCLUDE_DIRS}
                       LIBS     eccodes
                       )

ecbuild_add_executable(TARGET   tigge_accumulations
                       SOURCES  tigge_accumulations.c
                       INCLUDES ${ECCODES_EXTRA_INCLUDE_DIRS}
                       LIBS     eccodes
                       )

ecbuild_add_executable(TARGET   tigge_split
                       SOURCES  tigge_split.c
                       INCLUDES ${ECCODES_EXTRA_INCLUDE_DIRS}
                       LIBS     eccodes
                       )
