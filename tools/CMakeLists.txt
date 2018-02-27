#
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#

# tools library
ecbuild_add_library( TARGET    grib_tools
                     TYPE      STATIC
                     NOINSTALL
                     SOURCES   grib_tools.c grib_options.c grib_tools.h
                     LIBS      eccodes )

# tools binaries
list( APPEND grib_tools_bins
             codes_info codes_count codes_split_file
             grib_histogram grib_filter grib_ls grib_dump grib_merge
             grib2ppm grib_set grib_get grib_get_data grib_copy
             grib_compare codes_parser grib_index_build
             bufr_ls bufr_dump bufr_set bufr_get
             bufr_copy bufr_compare bufr_index_build
             gts_get gts_compare gts_copy gts_dump gts_filter gts_ls
             metar_dump metar_ls metar_compare metar_get metar_filter metar_copy
             )

list( APPEND grib_tools_bins_EXTRA
             big2gribex
             gg_sub_area_check
             grib_repair
             grib_to_json
             grib_check_gaussian_grid
             )

# Install generic tools
foreach( tool ${grib_tools_bins} )
    # here we use the fact that each tool has only one C file that matches its name
    ecbuild_add_executable( TARGET     ${tool}
                            SOURCES    ${tool}.c
                            LIBS       grib_tools )
endforeach()

# Install extra tools
# User must run cmake with -DECCODES_INSTALL_EXTRA_TOOLS=ON
foreach( tool ${grib_tools_bins_EXTRA} )
    ecbuild_add_executable( TARGET     ${tool}
                            SOURCES    ${tool}.c
                            CONDITION  ECCODES_INSTALL_EXTRA_TOOLS
                            LIBS       grib_tools )
endforeach()

# grib_count/bufr_count tools. Same source code, different executable names
ecbuild_add_executable( TARGET     grib_count
                        SOURCES    codes_count.c
                        LIBS       grib_tools  )
ecbuild_add_executable( TARGET     bufr_count
                        SOURCES    codes_count.c
                        LIBS       grib_tools  )

# grib to netcdf is optional
ecbuild_add_executable( TARGET     grib_to_netcdf
                        SOURCES    grib_to_netcdf.c
                        INCLUDES   ${NETCDF_INCLUDE_DIRS}
                        CONDITION  HAVE_NETCDF
                        LIBS       grib_tools ${NETCDF_LIBRARIES})


ecbuild_add_executable( TARGET     grib_list_keys
                        SOURCES    list_keys.c
                        CONDITION  ECCODES_INSTALL_EXTRA_TOOLS
                        LIBS       grib_tools  )

ecbuild_add_executable( TARGET     codes_bufr_filter
                        SOURCES    bufr_filter.c
                        LIBS       grib_tools  )


# grib1to2 script needs to be generated before installation
if (ECCODES_INSTALL_EXTRA_TOOLS)
    configure_file( grib1to2.in  grib1to2 )
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/grib1to2
             DESTINATION ${INSTALL_BIN_DIR}
             PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                         GROUP_EXECUTE GROUP_READ
                         WORLD_EXECUTE WORLD_READ )
endif()
########################################

# BUFR compare script
install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/bufr_compare_dir
         DESTINATION ${INSTALL_BIN_DIR}
         PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                     GROUP_EXECUTE GROUP_READ
                     WORLD_EXECUTE WORLD_READ )


# bufr_filter script (See ECC-205)
execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/tools/bufr_filter
    ${CMAKE_BINARY_DIR}/bin/ )

install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/bufr_filter
         DESTINATION ${INSTALL_BIN_DIR}
         PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                     GROUP_EXECUTE GROUP_READ
                     WORLD_EXECUTE WORLD_READ )

