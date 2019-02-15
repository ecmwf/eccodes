if( HAVE_PYTHON )

    #find_package( SWIG )
    #if( SWIG_FOUND )
    #    include( ${SWIG_USE_FILE} )
    #endif()

    # preparing for generating setup.py
    if( HAVE_LIBJASPER )
        get_filename_component(JASPER_DIR ${JASPER_INCLUDE_DIR} PATH )
    endif()

    if( HAVE_LIBOPENJPEG )
        # Get the name of the OpenJPEG library: version dependent!
        # E.g. openjpeg (version 1.5) or openjp2 (version 2.x)
        get_filename_component(OPENJPEG_LIB_DIR ${OPENJPEG_LIBRARY} PATH )
        get_filename_component(OJ_NAME_WE       ${OPENJPEG_LIBRARY} NAME_WE )
        STRING(REGEX REPLACE "^lib" "" OJ_WITHOUT_LIB ${OJ_NAME_WE})
    endif()

    if( HAVE_AEC )
      get_filename_component(AEC_DIR ${AEC_INCLUDE_DIR} PATH )
    endif()

    configure_file( setup.py.in setup.py )

    # compile swig interface
    # set(CMAKE_SWIG_FLAGS "")
    # set_source_files_properties( gribapi_swig.i PROPERTIES C ON )

    # if( NUMPY_FOUND )
    #     set( CMAKE_SWIG_FLAGS "-DNUMPY" )
    #     include_directories( ${NUMPY_INCLUDE_DIRS} )
    # endif()

    ####### Do not invoke swig. Use our own generated C wrapper file ######
    set( _gribapi_swig "gribapi/_gribapi_swig${CMAKE_SHARED_LIBRARY_SUFFIX}" )
    # Build the extension module for use in build tree with RPATH pointing to the build tree
    add_custom_command( OUTPUT ${_gribapi_swig}
                        COMMAND ${PYTHON_EXECUTABLE} setup.py build_ext --inplace --rpath ${CMAKE_BINARY_DIR}/lib
                        DEPENDS grib_interface.h grib_interface.c swig_wrap_numpy.c setup.py.in eccodes )
    add_custom_target(eccodes_build_swig_wrapper ALL DEPENDS ${_gribapi_swig})

    #ecbuild_add_library(TARGET    _gribapi_swig
    #                    TYPE SHARED
    #                    NOINSTALL
    #                    SOURCES   grib_interface.h grib_interface.c swig_wrap_numpy.c
    #                    INCLUDES  ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${PYTHON_INCLUDE_DIRS}
    #                    LIBS      eccodes ${PYTHON_LIBRARIES} )
    # Don't use the lib prefix.  This is needed for the python case where a _modulename.so is generated
    #set_target_properties(_gribapi_swig PROPERTIES PREFIX "")

    # Copy gribapi and eccodes python modules to build area
    file( COPY eccodes gribapi DESTINATION . )
    configure_file( swig_wrap_numpy.py gribapi/gribapi_swig.py COPYONLY )

    # Build the extension module for use in install tree with RPATH pointing to install tree
    install(CODE "message(\"Building Python extension modules:
${PYTHON_EXECUTABLE} setup.py build_ext --rpath ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR}\")
                  execute_process(COMMAND ${PYTHON_EXECUTABLE} setup.py build_ext --rpath ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR}
                                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})")

    # Library installation directory override
    if( NOT INSTALL_LIB_DIR STREQUAL lib )
      execute_process( COMMAND ${PYTHON_EXECUTABLE} -c "from __future__ import print_function; import sys; print(sys.version[:3], end='')"
                       OUTPUT_VARIABLE PYVER )
      set( __install_lib "--install-lib=${ECCODES_FULL_INSTALL_LIB_DIR}/python${PYVER}/site-packages" )
    endif()

    # Call distutils for installation
    install(CODE "if( NOT \$ENV{DESTDIR} STREQUAL \"\" )
                    set( __root \"--root=\$ENV{DESTDIR}\" )
                  endif()
                  message(\"Installing Python modules:
${PYTHON_EXECUTABLE} setup.py install \${__root}
                                      ${__install_lib}
                                      --prefix=${CMAKE_INSTALL_PREFIX}
                                      --record=${CMAKE_BINARY_DIR}/extra_install.txt\")
                  execute_process(COMMAND ${PYTHON_EXECUTABLE} setup.py install
                                            \${__root}
                                            --prefix=${CMAKE_INSTALL_PREFIX}
                                            ${__install_lib}
                                            --record=${CMAKE_BINARY_DIR}/extra_install.txt
                                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})")

endif()
