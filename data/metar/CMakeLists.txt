# data/metar/CMakeLists.txt
# Download all the METAR data and reference files from website
#

FILE(READ "metar_data_files.txt" metar_files_to_download)
# Convert file contents into a CMake list (where each element in the list
# is one line of the file)
STRING(REGEX REPLACE "\n" ";" metar_files_to_download "${metar_files_to_download}")


FILE(READ "metar_ref_files.txt" metar_refs_to_download)
STRING(REGEX REPLACE "\n" ";" metar_refs_to_download "${metar_refs_to_download}")


ecbuild_get_test_multidata(
     TARGET eccodes_download_metars
     NOCHECK
     NAMES ${metar_files_to_download} ${metar_refs_to_download}
)

# Copy other files - e.g. text files etc from the source data/metar dir
LIST(APPEND other_files
    metar_data_files.txt
    metar_ref_files.txt
)

foreach( file ${other_files} )
   execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${file} ${CMAKE_CURRENT_BINARY_DIR} )
endforeach()
