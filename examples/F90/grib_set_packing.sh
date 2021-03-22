#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

. ./include.sh

tempSimple=temp.f_grib_set_packing.simple.grib
tempSecond=temp.f_grib_set_packing.second.grib
tempCCSDS=temp.f_grib_set_packing.ccsds.grib

${examples_dir}/eccodes_f_grib_set_packing grid_simple       $tempSimple
${examples_dir}/eccodes_f_grib_set_packing grid_second_order $tempSecond
${examples_dir}/eccodes_f_grib_set_packing grid_ccsds        $tempCCSDS

${tools_dir}/grib_ls -P packingType -n statistics $tempSimple $tempCCSDS $tempSecond

p=`${tools_dir}/grib_get -p packingType $tempSimple`
[ "$p" = "grid_simple" ]
p=`${tools_dir}/grib_get -p packingType $tempSecond`
[ "$p" = "grid_second_order" ]
p=`${tools_dir}/grib_get -p packingType $tempCCSDS`
[ "$p" = "grid_ccsds" ]

${tools_dir}/grib_compare -c data:n $tempSimple $tempCCSDS
${tools_dir}/grib_compare -c data:n $tempSimple $tempSecond


rm -f $tempSimple $tempCCSDS
