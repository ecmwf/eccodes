# tigge

if( HAVE_EXPERIMENTAL_BUILD_WITH_CXX )
    set_source_files_properties(tigge_tools.c PROPERTIES LANGUAGE CXX)
    set_source_files_properties(tigge_check.c PROPERTIES LANGUAGE CXX)
    if( ECCODES_INSTALL_EXTRA_TOOLS )
        set_source_files_properties(tigge_name.c tigge_accumulations.c tigge_split.c PROPERTIES LANGUAGE CXX)
    endif()
endif()

# library
ecbuild_add_library(TARGET    tigge_tools
                    TYPE      STATIC
                    NOINSTALL
                    SOURCES   tigge_tools.c tigge_tools.h)

# executables
ecbuild_add_executable(TARGET   tigge_check
                       SOURCES  tigge_check.c
                       INCLUDES ${ECCODES_EXTRA_INCLUDE_DIRS}
                       LIBS     eccodes tigge_tools)

ecbuild_add_executable(TARGET     tigge_name
                       SOURCES    tigge_name.c
                       INCLUDES   ${ECCODES_EXTRA_INCLUDE_DIRS}
                       CONDITION  ECCODES_INSTALL_EXTRA_TOOLS
                       LIBS       eccodes tigge_tools)

ecbuild_add_executable(TARGET     tigge_accumulations
                       SOURCES    tigge_accumulations.c
                       INCLUDES   ${ECCODES_EXTRA_INCLUDE_DIRS}
                       CONDITION  ECCODES_INSTALL_EXTRA_TOOLS
                       LIBS       eccodes tigge_tools)

ecbuild_add_executable(TARGET     tigge_split
                       SOURCES    tigge_split.c
                       INCLUDES   ${ECCODES_EXTRA_INCLUDE_DIRS}
                       CONDITION  ECCODES_INSTALL_EXTRA_TOOLS
                       LIBS       eccodes tigge_tools)
