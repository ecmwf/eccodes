if( HAVE_PYTHON )

    ecbuild_find_python( VERSION 2.5)

    #find_package( SWIG )
    #if( SWIG_FOUND )
    #    include( ${SWIG_USE_FILE} )
    #endif()

    # check for Numpy
    find_package( NumPy )

    ###############################################################################
    # swig python interface

    #if( HAVE_PYTHON AND SWIG_FOUND AND PYTHONLIBS_FOUND )
    if( HAVE_PYTHON AND PYTHONLIBS_FOUND )

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

        # Invoke swig to generate the C wrapper
        #  swig_add_module( gribapi_swig python grib_interface.h grib_interface.c gribapi_swig.i )
        #  swig_link_libraries( gribapi_swig grib_api ${PYTHON_LIBRARIES} )

        ####### Do not invoke swig. Use our own generated C wrapper file ######
        ecbuild_add_library(TARGET    _gribapi_swig
                            TYPE SHARED
                            NOINSTALL
                            SOURCES   grib_interface.h grib_interface.c ${_swig_c_wrapper}
                            LIBS      eccodes ${PYTHON_LIBRARIES} )
        # Don't use the lib prefix.  This is needed for the python case where a _modulename.so is generated
        set_target_properties(_gribapi_swig PROPERTIES PREFIX "")

        # Copy the wrapper python file to build area
        set( _swig_py  gribapi_swig.py )
        add_custom_command(
           OUTPUT ${_swig_py}
           COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${_swig_py_wrapper} ${CMAKE_CURRENT_BINARY_DIR}/${_swig_py}
           DEPENDS _gribapi_swig
        )
        add_custom_target(copy_swig_py ALL DEPENDS ${_swig_py})
        #######################

        set(PYTHON_DEST "lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages/grib_api" )

        install(TARGETS _gribapi_swig DESTINATION ${PYTHON_DEST} )

        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gribapi_swig.py  DESTINATION ${PYTHON_DEST})
        install(FILES gribapi.py DESTINATION ${PYTHON_DEST})
        install(FILES eccodes.py DESTINATION ${PYTHON_DEST})

    endif()

endif()
