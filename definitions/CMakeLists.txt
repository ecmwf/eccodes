file( GLOB definition_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.def" )
file( GLOB table_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.table" )

install( FILES ${definition_files} ${table_files}  
         DESTINATION share/${PROJECT_NAME}/definitions
         PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ )

install(  DIRECTORY budg cdf common grib1 grib2 gts mars tide
          DESTINATION share/${PROJECT_NAME}/definitions
          FILES_MATCHING 
          PATTERN "*.def"
          PATTERN "*.table"
          PATTERN "4.2.192.*.table" EXCLUDE
          PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ )

# copy the definitions to the build directory

file(    COPY ${definition_files} ${table_files}
         DESTINATION ${CMAKE_BINARY_DIR}/share/${PROJECT_NAME}/definitions )

file(     COPY budg cdf common grib1 grib2 gts mars tide
          DESTINATION ${CMAKE_BINARY_DIR}/share/${PROJECT_NAME}/definitions
          FILES_MATCHING
          PATTERN "*.def"
          PATTERN "*.table"
          PATTERN "4.2.192.*.table" EXCLUDE )
