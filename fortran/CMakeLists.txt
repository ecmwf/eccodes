if( HAVE_FORTRAN )

    set( srcdir ${CMAKE_CURRENT_SOURCE_DIR} )
    set( bindir ${CMAKE_CURRENT_BINARY_DIR} )

    include_directories( ${srcdir} ${bindir} )

    ecbuild_add_executable( TARGET  grib_types
                            NOINSTALL
                            SOURCES grib_types.f90 grib_fortran_kinds.c )

    add_custom_command( OUTPUT  grib_kinds.h
                        COMMAND grib_types > grib_kinds.h
                        DEPENDS grib_types )

    if( ${EC_SIZEOF_INT} EQUAL ${EC_SIZEOF_LONG} )
        set( _long_int_interface    grib_f90_int.f90 )
        set( _long_int_interface_ec eccodes_f90_int.f90 )
    else()
        set( _long_int_interface    grib_f90_long_int.f90 )
        set( _long_int_interface_ec eccodes_f90_long_int.f90 )
    endif()

    if( ${EC_SIZEOF_INT} EQUAL ${EC_SIZEOF_SIZE_T} )
        set( _sizet_int_interface    grib_f90_int_size_t.f90 )
        set( _sizet_int_interface_ec eccodes_f90_int_size_t.f90 )
    else()
        set( _sizet_int_interface    grib_f90_long_size_t.f90 )
        set( _sizet_int_interface_ec eccodes_f90_long_size_t.f90 )
    endif()

    add_custom_command( OUTPUT  grib_f90.f90
                        COMMAND cat ${srcdir}/grib_f90_head.f90 ${srcdir}/${_long_int_interface} ${srcdir}/${_sizet_int_interface} ${srcdir}/grib_f90_tail.f90 > grib_f90.f90
                        DEPENDS grib_f90_head.f90 grib_f90_tail.f90 grib_kinds.h ${_long_int_interface} ${_sizet_int_interface} )
    add_custom_command( OUTPUT  eccodes_f90.f90
                        COMMAND cat ${srcdir}/eccodes_f90_head.f90 ${srcdir}/${_long_int_interface_ec} ${srcdir}/${_sizet_int_interface_ec} ${srcdir}/eccodes_f90_tail.f90 > eccodes_f90.f90
                        DEPENDS eccodes_f90_head.f90 eccodes_f90_tail.f90 grib_kinds.h ${_long_int_interface_ec} ${_sizet_int_interface_ec} )

    #ecbuild_add_library( TARGET     eccodes_f77
    #                     SOURCES    grib_fortran.c grib_f77.c
    #                     LIBS       eccodes )

    ecbuild_add_library( TARGET     eccodes_f90
                         SOURCES    grib_fortran.c grib_f90.f90 eccodes_f90.f90 grib_kinds.h
                         GENERATED  grib_f90.f90 eccodes_f90.f90
                         LIBS       eccodes )
    add_custom_command( TARGET     eccodes_f90 POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include
                        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/eccodes.mod ${CMAKE_BINARY_DIR}/include
                        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/grib_api.mod ${CMAKE_BINARY_DIR}/include
    )

    # Add the eccodes module file as well
    #set(_eccodes_mod "${CMAKE_Fortran_MODULE_DIRECTORY}/eccodes.mod" )

    #add_custom_command(
    #    OUTPUT ${_eccodes_mod}
    #    WORKING_DIRECTORY ${CMAKE_Fortran_MODULE_DIRECTORY}
    #    #COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_Fortran_MODULE_DIRECTORY}/grib_api.mod" ${_eccodes_mod}
    #    #COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_Fortran_MODULE_DIRECTORY}/grib_api.mod" ${_eccodes_mod}
    #    COMMAND "/usr/bin/sh" "-c" "if test -f GRIB_API.mod; then ln -s GRIB_API.mod ECCODES.mod; fi"
    #    COMMAND "/usr/bin/sh" "-c" "if test -f grib_api.mod; then ln -s grib_api.mod eccodes.mod; fi"
    #   DEPENDS eccodes_f90
    #    VERBATIM
    #)
    #add_custom_target(add_eccodes_mod ALL DEPENDS ${_eccodes_mod})

    ecbuild_add_resources( TARGET fortran_resources
                           PACK 
                            grib_fortran_prototypes.h grib_api_constants.h grib_api_externals.h
                            grib_api_visibility.h grib_types.f90 create_grib_f90.sh
                            grib_f90.f90.head grib_f90.f90.tail grib_f90_int.f90 grib_f90_long_int.f90
                            grib_f90_int_size_t.f90 grib_f90_long_size_t.f90

                            eccodes_visibility.h eccodes_constants.h
                            eccodes_f90.f90.head eccodes_f90.f90.tail eccodes_f90_int.f90 eccodes_f90_long_int.f90
                            eccodes_f90_int_size_t.f90 eccodes_f90_long_size_t.f90

                            same_int_long.f90 same_int_size_t.f90 grib_fortran_kinds.c )



### installation
    #install( FILES eccodes_f77.h DESTINATION ${INSTALL_INCLUDE_DIR} )

    # Install the generated .mod file
    # install( CODE "EXECUTE_PROCESS (COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR} ${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR})" )

    # Install the contents of the fortran module directory
    install( DIRECTORY ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/
                     DESTINATION ${INSTALL_INCLUDE_DIR} )

    # Install the eccodes module file as a copy of the grib api mod file.
    # Note: the mod filename can be lowercase or uppercase!
    # First try the lowercase one. Specify OPTIONAL so if it fails it carries on
    #install( FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/grib_api.mod
    #                 RENAME eccodes.mod
    #                 OPTIONAL
    #                 DESTINATION ${INSTALL_INCLUDE_DIR} )
    # Now try the uppercase mod file
    #install( FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/GRIB_API.mod
    #                 RENAME ECCODES.mod
    #                 OPTIONAL
    #                 DESTINATION ${INSTALL_INCLUDE_DIR} )

endif()

