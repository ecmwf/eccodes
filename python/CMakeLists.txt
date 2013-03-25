# check for Numpy

if(PYTHON_EXECUTABLE)

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/numpy_path.py "try: import numpy; print numpy.get_include()\nexcept: pass\n")
    exec_program( ${PYTHON_EXECUTABLE}
                   ARGS "\"${CMAKE_CURRENT_BINARY_DIR}/determineNumpyPath.py\""
                   OUTPUT_VARIABLE NUMPY_PATH
                 )
endif()

find_path( PYTHON_NUMPY_INCLUDE_DIR arrayobject.h
          "${NUMPY_PATH}/numpy/"
          "${PYTHON_INCLUDE_PATH}/numpy/"
          /usr/include/python2.6/numpy/
          /usr/include/python2.5/numpy/
          /usr/include/python2.4/numpy/
          /usr/include/python2.3/numpy/
          DOC "Directory where the arrayobject.h header file can be found. This file is part of the numpy package"
)

if( PYTHON_NUMPY_INCLUDE_DIR )
  set(PYTHON_NUMPY_FOUND 1 CACHE INTERNAL "Python numpy development package is available")
endif()

###############################################################################
# swig python interface

if( GRIB_API_PYTHON AND SWIG_FOUND AND PYTHONLIBS_FOUND )

    # preparing for generating setup.py

    if( PYTHON_NUMPY_FOUND )
      set( DATA_HANDLER "numpy" )
    else()
      set( DATA_HANDLER "array" )
    endif()

    if( PNG_LIBRARIES )
      set( LIB_PNG "${PNG_LIBRARIES}" )
    else()
      set( LIB_PNG "" )
    endif()

   include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

   # do the smae for JASPER and OPENJPEG

    configure_file( setup.py.in setup.py )

    set(CMAKE_SWIG_FLAGS "")

    include_directories( ${PYTHON_INCLUDE_PATH} )

    set_source_files_properties( gribapi_swig.i PROPERTIES C ON )

    swig_add_module( gribapi_swig python gribapi_swig.i )

    swig_link_libraries( gribapi_swig grib_api ${PYTHON_LIBRARIES} )

    set(PYTHON_DEST "lib/python${PYTHON_VERSION}/site-packages" )
    install(TARGETS _gribapi_swig DESTINATION ${PYTHON_DEST} )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gribapi_swig.py  DESTINATION ${PYTHON_DEST})

endif()
