set -ea

echo
echo "TEST: $0"

if [ -z "${data_dir}" ]
then
  cd ../
  cpath=`pwd`
  GRIB_DEFINITION_PATH=$cpath/definitions
  export GRIB_DEFINITION_PATH
  GRIB_TEMPLATES_PATH=$cpath/templates
  export GRIB_TEMPLATES_PATH
  tools_dir=$cpath/tools/
  examples_dir=$cpath/examples/
  data_dir=$cpath/data
else
  echo "Skipping test $0"
  exit
fi

cd ${examples_dir}

if [ -z "${GRIB_API_INCLUDE}" ]
then 
  GRIB_API_INCLUDE=`pwd`/src
fi

if [ -z "${GRIB_API_LIB}" ]
then 
  GRIB_API_LIB=`pwd`/src
fi

#${tools_dir}grib_info

set -u

