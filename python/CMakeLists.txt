if( HAVE_PYTHON )

    #find_package( SWIG )
    #if( SWIG_FOUND )
    #    include( ${SWIG_USE_FILE} )
    #endif()

    # check for Numpy
    find_package( NumPy )

    ###############################################################################
    # swig python interface

    #if( SWIG_FOUND AND PYTHONLIBS_FOUND )
    if( PYTHONLIBS_FOUND )

        set(PYTHON_DEST_ECCODES "${INSTALL_LIB_DIR}/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages/eccodes" )
        set(PYTHON_DEST_GRIBAPI "${INSTALL_LIB_DIR}/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages/gribapi" )

        file( RELATIVE_PATH relative_rpath "${CMAKE_INSTALL_PREFIX}/${PYTHON_DEST_ECCODES}" "${${PNAME}_FULL_INSTALL_LIB_DIR}" )

        # debug_var( relative_rpath )

        ecbuild_append_to_rpath( ${relative_rpath} )
        ecbuild_append_to_rpath( "../../lib" )        # for ctest to find the libs

        # preparing for generating setup.py -- this may not be needed as cmake can do the swig + shared libs without libtool

        if( NUMPY_FOUND )
            set( DATA_HANDLER "numpy" )
        else()
            set( DATA_HANDLER "array" )
        endif()
        set( _swig_c_wrapper  swig_wrap_${DATA_HANDLER}.c )
        set( _swig_py_wrapper swig_wrap_${DATA_HANDLER}.py )

        set( BUILD_DIR "${PROJECT_SOURCE_DIR}" ) # setup.py assumes build_dir is same as source_dir -- not true for cmake builds

        set( LIB_PNG "${HAVE_LIBPNG}" )

        set( LIB_JASPER ${HAVE_LIBJASPER} )
        if( HAVE_LIBJASPER )
            get_filename_component(JASPER_DIR ${JASPER_INCLUDE_DIR} PATH )
        endif()

        set( LIB_OPENJPEG ${HAVE_LIBOPENJPEG} )
        if( HAVE_LIBOPENJPEG )
            get_filename_component(OPENJPEG_DIR ${OPENJPEG_INCLUDE_DIR} PATH )
        endif()

        include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${PYTHON_INCLUDE_DIRS} )

        configure_file( setup.py.in setup.py )

        # compile swig interface

        set(CMAKE_SWIG_FLAGS "")

        include_directories( ${PYTHON_INCLUDE_PATH} )

        set_source_files_properties( gribapi_swig.i PROPERTIES C ON )

        if( NUMPY_FOUND )
            set( CMAKE_SWIG_FLAGS "-DNUMPY" )
            include_directories( ${NUMPY_INCLUDE_DIRS} )
        endif()

        ####### Do not invoke swig. Use our own generated C wrapper file ######
        set( _gribapi_swig "_gribapi_swig${CMAKE_SHARED_LIBRARY_SUFFIX}" )
        # Build the extension module for use in build tree with RPATH pointing to the build tree
        add_custom_command( OUTPUT ${_gribapi_swig}
                            COMMAND ${PYTHON_EXECUTABLE} setup.py build_ext --inplace -R ${CMAKE_BINARY_DIR}/lib
                            DEPENDS grib_interface.h grib_interface.c ${_swig_c_wrapper} setup.py.in eccodes )
        add_custom_target(build_swig_wrapper ALL DEPENDS ${_gribapi_swig})

        #ecbuild_add_library(TARGET    _gribapi_swig
        #                    TYPE SHARED
        #                    NOINSTALL
        #                    SOURCES   grib_interface.h grib_interface.c ${_swig_c_wrapper}
        #                    LIBS      eccodes ${PYTHON_LIBRARIES} )
        # Don't use the lib prefix.  This is needed for the python case where a _modulename.so is generated
        #set_target_properties(_gribapi_swig PROPERTIES PREFIX "")

        # Copy gribapi and eccodes python modules to build area
        file( COPY eccodes gribapi DESTINATION . )
        configure_file( ${_swig_py_wrapper} gribapi/gribapi_swig.py COPYONLY )

        # Call distutils for installation
        install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} setup.py install --prefix ${CMAKE_INSTALL_PREFIX} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})")

    endif()

endif()
