CMAKE_INCLUDE_FILE=include.ctest.sh
if [ -f "$CMAKE_INCLUDE_FILE" ]; then
  # This is the config file for Cmake tests
  . ./$CMAKE_INCLUDE_FILE

else
  set -ea
  echo
  echo "TEST: $0"

  cpath=$TOPBUILDDIR
  GRIB_DEFINITION_PATH=$cpath/definitions
  export GRIB_DEFINITION_PATH
  GRIB_SAMPLES_PATH=$cpath/samples
  export GRIB_SAMPLES_PATH
  tools_dir=$cpath/tools/
  examples_dir=$cpath/examples/python
  data_dir=$cpath/data

  PYTHONPATH=$cpath/python:$cpath/python/.libs:$PYTHONPATH
  export PYTHONPATH

  # Download the data needed for tests
  ${data_dir}/download.sh "${data_dir}"

  set -u

fi
