if( ENABLE_FORTRAN )

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
        set( _long_int_interface grib_f90_int.f90 )
    else()
        set( _long_int_interface grib_f90_long_int.f90 )
    endif()

    if( ${EC_SIZEOF_INT} EQUAL ${EC_SIZEOF_SIZE_T} )
        set( _sizet_int_interface grib_f90_int_size_t.f90 )
    else()
        set( _sizet_int_interface grib_f90_long_size_t.f90 )
    endif()

    add_custom_command( OUTPUT  grib_f90.f90
                        COMMAND cat ${srcdir}/grib_f90_head.f90 ${srcdir}/${_long_int_interface} ${srcdir}/${_sizet_int_interface} ${srcdir}/grib_f90_tail.f90 > grib_f90.f90
                        DEPENDS grib_f90_head.f90 grib_f90_tail.f90 grib_kinds.h ${_long_int_interface} ${_sizet_int_interface} )

    ecbuild_add_library( TARGET     grib_api_f77
                         SOURCES    grib_fortran.c grib_f77.c
                         LIBS       grib_api )

    ecbuild_add_library( TARGET     grib_api_f90
                         SOURCES    grib_fortran.c grib_f90.f90 grib_kinds.h
                         GENERATED  grib_f90.f90
                         LIBS       grib_api )

    ecbuild_add_resources( TARGET fortran_resources
                           PACK 
                            grib_fortran_prototypes.h grib_api_constants.h grib_api_externals.h
                            grib_api_visibility.h grib_types.f90 create_grib_f90.sh
                            grib_f90.f90.head grib_f90.f90.tail grib_f90_int.f90 grib_f90_long_int.f90
                            grib_f90_int_size_t.f90 grib_f90_long_size_t.f90
                            same_int_long.f90 same_int_size_t.f90 grib_fortran_kinds.c )

    install( FILES grib_api_f77.h DESTINATION ${INSTALL_INCLUDE_DIR} )

    install( CODE "EXECUTE_PROCESS (COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR} ${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR})" )

endif()

