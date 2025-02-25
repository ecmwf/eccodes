set -ea

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
  examples_dir=$cpath/examples/extra
  data_dir=$cpath/data
else
  echo "Skipping test $0"
  exit
fi

cd ${examples_dir}

if [ -z "${GRIB_API_INCLUDE}" ]
then 
  GRIB_API_INCLUDE=`pwd`/src/eccodes
fi

if [ -z "${GRIB_API_LIB}" ]
then 
  GRIB_API_LIB=`pwd`/src/eccodes
fi

set -u
