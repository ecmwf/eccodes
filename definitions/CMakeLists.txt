file( GLOB definition_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.def" )

set(definition_files ${definition_files} PARENT_SCOPE) # needed for memfs

file( GLOB table_files      RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.table" )
file( GLOB text_files       RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.txt" )

# Collect all directories at the top-level of definitions
file( GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*" )
set( dirlist "" )
foreach( child ${children} )
  if( IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child} )
      set( add_child 1 )
      if( NOT HAVE_PRODUCT_BUFR AND child STREQUAL bufr )
        set( add_child 0 )
      endif()
      if( NOT HAVE_PRODUCT_GRIB AND child MATCHES "^grib" )
        set( add_child 0 )
      endif()
      if( add_child )
        list( APPEND dirlist ${child} )
      endif()
  endif()
endforeach()

if( HAVE_INSTALL_ECCODES_DEFINITIONS )
  install( FILES ${definition_files} ${table_files} ${text_files}
           DESTINATION ${ECCODES_DEFINITION_SUFF}
           PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ )

  install( FILES installDefinitions.sh
           DESTINATION ${ECCODES_DEFINITION_SUFF} )

  install( DIRECTORY ${dirlist}
           DESTINATION ${ECCODES_DEFINITION_SUFF}
           FILES_MATCHING
           PATTERN "*.def"
           PATTERN "*.txt"
           PATTERN "*.list"
           PATTERN "*.table"
           PATTERN "4.2.192.*.table" EXCLUDE
           PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ )
endif()

# link to the definitions. See GRIB-786
file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${INSTALL_DATA_DIR} )
if( NOT EXISTS "${CMAKE_BINARY_DIR}/${ECCODES_DEFINITION_SUFF}" )
  execute_process( COMMAND "${CMAKE_COMMAND}" "-E" "create_symlink"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_BINARY_DIR}/${ECCODES_DEFINITION_SUFF}" )
endif()

# copy the definitions to the build directory
#file(    COPY ${definition_files} ${table_files} ${text_files}
#         DESTINATION ${CMAKE_BINARY_DIR}/${ECCODES_DEFINITION_SUFF} )
#file(COPY budg bufr cdf common grib1 grib2 grib3 gts mars metar tide hdf5 wrap
#     DESTINATION ${CMAKE_BINARY_DIR}/${ECCODES_DEFINITION_SUFF}
#     FILES_MATCHING
#     PATTERN "*.def"
#     PATTERN "*.txt"
#     PATTERN "*.list"
#     PATTERN "*.table"
#     PATTERN "4.2.192.*.table" EXCLUDE )
