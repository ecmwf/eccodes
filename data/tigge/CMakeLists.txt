# data/tigge/CMakeLists.txt
# Download all the TIGGE grib data from website
#
FILE(READ "tigge_data_files.txt" tigge_files_to_download)
STRING(REGEX REPLACE "\n" ";" tigge_files_to_download "${tigge_files_to_download}")

# Download all data files doing md5 check on each
ecbuild_get_test_multidata( TARGET get_tigge_gribs
                            NAMES ${tigge_files_to_download}
)
