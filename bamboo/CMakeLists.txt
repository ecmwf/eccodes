file( GLOB_RECURSE bamboo_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*" )

ecbuild_add_resources(  TARGET ${PROJECT_NAME}_bamboo
						SOURCES_DONT_PACK ${bamboo_files} )
