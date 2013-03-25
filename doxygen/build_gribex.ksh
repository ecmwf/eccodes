#!/usr/bin/ksh

prov=$1
dest=$2

mkdir -p $dest
p4 edit $dest/*.html

for file in $prov/*.html
do
  f=`basename $file`
  cat head.html $file tail.html > $dest/$f
done

p4 add $dest/*.html

