#!/usr/bin/ksh

prov=$1
dest=$2

mkdir -p $dest

for file in $prov/*.html
do
  f=`basename $file`
  cat head.html $file tail.html > $dest/$f
done
