#!/bin/sh
set -ex
cat > accessor_class_list.gperf <<EOF
%{
#include "grib_api_internal.h"
#include "grib_accessor_class.h"

%}
struct accessor_class_hash { const char *name; grib_accessor **cclass;};
%%
EOF

cat grib_accessor_factory_hash_list | sed 's/\/\*/#/g' >> accessor_class_list.gperf

# editing grib_accessor_classes_hash.cc

gperf --version
echo

gperf -L C++ -C -W classes -t -G -H get_id -Z AccessorHash -N get_builder -m 1 -j 1 accessor_class_list.gperf > grib_accessor_classes_hash.h

rm -f accessor_class_list.gperf
