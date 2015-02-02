# tools library
ecbuild_add_library( TARGET    grib_tools
                     TYPE      STATIC
                     NOINSTALL
                     SOURCES   grib_tools.c grib_options.c grib_tools.h
                     LIBS      grib_api )

# tools binaries
list( APPEND grib_tools_bins
             codes_info codes_count
             grib_histogram grib_filter grib_ls grib_dump grib_merge
             grib2ppm grib_set grib_get grib_get_data grib_copy
             grib_compare parser grib_index_build grib_to_netcdf
             bufr_filter bufr_ls bufr_dump bufr_set bufr_get
             bufr_copy bufr_compare bufr_index_build
             taf_dump taf_ls taf_get taf_filter
             metar_dump metar_ls metar_get metar_filter
             )

list( APPEND grib_tools_bins_ECMWF_ONLY
             big2gribex
             gg_sub_area_check
             grib_repair
             grib_to_json
             )

# Install generic tools
foreach( tool ${grib_tools_bins} )
    # here we use the fact that each tool has only one C file that matches its name
    ecbuild_add_executable( TARGET     ${tool}
                            SOURCES    ${tool}.c
                            LIBS       grib_tools )
endforeach()

# Install tools specific to ECMWF
# User must run cmake with -DECCODES_INSTALL_ECMWF_TOOLS
########################################
foreach( tool ${grib_tools_bins_ECMWF_ONLY} )
    ecbuild_add_executable( TARGET     ${tool}
                            SOURCES    ${tool}.c
                            CONDITION  ECCODES_INSTALL_ECMWF_TOOLS
                            LIBS       grib_tools )
endforeach()

# Count tools. Same source code, different executable names
ecbuild_add_executable( TARGET     grib_count
                        SOURCES    codes_count.c
                        LIBS       grib_tools  )
ecbuild_add_executable( TARGET     bufr_count
                        SOURCES    codes_count.c
                        LIBS       grib_tools  )

# grib_list_keys
ecbuild_add_executable( TARGET     grib_list_keys
                        SOURCES    list_keys.c
                        CONDITION  ECCODES_INSTALL_ECMWF_TOOLS
                        LIBS       grib_tools  )

# grib1to2 script needs to be generated before installation
if (ECCODES_INSTALL_ECMWF_TOOLS)
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

