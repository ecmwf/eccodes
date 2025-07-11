#
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#

list( APPEND ecc_tools_sources
             grib_tools.cc
             grib_options.cc
             grib_tools.h )

if( EC_OS_NAME MATCHES "windows" )
  list( APPEND ecc_tools_sources wingetopt.cc )
endif()

# tools library
ecbuild_add_library( TARGET    ecc_tools
                     TYPE      STATIC
                     NOINSTALL
                     SOURCES   ${ecc_tools_sources}
                     PRIVATE_LIBS      eccodes )
if( eccodes_HAVE_ECKIT_GEO )
    target_link_libraries( ecc_tools PUBLIC eckit )
endif()

# tools binaries
list( APPEND ecc_tools_binaries
             codes_info codes_count codes_split_file codes_export_resource
             grib_histogram grib_filter grib_ls grib_dump
             grib2ppm grib_set grib_get grib_get_data grib_copy
             grib_compare codes_parser grib_index_build bufr_index_build
             bufr_ls bufr_dump bufr_set bufr_get
             bufr_copy bufr_compare
             gts_get gts_compare gts_copy gts_dump gts_filter gts_ls
             metar_dump metar_ls metar_compare metar_get metar_filter metar_copy )

list( APPEND ecc_tools_binaries_extra
             grib_repair
             grib_to_json
             grib_check_gaussian_grid
             bufr_split_by_rdbSubtype )

# Install generic tools
foreach( tool ${ecc_tools_binaries} )
    # here we use the fact that each tool has only one C file that matches its name
    ecbuild_add_executable( TARGET     ${tool}
                            SOURCES    ${tool}.cc
                            LIBS       ecc_tools )
endforeach()

# Install extra tools
# User must run cmake with -DECCODES_INSTALL_EXTRA_TOOLS=ON
foreach( tool ${ecc_tools_binaries_extra} )
    ecbuild_add_executable( TARGET     ${tool}
                            SOURCES    ${tool}.cc
                            CONDITION  ECCODES_INSTALL_EXTRA_TOOLS
                            LIBS       ecc_tools )
endforeach()


# grib_count/bufr_count etc. Same source code, different executable names
ecbuild_add_executable( TARGET     grib_count
                        SOURCES    codes_count.cc
                        LIBS       ecc_tools )
ecbuild_add_executable( TARGET     bufr_count
                        SOURCES    codes_count.cc
                        LIBS       ecc_tools )
ecbuild_add_executable( TARGET     gts_count
                        SOURCES    codes_count.cc
                        LIBS       ecc_tools )

# grib to netcdf is optional
ecbuild_add_executable( TARGET     grib_to_netcdf
                        SOURCES    grib_to_netcdf.cc
                        CONDITION  HAVE_NETCDF
                        LIBS       ecc_tools NetCDF::NetCDF_C )


ecbuild_add_executable( TARGET     grib_list_keys
                        SOURCES    list_keys.cc
                        CONDITION  ECCODES_INSTALL_EXTRA_TOOLS
                        LIBS       ecc_tools )

ecbuild_add_executable( TARGET     codes_bufr_filter
                        SOURCES    bufr_filter.cc
                        LIBS       ecc_tools )


if( ECCODES_INSTALL_EXTRA_TOOLS )
    ecbuild_info("ECCODES_INSTALL_EXTRA_TOOLS enabled")
else()
    ecbuild_info("ECCODES_INSTALL_EXTRA_TOOLS disabled")
endif()
########################################

configure_file( codes_config.in ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/codes_config @ONLY )
file(COPY ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/codes_config
  DESTINATION ${CMAKE_BINARY_DIR}/bin
  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
  GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install( FILES ${CMAKE_BINARY_DIR}/bin/codes_config
         DESTINATION ${INSTALL_BIN_DIR}
         PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                     GROUP_EXECUTE GROUP_READ
                     WORLD_EXECUTE WORLD_READ )


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

