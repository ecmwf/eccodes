# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

CMAKE_INCLUDE_FILE=include.ctest.sh
if [ -f "$CMAKE_INCLUDE_FILE" ]; then
  # This is the config file for Cmake tests
  . ./$CMAKE_INCLUDE_FILE

else
  set -eax

  echo
  echo "TEST: $0"

  if [ -z "${data_dir}" ]
  then
    cd ../../
    cpath=`pwd`
    ECCODES_DEFINITION_PATH=$cpath/definitions
    export ECCODES_DEFINITION_PATH
    ECCODES_SAMPLES_PATH=$cpath/samples
    export ECCODES_SAMPLES_PATH
    tools_dir=$cpath/tools
    examples_dir=$cpath/examples/F90
    data_dir=$cpath/data
    samples_dir=$cpath/samples

    if test "x$ECCODES_TEST_WITH_VALGRIND" != "x"; then
      tools_dir="valgrind --error-exitcode=1 -q $cpath/tools"
      examples_dir="valgrind --error-exitcode=1 -q $cpath/examples/F90"
    fi

  else
    echo "Skipping test $0"
    exit
  fi

  cd "$cpath/examples/F90"

  if [ -z "${GRIB_API_INCLUDE}" ]
  then 
    GRIB_API_INCLUDE=`pwd`/src
  fi

  if [ -z "${GRIB_API_LIB}" ]
  then 
    GRIB_API_LIB=`pwd`/src
  fi

  # Download the data needed for tests
  ${data_dir}/download.sh "${data_dir}"

  set -u
fi

grib_check_filesize()
(
  # There are tests that try to work on non-existing file.
  # e.g. eccodes_t_grib_bitsPerValue. Check the size if it exists.
  if [ -f "${data_dir}/$1" ]; then
    subdir=`dirname $1`
    realsize=`stat -c %s ${data_dir}/$1`
    expected=`grep " $1$" ${data_dir}/${subdir}/filesize_db.txt | awk -F " " '{print $1}'`
    if [ "$realsize" != "$expected" ]; then
      echo Data file \"$1\" does not have the expected size, $expected, which is from http://download.ecmwf.org/test-data/eccodes/data/.
      exit 1
    fi
  fi
)
