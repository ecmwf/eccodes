ecbuild_add_test( TARGET    new_sample
                  SOURCES   new_sample.c
		          LIBS      grib_api
                  ARGS      "out.grib"
                  ENVIRONMENT "GRIB_SAMPLES_PATH=${PROJECT_SOURCE_DIR}/samples" "GRIB_DEFINITION_PATH=${PROJECT_SOURCE_DIR}/definitions" )

#ecbuild_add_test( TARGET    iterator
#                  SOURCES   iterator.c
# 		          LIBS      grib_api
#                  ARGS      "out.grib"
#                  ENVIRONMENT "GRIB_SAMPLES_PATH=${PROJECT_SOURCE_DIR}/samples" "GRIB_DEFINITION_PATH=${PROJECT_SOURCE_DIR}/definitions" )
