# tigge

# library
ecbuild_add_library(TARGET    tigge_tools
                    TYPE      STATIC
                    NOINSTALL
                    SOURCES   tigge_tools.cc tigge_tools.h)

# executables
ecbuild_add_executable(TARGET   tigge_check
                       SOURCES  tigge_check.cc
                       INCLUDES ${ECCODES_EXTRA_INCLUDE_DIRS}
                       LIBS     eccodes tigge_tools)

ecbuild_add_executable(TARGET     tigge_name
                       SOURCES    tigge_name.cc
                       INCLUDES   ${ECCODES_EXTRA_INCLUDE_DIRS}
                       CONDITION  ECCODES_INSTALL_EXTRA_TOOLS
                       LIBS       eccodes tigge_tools)

ecbuild_add_executable(TARGET     tigge_accumulations
                       SOURCES    tigge_accumulations.cc
                       INCLUDES   ${ECCODES_EXTRA_INCLUDE_DIRS}
                       CONDITION  ECCODES_INSTALL_EXTRA_TOOLS
                       LIBS       eccodes tigge_tools)

ecbuild_add_executable(TARGET     tigge_split
                       SOURCES    tigge_split.cc
                       INCLUDES   ${ECCODES_EXTRA_INCLUDE_DIRS}
                       CONDITION  ECCODES_INSTALL_EXTRA_TOOLS
                       LIBS       eccodes tigge_tools)
