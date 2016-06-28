#!/bin/ksh
set -ex
set -A tools grib_dump grib_ls grib_get grib_copy grib_set grib_filter grib_compare grib_get_data grib_index_build
export DOXYGEN_USAGE=1

cat tools_head.dox > tools.dox

for tool in ${tools[@]}
do 
  set +e
  ./$tool > ${tool}.dox
  set -e

  cat >> ${tool}.dox <<EOF

\section ${tool}_examples ${tool} examples
EOF

  ./${tool}.sh >> ${tool}.dox

  cat >> ${tool}.dox <<EOF
*/
EOF

  cat >> tools.dox <<EOF
- \ref ${tool}
EOF

done

cat >> tools.dox <<EOF
*/
EOF
