#!/bin/sh
# Copyright 2005-2019 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh
set -u
# ---------------------------------------------------------
# This is the test for the JIRA issue ECC-873.
# It tests grib_ls with the ordering option '-B'
# ---------------------------------------------------------
label="grib_ecc-873-test"
tempRef=temp.${label}.ref
tempOut=temp.${label}.out

# Test 1
# ------
input=${data_dir}/tigge_pf_ecmwf.grib2
${tools_dir}/grib_ls -p count,level -B'level:i' $input > $tempOut

cat > $tempRef <<EOF
count       level
1           0
2           0
3           0
4           0
5           0
6           0
7           0
8           0
9           0
10          0
11          0
12          0
13          0
14          0
15          0
16          0
17          0
18          0
19          0
20          0
21          0
22          0
23          0
24          2
25          2
26          2
27          2
28          2
29          2
30          2
31          10
32          10
33          320
34          925
35          925
36          925
37          925
38          925
EOF

diff -w $tempRef $tempOut

# Test 2
# ------
input=${data_dir}/tigge_pf_ecmwf.grib2
${tools_dir}/grib_ls -p count,level,shortName -B'level:i desc, shortName asc' $input > $tempOut
cat > $tempRef <<EOF
count       level       shortName
1           925         gh
2           925         q
3           925         t
4           925         u
5           925         v
6           320         pv
7           10          10u
8           10          10v
9           2           2d
10          2           2t
11          2           mn2t6
12          2           mx2t6
13          2           pt
14          2           u
15          2           v
16          0           cape
17          0           msl
18          0           sd
19          0           sf
20          0           skt
21          0           slhf
22          0           slhf
23          0           sm
24          0           sp
25          0           sshf
26          0           sshf
27          0           ssr
28          0           ssr
29          0           st
30          0           str
31          0           str
32          0           sund
33          0           tcc
34          0           tcw
35          0           tp
36          0           ttr
37          0           ttr
38          0           u
EOF

# Clean up
rm -f $tempRef $tempOut
