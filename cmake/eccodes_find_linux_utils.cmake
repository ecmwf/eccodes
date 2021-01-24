# (C) Copyright 2011- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

function( eccodes_find_linux_util _p_UTIL )
    find_program( FOUND_PROGRAM ${_p_UTIL} )
    if( NOT FOUND_PROGRAM )
        ecbuild_critical( "Failed to find linux util: ${_p_UTIL}" )
    endif()
endfunction( eccodes_find_linux_util )

if( ENABLE_TESTS )
    # These utils are required for the tests to run.
    # To install them on Windows, install the appropriate m2-* conda package.
    # e.g. for bash: `conda install -msys2 m2-bash`
    # Make sure to activate the conda environment so the utils are in the system path.
    set( REQUIRED_UTILS bash find grep sed gawk diff )

    foreach( UTIL ${REQUIRED_UTILS} )
        eccodes_find_linux_util( ${UTIL} )
    endforeach()
endif()
