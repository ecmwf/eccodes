# tools library

ecbuild_add_library( TARGET    grib_tools
                     TYPE      STATIC
                     NOINSTALL
                     SOURCES   grib_tools.c grib_options.c grib_tools.h
                     LIBS      grib_api )

# tools binaries

list( APPEND grib_tools_bins
               grib_keys grib_histogram grib_error grib_add big2gribex
               grib_debug grib_info grib_filter grib_ls grib_dump grib_merge
			   grib2ppm grib_set grib_get grib_get_data grib_copy grib_repair
			   grib_packing grib_distance grib_corruption_check
			   grib_compare parser grib_count grib_index_build 
			   gg_sub_area_check grib_moments grib_to_netcdf grib_to_json
               bufr_filter bufr_ls bufr_dump bufr_set bufr_get
               bufr_copy bufr_compare bufr_index_build
               taf_dump taf_ls taf_get taf_filter
               metar_dump metar_ls metar_get metar_filter
               )

foreach( tool ${grib_tools_bins} )
    
    # here we use the fact that each tool has only one C file that matches its name
    ecbuild_add_executable( TARGET     ${tool}
                            SOURCES    ${tool}.c
                            LIBS       grib_tools )

endforeach()

    ecbuild_add_executable( TARGET     grib_list_keys
                            SOURCES    list_keys.c
                            LIBS       grib_tools  )

# missing noinst_PROGRAMS

# grib1to2 script
configure_file( grib1to2.in  grib1to2 )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/grib1to2
         DESTINATION ${INSTALL_BIN_DIR}
         PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                     GROUP_EXECUTE GROUP_READ
                     WORLD_EXECUTE WORLD_READ )
