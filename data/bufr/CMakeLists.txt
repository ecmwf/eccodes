# data/bufr/CMakeLists.txt
# Download all the BUFR data and reference files from website
#

FILE(READ "bufr_data_files.txt" bufr_files_to_download)
# Convert file contents into a CMake list (where each element in the list
# is one line of the file)
STRING(REGEX REPLACE "\n" ";" bufr_files_to_download "${bufr_files_to_download}")


FILE(READ "bufr_ref_files.txt" bufr_refs_to_download)
STRING(REGEX REPLACE "\n" ";" bufr_refs_to_download "${bufr_refs_to_download}")

# Exceptional case: download bufr files which have to be treated specially
LIST(APPEND bufr_refs_to_download "vos308014_v3_26.bufr")  # See test ecc-197

ecbuild_get_test_multidata(
     TARGET eccodes_download_bufrs
     NOCHECK
     NAMES ${bufr_files_to_download} ${bufr_refs_to_download}
)

# Copy other files - e.g. text files etc from the source data/bufr dir
LIST(APPEND other_files
    bufr_data_files.txt
    bufr_ref_files.txt
)

foreach( file ${other_files} )
   execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${file} ${CMAKE_CURRENT_BINARY_DIR} )
endforeach()
