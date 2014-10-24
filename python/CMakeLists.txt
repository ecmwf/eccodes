if( HAVE_PYTHON )

    ecbuild_find_python( VERSION 2.5)

    find_package( SWIG )
    if( SWIG_FOUND )
        include( ${SWIG_USE_FILE} )
    endif()

    # check for Numpy
    find_package( NumPy )


    ###############################################################################
    # swig python interface

    if( HAVE_PYTHON AND SWIG_FOUND AND PYTHONLIBS_FOUND )

        # preparing for generating setup.py -- this may not be needed as cmake can do the swig + shared libs without libtool

        if( NUMPY_FOUND )
            set( DATA_HANDLER "numpy" )
        else()
            set( DATA_HANDLER "array" )
        endif()

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

        swig_add_module( gribapi_swig python grib_interface.h grib_interface.c gribapi_swig.i )

        swig_link_libraries( gribapi_swig grib_api ${PYTHON_LIBRARIES} )

        set(PYTHON_DEST "lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages" )

        install(TARGETS _gribapi_swig DESTINATION ${PYTHON_DEST} )

        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gribapi_swig.py  DESTINATION ${PYTHON_DEST})
        install(FILES gribapi.py DESTINATION ${PYTHON_DEST})

    endif()

endif()
