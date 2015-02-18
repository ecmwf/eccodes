file( GLOB samples_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.tmpl" )

install( FILES ${samples_files}
         DESTINATION share/${PROJECT_NAME}/samples
         PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ )

foreach( tmpl ${samples_files} )
    list( APPEND ECCODES_SAMPLES_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${tmpl} )
endforeach() 

set( ECCODES_SAMPLES_DIR   ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE )
set( ECCODES_SAMPLES_FILES ${ECCODES_SAMPLES_FILES}    PARENT_SCOPE )

# copy the samples to the build directory

file(    COPY ${samples_files}
		 DESTINATION ${CMAKE_BINARY_DIR}/share/${PROJECT_NAME}/samples )
