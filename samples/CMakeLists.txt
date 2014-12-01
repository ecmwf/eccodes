file( GLOB samples_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.tmpl" )

install( FILES ${samples_files}
         DESTINATION share/${PROJECT_NAME}/samples
         PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ )

foreach( tmpl ${samples_files} )
    list( APPEND GRIB_API_SAMPLES_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${tmpl} )
endforeach() 

set( GRIB_API_SAMPLES_DIR   ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE )
set( GRIB_API_SAMPLES_FILES ${GRIB_API_SAMPLES_FILES}   PARENT_SCOPE )

# copy the samples to the build directory

file(    COPY ${samples_files}
		 DESTINATION ${CMAKE_BINARY_DIR}/share/${PROJECT_NAME}/samples )
