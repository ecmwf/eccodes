add_custom_command(
  OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/memfs.c
  COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/memfs.py
    ${PROJECT_SOURCE_DIR}/definitions
    ${PROJECT_SOURCE_DIR}/samples
    ${PROJECT_SOURCE_DIR}/ifs_samples
    ${CMAKE_CURRENT_BINARY_DIR}/memfs.c
  DEPENDS ${PROJECT_SOURCE_DIR}/memfs.py )

set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/memfs.c PROPERTIES OBJECT_DEPENDS
    "${PROJECT_SOURCE_DIR}/memfs.py"
    # "${PROJECT_SOURCE_DIR}/memfs.py" ${definition_files}"
    )

# No debug symbols on Cray (workaround for HPC-230)
if( CMAKE_C_COMPILER_ID MATCHES Cray )
  string( REGEX REPLACE "-g|-G[ 0-3]" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" )
  string( REGEX REPLACE "-g|-G[ 0-3]" " " CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_CAPS} "${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_CAPS}}" )
endif()

if (HAVE_MEMFS)
    ecbuild_add_library(TARGET    eccodes_memfs
                        SOURCES   ${CMAKE_CURRENT_BINARY_DIR}/memfs.c
                        GENERATED ${CMAKE_CURRENT_BINARY_DIR}/memfs.c)
endif()
