set( generated_c_files
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_000.c
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_001.c
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_002.c
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_003.c
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_004.c
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_005.c
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_006.c
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_final.c)

set( exclude "" )
if( NOT HAVE_PRODUCT_BUFR )
  set( exclude -exclude bufr )
  set( generated_c_files
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_000.c
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_001.c
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_002.c
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_003.c
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_004.c
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_final.c)
elseif( NOT HAVE_PRODUCT_GRIB )
  set( exclude -exclude grib )
  set( generated_c_files
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_000.c
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_001.c
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_002.c
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen_final.c)
endif()

add_custom_command(
  OUTPUT ${generated_c_files}
  COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/memfs.py
    ${exclude}
    ${PROJECT_SOURCE_DIR}/definitions
    ${PROJECT_SOURCE_DIR}/samples
    ${PROJECT_SOURCE_DIR}/ifs_samples
    ${CMAKE_CURRENT_BINARY_DIR}/memfs_gen
  DEPENDS ${PROJECT_SOURCE_DIR}/memfs.py)

set_source_files_properties(
    ${generated_c_files} PROPERTIES OBJECT_DEPENDS
    "${PROJECT_SOURCE_DIR}/memfs.py"
    # "${PROJECT_SOURCE_DIR}/memfs.py" ${definition_files}"
    )

# No debug symbols on Cray (workaround for HPC-230)
if( CMAKE_C_COMPILER_ID MATCHES Cray )
  string( REGEX REPLACE "-g|-G[ 0-3]" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" )
  string( REGEX REPLACE "-g|-G[ 0-3]" " " CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_CAPS} "${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_CAPS}}" )
endif()

if( HAVE_MEMFS )
    ecbuild_add_library(TARGET    eccodes_memfs
                        SOURCES   ${generated_c_files}
                        GENERATED ${generated_c_files})
endif()
