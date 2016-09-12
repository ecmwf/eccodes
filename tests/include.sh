# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#
set -ea

. ./utils.sh

CMAKE_INCLUDE_FILE=include.ctest.sh
if [ -f "$CMAKE_INCLUDE_FILE" ]; then
  # This is the config file for Cmake tests
  . ./$CMAKE_INCLUDE_FILE

else
  # This is for autotools
  echo
  echo "TEST: $0"

  data_dir=""
  # save current working dir
  save=`pwd`

  # If this environment variable is set, then become verbose
  # so one can see why and how a test failed
  if test "x$ECCODES_TEST_VERBOSE_OUTPUT" != "x"; then
     set -x
  fi

  if [ -z "${data_dir}" ]
  then
    cd ../
    cpath=`pwd`
    proj_dir=$cpath
    ECCODES_DEFINITION_PATH=$cpath/definitions
    export ECCODES_DEFINITION_PATH
    ECCODES_SAMPLES_PATH=$cpath/samples
    export ECCODES_SAMPLES_PATH
    tools_dir=$cpath/tools/

#tools_dir="valgrind --error-exitcode=1 -q $cpath/tools/"

    tigge_dir=$cpath/tigge/
    data_dir=$cpath/data
    test_dir=$cpath/tests/
    def_dir=$cpath/definitions
    src_dir=$cpath/src
  else
    tools_dir=""
    tigge_dir=""
  fi

  if [ -z "${GRIB_API_INCLUDE}" ]
  then 
    GRIB_API_INCLUDE=`pwd`/src
  fi

  if [ -z "${GRIB_API_LIB}" ]
  then 
    GRIB_API_LIB=`pwd`/src
  fi

  # go back to current working dir
  cd $save
  set -u
fi

