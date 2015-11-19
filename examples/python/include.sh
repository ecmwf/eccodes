CMAKE_INCLUDE_FILE=include.ctest.sh
if [ -f "$CMAKE_INCLUDE_FILE" ]; then
  # This is the config file for Cmake tests
  . ./$CMAKE_INCLUDE_FILE

else
  set -ea
  echo
  echo "TEST: $0"

  # If this environment variable is set, then become verbose
  # so one can see why and how a test failed
  if test "x$ECCODES_TEST_VERBOSE_OUTPUT" != "x"; then
     set -x
  fi

  cpath=$TOPBUILDDIR
  ECCODES_DEFINITION_PATH=$cpath/definitions
  export ECCODES_DEFINITION_PATH
  ECCODES_SAMPLES_PATH=$cpath/samples
  export ECCODES_SAMPLES_PATH
  tools_dir=$cpath/tools/
  examples_dir=$cpath/examples/python
  data_dir=$cpath/data
  examples_src=$examples_dir

  PYTHONPATH=$cpath/python:$cpath/python/.libs:$PYTHONPATH
  export PYTHONPATH

  # Download the data needed for tests
  ${data_dir}/download.sh "${data_dir}"

  set -u

fi
