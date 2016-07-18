if( HAVE_PYTHON )

    #find_package( SWIG )
    #if( SWIG_FOUND )
    #    include( ${SWIG_USE_FILE} )
    #endif()

    ###############################################################################
    # swig python interface

    #if( SWIG_FOUND AND PYTHONLIBS_FOUND )
    if( PYTHONLIBS_FOUND )

        # preparing for generating setup.py -- this may not be needed as cmake can do the swig + shared libs without libtool

        if( HAVE_LIBJASPER )
            get_filename_component(JASPER_DIR ${JASPER_INCLUDE_DIR} PATH )
        endif()

        if( HAVE_LIBOPENJPEG )
            get_filename_component(OPENJPEG_DIR ${OPENJPEG_INCLUDE_DIR} PATH )
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
        set( _gribapi_swig "_gribapi_swig${CMAKE_SHARED_LIBRARY_SUFFIX}" )
        # Build the extension module for use in build tree with RPATH pointing to the build tree
        add_custom_command( OUTPUT ${_gribapi_swig}
                            COMMAND ${PYTHON_EXECUTABLE} setup.py build_ext --inplace -R ${CMAKE_BINARY_DIR}/lib
                            DEPENDS grib_interface.h grib_interface.c swig_wrap_numpy.c setup.py.in eccodes )
        add_custom_target(build_swig_wrapper ALL DEPENDS ${_gribapi_swig})

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

        # Call distutils for installation
        install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} setup.py install --prefix ${CMAKE_INSTALL_PREFIX} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})")

    endif()

endif()
