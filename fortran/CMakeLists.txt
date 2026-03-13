if( HAVE_FORTRAN )

    configure_file( eccodes_settings.h.in  eccodes_settings.h  @ONLY )

    set( srcdir ${CMAKE_CURRENT_SOURCE_DIR} )
    set( bindir ${CMAKE_CURRENT_BINARY_DIR} )

    include_directories( ${srcdir} ${bindir} )

    # Our code is compiled with '-fallow-argument-mismatch' when compiling with
    # gfortran. The manual has to say this about it:
    #
    # > Some code contains calls to external procedures with mismatches between
    # > the calls and the procedure definition, or with mismatches between
    # > different calls. Such code is nonconforming, and is usually flagged with
    # > an error. This options degrades the error to a warning that can only be
    # > disabled by disabling all warnings via -w. Only a single occurrence per
    # > argument is flagged by this warning. -fallow-argument-mismatch is implied
    # > by -std=legacy.
    # >
    # > Using this option is strongly discouraged. It is possible to provide
    # > standard-conforming code that allows different types of arguments by
    # > using an explicit interface and TYPE(*).
    # From: https://gcc.gnu.org/onlinedocs/gcc-15.2.0/gfortran/Fortran-Dialect-Options.html
    #
    # Code in grib_types.f90, grib_f90.f90 is emitting this warning and for now we
    # want to silence it.
    # See https://jira.ecmwf.int/browse/ECC-2148
    if(CMAKE_Fortran_COMPILER_ID MATCHES "GNU")
        set_source_files_properties(
            grib_types.f90
            grib_f90.f90
            PROPERTIES
                COMPILE_FLAGS "-w"
        )
    endif()

    ecbuild_add_executable( TARGET  grib_types
                            NOINSTALL
                            SOURCES grib_types.f90 grib_fortran_kinds.c )

    add_custom_command( OUTPUT  grib_kinds.h
                        COMMAND grib_types > grib_kinds.h
                        DEPENDS grib_types )

    if( ${ECCODES_SIZEOF_INT} EQUAL ${ECCODES_SIZEOF_LONG} )
        set( _long_int_interface    grib_f90_int.f90 )
        set( _long_int_interface_ec eccodes_f90_int.f90 )
    else()
        set( _long_int_interface    grib_f90_long_int.f90 )
        set( _long_int_interface_ec eccodes_f90_long_int.f90 )
    endif()

    if( ${ECCODES_SIZEOF_INT} EQUAL ${ECCODES_SIZEOF_SIZE_T} )
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


    # Note: $<BUILD_INTERFACE:...> will be present only while building (for the whole bundle),
    # whereas $<INSTALL_INTERFACE:...> is only present once you install the package / bundle
    ecbuild_add_library( TARGET          eccodes_f90
                         SOURCES         grib_fortran.cc grib_f90.f90 eccodes_f90.f90 grib_kinds.h
                         GENERATED       grib_f90.f90 eccodes_f90.f90
                         PUBLIC_INCLUDES $<BUILD_INTERFACE:${CMAKE_Fortran_MODULE_DIRECTORY}>
                                         $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>
                         PRIVATE_LIBS    eccodes ${ECCODES_PTHREADS_LIBRARIES} )

    set(_eccodes_mod_candidates eccodes.mod ECCODES.mod grib_api.mod GRIB_API.mod)

    file( GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/eccodes_f90_copy_mod.cmake
          CONTENT  "file( MAKE_DIRECTORY \${MOD_DESTINATION} )
                    foreach( _mod_candidate IN ITEMS ${_eccodes_mod_candidates} )
                        if( EXISTS \"${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/\${_mod_candidate}\" )
                            file( COPY \"${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/\${_mod_candidate}\"
                                DESTINATION \"\${MOD_DESTINATION}\" )
                        endif()
                    endforeach()")

    add_custom_command( TARGET     eccodes_f90 POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -D MOD_DESTINATION=${CMAKE_BINARY_DIR}/include -P ${CMAKE_CURRENT_BINARY_DIR}/eccodes_f90_copy_mod.cmake )

    ecbuild_add_resources( TARGET fortran_resources
                           PACK
                            grib_api_constants.h grib_api_externals.h
                            grib_api_visibility.h grib_types.f90 create_grib_f90.sh
                            grib_f90.f90.head grib_f90.f90.tail grib_f90_int.f90 grib_f90_long_int.f90
                            grib_f90_int_size_t.f90 grib_f90_long_size_t.f90

                            eccodes_visibility.h eccodes_constants.h
                            eccodes_f90.f90.head eccodes_f90.f90.tail eccodes_f90_int.f90 eccodes_f90_long_int.f90
                            eccodes_f90_int_size_t.f90 eccodes_f90_long_size_t.f90

                            same_int_long.f90 same_int_size_t.f90 grib_fortran_kinds.c )


### installation

    # Install the contents of the fortran module directory
    if(ECBUILD_INSTALL_FORTRAN_MODULES)
        install( CODE  "execute_process(
                            COMMAND ${CMAKE_COMMAND} -D MOD_DESTINATION=\${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}
                                                     -P ${CMAKE_CURRENT_BINARY_DIR}/eccodes_f90_copy_mod.cmake )" )
    endif()

endif()
